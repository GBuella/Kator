
#include "game_state.h"

#include <memory>
#include <sstream>
#include <cstdlib>
#include <functional>

using ::std::unique_ptr;
using ::std::make_unique;
using ::std::string;
using ::std::istream;


namespace kator
{

game_state::game_state(const kator::position& position_arg,
                       unsigned half_moves_arg,
                       unsigned full_moves_arg,
                       real_player real_player_arg,
                       sq_index ep_target):
  game_state(make_unique<kator::position>(position_arg),
             half_moves_arg,
             full_moves_arg,
             real_player_arg,
             ep_target)
{
}

game_state::game_state(unique_ptr<const kator::position> position_arg,
                       unsigned half_moves_arg,
                       unsigned full_moves_arg,
                       real_player real_player_arg,
                       sq_index ep_target):
  position(std::move(position_arg)),
  turn(real_player_arg),
  whites_view((turn == white) ? player_to_move : player_opponent),
  moves(*position, turn),
  has_any_legal_moves(moves.count() != 0),
  half_moves(half_moves_arg),
  full_moves(full_moves_arg),
  in_check(position->in_check()),
  is_check_mate(in_check and not has_any_legal_moves),
  is_stale_mate((not has_any_legal_moves) and (not in_check)),
  can_flip(not position->has_en_passant_square()),
  en_passant_target_square(ep_target)
{
}

game_state::game_state(const game_state& other):
  game_state(*other.position,
             other.half_moves, other.full_moves,
             other.turn, other.en_passant_target_square)
{
}

namespace
{

static bitboard
gen_ambiguous_map(const position& position, sq_index from, sq_index to)
{
  bitboard ambiguous_pieces = bitboard::empty();

  for (auto move : move_list(position)) {
    if (move.from == from || move.to != to) {
      continue;
    }
    if (position.piece_at(from) == position.piece_at(move.from)) {
      ambiguous_pieces.set_bit(move.from);
    }
  }
  return ambiguous_pieces;
}

static string
SAN_move_from(const game_state* state, sq_index from, sq_index to)
{
  bitboard ambiguous_pieces = gen_ambiguous_map(*state->position, from, to);
  string result;

  if (!ambiguous_pieces.is_empty()) {
    if (!(ambiguous_pieces & bitboard(from.file())).is_empty()) {
      if (!(ambiguous_pieces & bitboard(to.rank())).is_empty()) {
        result += from.to_str(state->whites_view);
      }
      else {
        result += from.rank().to_str(state->whites_view);
      }
    }
    else {
      result += from.file().to_str();
    }
  }
  return result;
}

static void mark_check(const game_state* state, string& result, move move)
{
  kator::position child(*state->position, move);

  if (child.in_check()) {
    result += "+";
  }
}

static void mark_promotion(string& result, move move, bool use_figurines)
{
  if (move.is_promotion()) {
    result += "=";
    result += real_piece(move.result(), white).to_str(use_figurines);
  }
}

static void mark_en_passant(string& result, move move)
{
  if (move.is_en_passant()) {
    result += "e.p.";
  }
}

static void mark_capture(string& result, move move)
{
  if (move.is_capture()) {
    result += "x";
  }
}

static string
print_algebraic_move(const game_state* state, move move, bool use_figurines)
{
  if (move.is_castle_queenside()) {
    return "O-O-O";
  }
  else if (move.is_castle_kingside()) {
    return "O-O";
  }
  else {
    string result;
    real_piece actor = state->piece_at(move.from);

    if (state->turn == black) {
      move.flip();
    }
    if (actor.type == piece::pawn) {
      if (move.is_capture()) {
        result = move.from.file().to_str();
      }
    }
    else {
      result += actor.to_str(use_figurines);
      result += SAN_move_from(state, move.from, move.to);
    }
    mark_capture(result, move);
    result += move.to.to_str(state->whites_view);
    mark_promotion(result, move, use_figurines);
    mark_en_passant(result, move);
    mark_check(state, result, move);
    return result;
  }
}


class fen_parser
{

private:

  static string get_string(istream& stream)
  {
    string result;

    if (!(stream >> result)) {
      throw invalid_fen();
    }
    return result;
  }

  static real_player parse_real_player(istream& stream)
  {
    string token = get_string(stream);

    if (token == "w" || token == "W") {
      return real_player::white;
    }
    else if (token == "b" || token == "B") {
      return real_player::black;
    }
    else {
      throw invalid_fen();
    }
  }

  void parse_ep_index(std::istream& stream, real_player point_of_view)
  {
    string token = get_string(stream);

    if (token == "-") {
      ep_index = sq_index::none();
      return;
    }
    else if (token.size() != 2) {
      throw invalid_fen();
    }
    try {
      ep_index = sq_index(token);

      if (point_of_view == white and ep_index.rank() != rank_6) {
        throw invalid_fen();
      }
      if (point_of_view == black and ep_index.rank() != rank_3) {
        throw invalid_fen();
      }
    }
    catch (...) {
      throw invalid_fen();
    }
  }

  static unsigned parse_uint(const string& str, unsigned min, unsigned max)
  {
    unsigned long value;
    char* endp;
    value = strtoul(str.data(), &endp, 10);
    if (endp != str.data() + str.length()) {
      throw invalid_fen();
    }
    if (value < min or value > max) {
      throw invalid_fen();
    }
    return static_cast<unsigned>(value);
  }

  static unsigned parse_half_moves(string arg)
  {
    return parse_uint(arg, 0, 256);
  }

  static unsigned parse_full_moves(string arg, unsigned half_moves)
  {
    unsigned min;

    if (half_moves > 1) {
      min = (half_moves-1) / 2;
    }
    else {
      min = 1;
    }
    return parse_uint(arg, min, 256);
  }

  static void verify_spaced_board_layout(const string& spaced_board)
  {
    if (spaced_board.size() != 64 + 7) {
      throw invalid_fen();
    }

    int piece_counter = 0;

    for (auto c : spaced_board) {
      if (c == '/') {
        if (piece_counter != 8) {
          throw invalid_fen();
        }
        piece_counter = 0;
      }
      else {
        ++piece_counter;
      }
    }
  }

  string generate_spaced_board(const string& FEN_board)
  {
    string result;

    for (auto c : FEN_board) {
      if (c >= '1' && c <= '8') {
        while (c --> '0') {
          result.push_back(' ');
        }
      }
      else if (c == '/' || real_piece::is_valid_char(c)) {
        result.push_back(c);
      }
      else {
        throw invalid_fen();
      }
    }
    verify_spaced_board_layout(result);
    return result;
  }

  static void verify_counts(const unsigned* counts)
  {
    unsigned sum = 0, opponent_sum = 0;

    if (counts[white_pawn.offset()] > 8 or
        counts[black_pawn.offset()] > 8 or
        counts[white_king.offset()] != 1 or
        counts[black_king.offset()] != 1)
    {
      throw invalid_fen();
    }
    for (auto type : all_piece_types()) {
      sum += counts[real_piece(type, white).offset()];
      opponent_sum += counts[real_piece(type, black).offset()];
    }
    if (sum > 16 || opponent_sum > 16) {
      throw invalid_fen();
    }
  }

  void parse_board(string fen)
  {
    string spaced_board = generate_spaced_board(fen);
    auto c = spaced_board.cbegin();
    unsigned counts[piece_array_size] = {0};

    for (auto rank : ranks_8_to_1) {
      for (auto file : files_a_to_h) {
        if (c == spaced_board.cend()) {
          throw invalid_fen();
        }
        if (*c == ' ') {
          board[sq_index(rank, file).offset()].is_empty = true;
        }
        else {
          real_piece piece(*c);

          board[sq_index(rank, file).offset()].is_empty = false;
          board[sq_index(rank, file).offset()].piece = piece;

          if (piece.type == ::kator::piece::pawn && rank.is_any_back_rank()) {
            throw invalid_fen();
          }
          counts[piece.offset()]++;
        }
        ++c;
      }
      if (c != spaced_board.cend()) {
        ++c;
      }
    }
    verify_counts(counts);
  }

public:

  std::array<real_square, 64> board;
  unsigned half_moves;
  unsigned full_moves;
  real_player turn;
  sq_index ep_index = sq_index::none();
  sq_index pos_ep_index = sq_index::none();
  unique_ptr<kator::castle_rights> castle;

  fen_parser(istream& fen)
  {
    string str_num;
    parse_board(get_string(fen));
    turn = parse_real_player(fen);
    castle = make_unique<castle_rights>(get_string(fen));
    parse_ep_index(fen, turn);
    pos_ep_index = ep_index;

    if (ep_index.is_set()) {
      pos_ep_index = sq_index(rank_5, ep_index.file());
    }
    if (fen >> str_num) {
      half_moves = parse_half_moves(str_num);
      fen >> str_num;
      full_moves = parse_full_moves(str_num, half_moves);
    }
    else {
      half_moves = 0;
      full_moves = 1;
    }
  }

}; /* class fen_parser */

string
generate_board_FEN(position_player whites_view,
                   std::function<square(rank, file)> square_at)
{
  std::stringstream result;

  for (auto rank : ranks_8_to_1) {
    int empty_count = 0;

    for (auto file : files_a_to_h) {
      kator::rank effective_rank = rank;

      if (whites_view == player_opponent) {
        effective_rank.flip();
      }
      square square = square_at(effective_rank, file);
      if (square != nonpiece) {
        if (empty_count > 0) {
          result << empty_count;
          empty_count = 0;
        }
        result << real_piece(square, whites_view).to_str();
      }
      else {
        ++empty_count;
      }
    }
    if (empty_count > 0) {
      result << empty_count;
    }
    if (rank != rank_1) {
      result << "/";
    }
  }
  
  return result.str();
}

} /* anonymous namespace */

string game_state::to_FEN_no_move_counts() const noexcept
{
  string result = generate_board_FEN(whites_view,
                  [&](rank rank, file file)
                  {
                    return position->square_at(rank, file);
                  });

  result += (turn == white) ? " w " : " b ";
  result += position->generate_castle_FEN(turn);
  result += " ";
  if (en_passant_target_square.is_set()) {
    result += en_passant_target_square.to_str();
  }
  else {
    result += "-";
  }
  return result;
}

static void add_move_counts(const game_state* state, string& result)
{
  std::stringstream buffer;

  buffer << " ";
  buffer << state->half_moves;
  buffer << " ";
  buffer << state->full_moves;
  result += buffer.str();
}

string game_state::to_FEN() const noexcept
{
  string result = to_FEN_no_move_counts();

  add_move_counts(this, result);
  return result;
}

static string print_coor_move(move move)
{
  string result;

  result += move.from.to_str();
  result += move.to.to_str();
  return result;
}

move game_state::parse_move(const string& original_move) const
{
  initialize_permanent_vectors();
  string subject(original_move);

  cleanup_move_string(subject);
  for (auto move : moves) {
    string mstr;

    mstr = print_coor_move(move);
    cleanup_move_string(mstr);
    if (mstr == subject) {
      return move;
    }
    mstr = print_algebraic_move(this, move, false);
    cleanup_move_string(mstr);
    if (mstr == subject) {
      return move;
    }
  }
  throw invalid_move_string(original_move);
}

real_piece game_state::piece_at(const sq_index& index) const noexcept
{
  if (turn == white) {
    return real_piece(position->piece_at(index), white);
  }
  else {
    return real_piece(position->piece_at(flip(index)), black);
  }
}

real_piece game_state::piece_at(const rank& rank, const file& file) const noexcept
{
  return piece_at(sq_index(rank, file));
}

string game_state::flipped_to_FEN() const
{
  if (not can_flip) {
    throw std::exception();
  }
  string result = flipped_to_FEN_no_move_counts();

  add_move_counts(this, result);
  return result;
}

string game_state::flipped_to_FEN_no_move_counts() const
{
  if (not can_flip) {
    throw std::exception();
  }
  string result = generate_board_FEN(opponent_of(whites_view),
                  [&](rank rank, file file)
                  {
                    return position->square_at(rank, file);
                  });

  result += (turn == white) ? " b " : " w ";
  result += position->generate_castle_FEN(turn);
  result += " -";
  return result;
}

string game_state::print_move(move move, move_notation notation) const
{
  switch (notation) {
    case move_notation::coordinate:
      return print_coor_move(move);
    case move_notation::SAN:
      return print_algebraic_move(this, move, false);
    case move_notation::FAN:
      return print_algebraic_move(this, move, true);
  }
  return "[invalid move notation]";
}

static move normalize_move(const move_list& moves, move move)
{
  for (auto legal_move : moves) {
    if (move == legal_move) {
      return legal_move;
    }
  }
  throw std::exception();
}

unique_ptr<game_state> game_state::make_move(move move) const
{
  initialize_permanent_vectors();
  move = normalize_move(moves, move);
  if (turn == black) {
    move.flip();
  }

  kator::position child(*position, move);
  unsigned new_full_moves;
  unsigned new_half_moves;
  sq_index ep_target = sq_index::none();

  if (turn == white) {
    new_full_moves = full_moves;
  }
  else {
    new_full_moves = full_moves + 1;
  }
  if (move.is_reversible()) {
    new_half_moves = half_moves + 1;
  }
  else {
    new_half_moves = 0;
  }

  if (move.is_double_pawn_push()) {
    if (turn == black) {
      ep_target = sq_index(rank_6, move.to.file());
    }
    else {
      ep_target = sq_index(rank_3, move.to.file());
    }
  }

  return unique_ptr<game_state>(new game_state(child,
                                               new_half_moves,
                                               new_full_moves,
                                               opponent_of(turn),
                                               ep_target));
}


unique_ptr<game_state> game_state::parse_fen(string fen_string)
{
  std::istringstream iss(fen_string);
  return parse_fen(iss);
}

unique_ptr<game_state> game_state::parse_fen(std::istream& fen_stream)
{
  try{
    unique_ptr<kator::position> position;
    fen_parser parsed(fen_stream);

    initialize_permanent_vectors();
    position.reset(new ::kator::position(parsed.board, *parsed.castle,
                                         parsed.pos_ep_index, parsed.turn));

    return unique_ptr<game_state>(new game_state(std::move(position),
                                                 parsed.half_moves,
                                                 parsed.full_moves,
                                                 parsed.turn,
                                                 parsed.ep_index));
  }
  catch (const invalid_en_passant_index&) {
    throw invalid_fen();
  }
  catch (const invalid_castle_rights&) {
    throw invalid_fen();
  }
  catch (const invalid_king_positions&) {
    throw invalid_fen();
  }
}

unique_ptr<game_state> parse_fen(string FEN)
{
  return game_state::parse_fen(FEN);
}

unique_ptr<game_state> parse_fen(std::istream& FEN)
{
  return game_state::parse_fen(FEN);
}

} /* namespace kator */

