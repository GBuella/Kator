
#include "game.h"
#include "position.h"

#include <memory>
#include <sstream>

using ::std::unique_ptr;
using ::std::string;
using ::std::istream;


namespace kator
{
namespace chess
{

const char starting_fen[] =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

game_state::game_state(const chess::position& position_arg,
                       unsigned half_moves_arg,
                       unsigned full_moves_arg,
                       real_player real_player_arg):
  position(position_arg),
  turn(real_player_arg),
  whites_view((turn == white) ? player::to_move : player::opponent),
  moves(position, whites_view),
  has_any_legal_moves(moves.count() != 0),
  half_moves(half_moves_arg),
  full_moves(full_moves_arg),
  in_check(position.in_check()),
  is_check_mate(in_check and not has_any_legal_moves),
  is_stale_mate((not has_any_legal_moves) and (not in_check))
  {
  }



namespace
{

struct fen_parser_implementation
{

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

  static sq_index parse_ep_index(std::istream& stream, player point_of_view)
  {
    string token = get_string(stream);

    if (token == "-") {
      return sq_index::none();
    }
    if (token.size() != 2) {
      throw invalid_fen();
    }
    try {
      sq_index ep(token);

      if (point_of_view == player::to_move and ep.rank() != rank_6) {
        throw invalid_fen();
      }
      if (point_of_view == player::opponent and ep.rank() != rank_3) {
        throw invalid_fen();
      }
      return sq_index(rank_5, ep.file());
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


  unique_ptr<chess::position> position;
  unsigned half_moves;
  unsigned full_moves;
  real_player turn;

  fen_parser_implementation(istream& fen)
  {
    initialize_permanent_vectors();
    position::piece_board board(get_string(fen));
    turn = parse_real_player(fen);
    castle_rights castle(get_string(fen));
    player whites_view = (turn == white) ? player::to_move : player::opponent;
    sq_index ep_index = parse_ep_index(fen, whites_view);
    try {
      position.reset(new chess::position(board, castle, ep_index, whites_view));
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
    string str_board, str_num;
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

};

class game_state_implementation : public game_state
{

  unique_ptr<chess::position> position_storage;

public:

  game_state_implementation(fen_parser_implementation& parsed):
    game_state(*parsed.position,
               parsed.half_moves,
               parsed.full_moves,
               parsed.turn),
    position_storage(std::move(parsed.position))
  {
  }

  game_state_implementation(unique_ptr<chess::position>& new_position,
                            unsigned new_half_moves,
                            unsigned new_full_moves,
                            real_player new_turn):
    game_state(*new_position,
               new_half_moves,
               new_full_moves,
               new_turn),
    position_storage(std::move(new_position))
  {
  }

  string to_FEN_no_move_counts() const noexcept
  {
    string result = position.generate_board_FEN(whites_view);

    result += (turn == white) ? " w " : " b ";
    result += position.generate_castle_FEN(turn);
    result += " ";
    result += position.generate_ep_FEN(turn);
    return result;
  }

  void add_move_counts(std::string& result) const
  {
    std::stringstream buffer;

    buffer << " ";
    buffer << half_moves;
    buffer << " ";
    buffer << full_moves;
    result += buffer.str();
  }

  string to_FEN() const noexcept
  {
    string result = to_FEN_no_move_counts();

    add_move_counts(result);
    return result;
  }

  move parse_move(const string& original_move) const
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
      mstr = print_algebraic_move(move, false);
      cleanup_move_string(mstr);
      if (mstr == subject) {
        return move;
      }
    }
    throw invalid_move_string(original_move);
  }

  piece piece_at(const sq_index& index) const noexcept
  {
    if (turn == white) {
      return position.piece_at(index);
    }
    else {
      return position.piece_at(flip(index));
    }
  }

  piece piece_at(const rank& rank, const file& file) const noexcept
  {
    return piece_at(sq_index(rank, file));
  }

  bool can_flip() const noexcept
  {
    return not position.has_en_passant_square();
  }

  string flipped_to_FEN() const
  {
    if (not can_flip()) {
      throw std::exception();
    }
    string result = flipped_to_FEN_no_move_counts();

    add_move_counts(result);
    return result;
  }

  string flipped_to_FEN_no_move_counts() const
  {
    if (not can_flip()) {
      throw std::exception();
    }
    string result = position.generate_board_FEN(opponent_of(whites_view));

    result += (turn == white) ? " b " : " w ";
    result += position.generate_castle_FEN(turn);
    result += " -";
    return result;
  }

private:

  std::string print_coor_move(move move) const
  {
    string result;

    result += move.from.to_str();
    result += move.to.to_str();
    return result;
  }

  bitboard gen_ambiguous_map(sq_index from, sq_index to) const
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

  string SAN_move_from(sq_index from, sq_index to) const
  {
    bitboard ambiguous_pieces = gen_ambiguous_map(from, to);
    string result;

    if (!ambiguous_pieces.is_empty()) {
      if (!(ambiguous_pieces & bitboard(from.file())).is_empty()) {
        if (!(ambiguous_pieces & bitboard(to.rank())).is_empty()) {
          result += from.to_str(whites_view);
        }
        else {
          result += from.rank().to_str(whites_view);
        }
      }
      else {
        result += from.file().to_str();
      }
    }
    return result;
  }

  void mark_check(string& result, move move) const
  {
    chess::position child(position, move);

    if (child.in_check()) {
      result += "+";
    }
  }

  void mark_promotion(string& result, move move, bool use_figurines) const
  {
    if (move.is_promotion()) {
      result += "=";
      result += move.result().to_str(player::to_move, use_figurines);
    }
  }

  void mark_en_passant(string& result, move move) const
  {
    if (move.is_en_passant()) {
      result += "e.p.";
    }
  }

  void mark_capture(string& result, move move) const
  {
    if (move.is_capture()) {
      result += "x";
    }
  }

  std::string
  print_algebraic_move(move move, bool use_figurines) const
  {
    if (turn == black) {
      move.flip();
    }
    if (move.is_castle_queenside()) {
      return "O-O-O";
    }
    else if (move.is_castle_kingside()) {
      return "O-O";
    }
    else {
      string result;
      piece actor = piece_at(move.from.effective_of(whites_view));

      if (actor == pawn) {
        if (move.is_capture()) {
          result = move.from.file().to_str();
        }
      }
      else {
        result += actor.to_str(player::to_move, use_figurines);
        result += SAN_move_from(move.from, move.to);
      }
      mark_capture(result, move);
      result += move.to.to_str(whites_view);
      mark_promotion(result, move, use_figurines);
      mark_en_passant(result, move);
      mark_check(result, move);
      return result;
    }
  }

public:

  std::string print_move(move move, move_notation notation) const
  {
    switch (notation) {
      case move_notation::coordinate:
        return print_coor_move(move);
      case move_notation::SAN:
        return print_algebraic_move(move, false);
      case move_notation::FAN:
        return print_algebraic_move(move, true);
    }
    return "[invalid move notation]";
  }


}; /* class game_state_implementation */

}

unique_ptr<game_state> game_state::make_move(move move) const
{
  initialize_permanent_vectors();
  unique_ptr<chess::position> child(new chess::position(position, move));
  unsigned new_full_moves;
  unsigned new_half_moves;

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

  return unique_ptr<game_state>(
           new game_state_implementation(child,
                                         new_half_moves,
                                         new_full_moves,
                                         opponent_of(turn)));
}

unique_ptr<game_state> game_state::parse_fen(string fen_string)
{
  std::istringstream iss(fen_string);
  fen_parser_implementation parsed(iss);

  return unique_ptr<game_state>(new game_state_implementation(parsed));
}

unique_ptr<game_state> game_state::parse_fen(std::istream& fen_stream)
{
  fen_parser_implementation parsed(fen_stream);

  return unique_ptr<game_state>(new game_state_implementation(parsed));
}


game_state::~game_state() noexcept
{
}

std::unique_ptr<game_state> parse_fen(std::string FEN)
{
  return game_state::parse_fen(FEN);
}

std::unique_ptr<game_state> parse_fen(std::istream& FEN)
{
  return game_state::parse_fen(FEN);
}

game::game()
{
  reset(starting_fen);
}

void game::reset(unique_ptr<game_state> state)
{
  states.clear();
  states.push_back(std::move(state));
  current_index = 0;
}

void game::reset(string fen)
{
  unique_ptr<game_state> new_state = game_state::parse_fen(fen);

  reset(std::move(new_state));
}

void game::reset(istream& fen)
{
  unique_ptr<game_state> new_state = game_state::parse_fen(fen);

  reset(std::move(new_state));
}

const game_state& game::current_state() const noexcept
{
  return *(states[current_index]);
}

void game::advance(move move)
{
  unique_ptr<game_state> new_state = current_state().make_move(move);
  auto begin = states.begin() + static_cast<ptrdiff_t>(current_index) + 1;

  states.erase(begin, states.end());
  states.push_back(std::move(new_state));
  ++current_index;
}

void game::advance()
{
  if (not is_at_last_state()) {
    ++current_index;
  }
}

bool game::is_at_last_state() const noexcept
{
  return current_index + 1 == states.size();
}

bool game::is_at_first_state() const noexcept
{
  return current_index == 0;
}

void game::revert()
{
  if (not is_at_first_state()) {
    --current_index;
  }
}

size_t game::length() const noexcept
{
  return states.size();
}

real_player game::turn() const noexcept
{
  return current_state().turn;
}

const move_list& game::legal_moves() const noexcept
{
  return current_state().moves;
}

game::~game()
{
}

} /* namespace kator::chess */
} /* namespace kator */
