
#include <memory>
#include <sstream>

#include "position.h"
#include "move.h"
#include "castle_rights.h"

using ::std::string;

namespace kator
{
namespace chess
{

#ifdef USE_ALIGNAS_64

void* position::operator new(std::size_t requested_size)
{
  return position_allocator::allocate(requested_size / sizeof(position) + 1);
}

void position::operator delete(void* pointer)
{
  if (pointer != nullptr) {
    position_allocator::deallocate(static_cast<position*>(pointer));
  }
}

#endif

namespace
{

void verify_spaced_board_layout(const string& spaced_board)
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
    else if (c == '/' || piece::is_valid_char(c)) {
      result.push_back(c);
    }
    else {
      throw invalid_fen();
    }
  }
  verify_spaced_board_layout(result);
  return result;
}

void verify_counts(const int* counts)
{
  int sum = 0, opponent_sum = 0;

  if (counts[pawn.index()] > 8 || counts[opponent_pawn.index()] > 8 ||
      counts[king.index()] != 1 || counts[opponent_king.index()] != 1)
  {
    throw invalid_fen();
  }
  for (auto p : piece::all_pieces()) {
    sum += counts[p.index()];
    opponent_sum += counts[opponent_of(p).index()];
  }
  if (sum > 16 || opponent_sum > 16) {
    throw invalid_fen();
  }
}

} /* anonym namespace */

position::piece_board::piece_board(string FEN_board)
{
  for (int i = 0; i < 64; ++i) {
    board.squares[i] = 0;
  }
  string spaced_board = generate_spaced_board(FEN_board);
  auto c = spaced_board.cbegin();
  int counts[piece::array_size] = {0};

  for (auto rank : ranks_8_to_1) {
    for (auto file : files_a_to_h) {
      if (c == spaced_board.cend()) {
        throw invalid_fen();
      }
      piece piece(*c);

      if (piece.is_pawn() && rank.is_any_back_rank()) {
        throw invalid_fen();
      }
      counts[piece.index()]++;
      board.squares[sq_index(rank, file).offset()] = piece.index();
      ++c;
    }
    if (c != spaced_board.cend()) {
      ++c;
    }
  }
  verify_counts(counts);
}

inline void position::add_piece(sq_index index, piece piece)
{
  board.squares[index.offset()] = piece.index();
}

inline void position::add_piece(rank rank, file file, piece piece)
{
  add_piece(sq_index(rank, file), piece);
}

inline void position::clear_square(sq_index index)
{
  board.squares[index.offset()] = 0;
}

inline void position::clear_square(rank rank, file file)
{
  clear_square(sq_index(rank, file));
}

inline void position::piece_map_remove(sq_index index)
{
  bitboard::reset_bit_mem(&map_of(piece_at(index)), index);
}

inline void position::piece_map_remove(sq_index index, piece piece)
{
  bitboard::reset_bit_mem(&map_of(piece), index);
}

inline void position::piece_map_add(piece piece, sq_index index)
{
  bitboard::set_bit_mem(&map_of(piece), index);
}

namespace
{

bool is_en_passant_index_valid(const position& position)
{
  if (not position.has_en_passant_square()) {
    return true;
  }

  // en passant index must be a result of a pawn double push
  sq_index index = position.ep_index();
  return position.piece_at(index) == opponent_pawn
        and not position.has_piece_at(north_of(index))
        and not position.has_piece_at(north_of(north_of(index)));
}

bool are_castling_rights_valid(const position& position)
{
  if (position.can_castle_any() and position.piece_at(e1) != king) {
    return false;
  }
  if (position.can_castle_queenside() and position.piece_at(a1) != rook) {
    return false;
  }
  if (position.can_castle_kingside() and position.piece_at(h1) != rook) {
    return false;
  }
  if (position.opponent_can_castle_any()
      and position.piece_at(e8) != opponent_king)
  {
    return false;
  }
  if (position.opponent_can_castle_queenside()
      and position.piece_at(a8) != opponent_rook)
  {
    return false;
  }
  if (position.opponent_can_castle_kingside()
      and position.piece_at(h8) != opponent_rook)
  {
    return false;
  }
  return true;
}

bool are_king_maps_sane(const position& position)
{
  return is_singular(position.map_of(king))
     and is_singular(position.map_of(opponent_king))
     and are_disjoint(
           bitboard::king_attacks(position.king_index()),
           position.map_of(opponent_king));
}

void setup_zhash(const position& position, zobrist_hash_pair& zhash)
{
  for (auto index : sq_index::range()) {
    zhash.xor_piece(position.piece_at(index), index);
  }
  if (position.can_castle_queenside()) {
    zhash.xor_castle_right(castle_rights::side::queenside);
  }
  if (position.can_castle_kingside()) {
    zhash.xor_castle_right(castle_rights::side::kingside);
  }
  if (position.opponent_can_castle_queenside()) {
    zhash.xor_castle_right(castle_rights::side::opponent_queenside);
  }
  if (position.opponent_can_castle_kingside()) {
    zhash.xor_castle_right(castle_rights::side::opponent_kingside);
  }
}

void update_player_maps(bitboard* maps)
{
  maps[0] = union_of(maps[2], maps[4], maps[6], maps[8], maps[10], maps[12]);
  maps[1] = union_of(maps[3], maps[5], maps[7], maps[9], maps[11], maps[13]);
}

bitboard all_knight_attacks(bitboard knights)
{
  bitboard result = bitboard::empty();

  for (auto ki : knights) {
    result.merge(bitboard::knight_attacks(ki));
  }
  return result;
}

bitboard all_bishop_attacks(bitboard piece_map, bitboard occupied)
{
  bitboard result = bitboard::empty();

  for (auto index : piece_map) {
    result.merge(bitboard::bishop_attacks(occupied, index));
  }
  return result;
}

bitboard all_rook_attacks(bitboard piece_map, bitboard occupied)
{
  bitboard result = bitboard::empty();

  for (auto index : piece_map) {
    result.merge(bitboard::rook_attacks(occupied, index));
  }
  return result;
}

bitboard ray_checkers(const position& position, bitboard bandits)
{
  bitboard rays = bandits;

  for (auto bandit_index : bandits) {
    rays.merge(bitboard::ray_between(position.king_index(), bandit_index));
  }
  return rays;
}

bitboard bishop_bandits(const position& position)
{
  return intersection_of(
      bitboard::bishop_attacks(position.occupied_map(), position.king_index()),
      position.opponent_bishop_queen_map());
}

bitboard rook_bandits(const position& position)
{
  return intersection_of(
      bitboard::rook_attacks(position.occupied_map(), position.king_index()),
      position.opponent_rook_queen_map());
}

bitboard knight_checkers(const position& position)
{
  return intersection_of(
            bitboard::knight_attacks(position.king_index()),
            position.map_of(opponent_knight));
}

bitboard pawn_checker(const position& position)
{
  return intersection_of(
            bitboard::pawn_attacks(position.map_of(king)),
            position.map_of(opponent_pawn));
}

} /* anonym namespace */

inline void position::generate_piece_maps()
{
  piece_map_raw.fill(bitboard(bitboard::empty()));
  for (auto index : sq_index::range()) {
    if (has_piece_at(index)) {
      piece_map_add(piece_at(index), index);
    }
  }
  update_player_maps(piece_map_raw.data() + piece_map_padding);
  occupied = union_of(map_of(player::to_move), map_of(player::opponent));
}

inline void position::generate_attacks_by_piece()
{
  bitboard temp_occupied;

  attacks_of(pawn) =
    bitboard::pawn_attacks(map_of(pawn));
  attacks_of(king) =
    bitboard::king_attacks(king_index());
  attacks_of(knight) =
    all_knight_attacks(map_of(knight));

  temp_occupied = intersection_of(occupied, compl map_of(opponent_king));

  attacks_of(bishop) =
    all_bishop_attacks(map_of(bishop), temp_occupied);
  attacks_of(rook) =
    all_rook_attacks(map_of(rook), temp_occupied);
  attacks_of(queen) = union_of(
      all_bishop_attacks(map_of(queen), temp_occupied),
      all_rook_attacks(map_of(queen), temp_occupied));
}

inline void position::generate_opponent_attacks_by_piece()
{
  bitboard temp_occupied;

  attacks_of(opponent_pawn) =
    bitboard::opponent_pawn_attacks(map_of(opponent_pawn));
  attacks_of(opponent_king) =
    bitboard::king_attacks(opponent_king_index());
  attacks_of(opponent_knight) =
    all_knight_attacks(map_of(opponent_knight));

  temp_occupied = intersection_of(occupied, compl map_of(king));

  attacks_of(opponent_bishop) =
    all_bishop_attacks(map_of(opponent_bishop), temp_occupied);
  attacks_of(opponent_rook) =
    all_rook_attacks(map_of(opponent_rook), temp_occupied);
  attacks_of(opponent_queen) = union_of(
      all_bishop_attacks(map_of(opponent_queen), temp_occupied),
      all_rook_attacks(map_of(opponent_queen), temp_occupied));
}

inline void position::generate_attack_maps()
{
  generate_attacks_by_piece();
  generate_opponent_attacks_by_piece();
  update_player_maps(attack_map.data());
  if (are_disjoint(attacks_of(player::opponent), map_of(king))) {
    king_attack_map = bitboard::empty();
  }
  else {
    king_attack_map = union_of(knight_checkers(*this),
                               pawn_checker(*this),
                               ray_checkers(*this, bishop_bandits(*this)),
                               ray_checkers(*this, rook_bandits(*this)));
  }
}

#ifdef USE_BOARD_VECTOR_64
#ifdef HAS_INTEL_512BIT_BUILTINS

#error not yet implemented

static inline void vector_board_copy_and_flip(vector_8_64* dst, const vector_8_64* src)
{
  vector_8_64* vdst = (vector_8_64*)dst;
  const vector_8_64* vsrc = (vector_8_64*)src;
  vector_8_64 temp0, temp1;

  temp0 = vsrc[0] ^ vector_64_one;
  temp1 = _mm512_cmpeq_epi8(vsrc[0], vector_64_zero);
  temp1 = _mm512_andnot_si512(temp1, temp0);
  vdst[0] = swap_64_bit_members_512(temp1);
}

#elif defined(HAS_INTEL_256BIT_BUILTINS)

static inline void vector_board_copy_and_flip(vector_8_64* dst, const vector_8_64* src)
{
  vector_8_32* vdst = (vector_8_32*)dst;
  const vector_8_32* vsrc = (vector_8_32*)src;
  vector_8_32 temp0, temp1;

  temp0 = vsrc[0] ^ vector_32_one;
  temp1 = _mm256_cmpeq_epi8(vsrc[0], vector_32_zero);
  temp1 = _mm256_andnot_si256(temp1, temp0);
  vdst[1] = swap_64_bit_members_256(temp1);
  temp0 = vsrc[1] ^ vector_32_one;
  temp1 = _mm256_cmpeq_epi8(vsrc[1], vector_32_zero);
  temp1 = _mm256_andnot_si256(temp1, temp0);
  vdst[0] = swap_64_bit_members_256(temp1);
}

#elif defined(HAS_INTEL_128BIT_BUILTINS)

static inline void vector_board_copy_and_flip(vector_8_64* dst, const vector_8_64* src)
{
  vector_8_16* vdst = (vector_8_16*)dst;
  const vector_8_16* vsrc = (vector_8_16*)src;
  vector_8_16 temp0, temp1;

  temp0 = vsrc[0] ^ vector_16_one;
  temp1 = _mm_cmpeq_epi8(vsrc[0], vector_16_zero);
  temp1 = _mm_andnot_si128(temp1, temp0);
  vdst[3] = swap_64_bit_members_128(temp1);
  temp0 = vsrc[1] ^ vector_16_one;
  temp1 = _mm_cmpeq_epi8(vsrc[1], vector_16_zero);
  temp1 = _mm_andnot_si128(temp1, temp0);
  vdst[2] = swap_64_bit_members_128(temp1);
  temp0 = vsrc[2] ^ vector_16_one;
  temp1 = _mm_cmpeq_epi8(vsrc[2], vector_16_zero);
  temp1 = _mm_andnot_si128(temp1, temp0);
  vdst[1] = swap_64_bit_members_128(temp1);
  temp0 = vsrc[3] ^ vector_16_one;
  temp1 = _mm_cmpeq_epi8(vsrc[3], vector_16_zero);
  temp1 = _mm_andnot_si128(temp1, temp0);
  vdst[0] = swap_64_bit_members_128(temp1);
}

#else
#  error No compiler vector support found for use with piece board
#endif

#endif // USE_BOARD_VECTOR_64

inline void
position::board_copy_and_flip(position::board_internal& RESTRICT dst,
                              const position::board_internal& RESTRICT src)
{
#ifdef USE_BOARD_VECTOR_64
  vector_board_copy_and_flip(&dst.squares, &src.squares);
#else

  unsigned char* RESTRICT ldst = dst.squares.data();
  const unsigned char* RESTRICT lsrc = src.squares.data();

  for (int i = 0; i < 64; ++i) {
    unsigned char temp = lsrc[i];

    if (temp != 0) {
      temp ^= 1;
    }
    ldst[i] = temp;
  }

  for (int i = 0; i < 32; i += 8) {
    unsigned char temp[8];

    std::memcpy(temp, ldst + (56 - i), 8);
    std::memcpy(ldst + (56 - i), ldst + i, 8);
    std::memcpy(ldst + i, temp, 8);
  }

#endif // USE_BOARD_VECTOR_64
}

/* construct from board and auxiliary information,
   intended to be used only during construction from a
   FEN string
*/
position::position(const position::piece_board& board_init,
                   chess::castle_rights castle,
                   const sq_index en_passant_square,
                   const player point_of_view):
  castle_rights(castle, point_of_view),
  en_passant_index(en_passant_square),
  zhash(zobrist_hash_pair::initial())
{
  if (point_of_view == player::to_move) {
    board.squares = board_init.board.squares;
  }
  else {
    board_copy_and_flip(board, board_init.board);
  }
  generate_piece_maps();
  generate_attack_maps();
  if (not are_king_maps_sane(*this)) {
    throw invalid_king_positions();
  }
  if (not is_en_passant_index_valid(*this)) {
    throw invalid_en_passant_index();
  }
  if (not are_castling_rights_valid(*this)) {
    throw invalid_castle_rights();
  }
  setup_zhash(*this, zhash);
}

string position::generate_board_FEN(player point_of_view) const noexcept
{
  std::stringstream result;

  for (auto rank : ranks_8_to_1) {
    int empty_count = 0;

    for (auto file : files_a_to_h) {
      chess::rank effective_rank = rank.effective_of(point_of_view);

      if (piece_at(effective_rank, file).is_set()) {
        if (empty_count > 0) {
          result << empty_count;
          empty_count = 0;
        }
        result << piece_at(effective_rank, file).to_str(point_of_view);
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

string position::generate_ep_FEN(real_player point_of_view) const noexcept
{
  string result;

  if (!en_passant_index.is_set()) {
    result = "-";
  }
  else {
    result += en_passant_index.file().to_str();
    if (point_of_view == white) {
      result.push_back('6');
    }
    else {
      result.push_back('3');
    }
  }
  return result;
}

namespace
{

static inline bool
has_potential_ep_captor(const position& position, sq_index victim) noexcept
{
  if (victim.file() != file_a and position.piece_at(left_of(victim)) == pawn) {
    return true;
  }
  if (victim.file() != file_h and position.piece_at(right_of(victim)) == pawn) {
    return true;
  }
  return false;
}

#ifdef USE_PIECE_MAP_VECTOR
#  ifdef HAS_INTEL_512BIT_BUILTINS

     void vector_piece_map_copy_and_flip(bitboard* RESTRICT dst,
                                         const bitboard* RESTRICT src)
     {
       vector64* src64 = (vector64*)src;
       vector64* dst64 = (vector64*)dst;

       dst64[0] = _mm512_shuffle_epi8(src64[0], bswap_shuffle_key_64);
       dst64[1] = _mm512_shuffle_epi8(src64[1], bswap_shuffle_key_64);
     }


#  elif defined(HAS_INTEL_256BIT_BUILTINS)

     void vector_piece_map_copy_and_flip(bitboard* RESTRICT dst,
                                         const bitboard* RESTRICT src)
     {
       vector32* src32 = (vector32*)src;
       vector32* dst32 = (vector32*)dst;

       dst32[1] = _mm256_shuffle_epi8(src32[1], bswap_shuffle_key_32);
       dst32[2] = _mm256_shuffle_epi8(src32[2], bswap_shuffle_key_32);
       dst32[3] = _mm256_shuffle_epi8(src32[3], bswap_shuffle_key_32);
     }

#  elif defined(HAS_INTEL_128BIT_BUILTINS)

     void vector_piece_map_copy_and_flip(bitboard* RESTRICT dst,
                                         const bitboard* RESTRICT src)
     {
       vector16* src16 = (vector16*)src;
       vector16* dst16 = (vector16*)dst;

       for (size_t i = 2; i < 8; ++i) {
         dst16[i] = _mm_shuffle_epi8(src16[i], bswap_shuffle_key_16);
       }
     }


#  else
#    error No compiler vector support found for use with piece_maps
#  endif
#endif // USE_PIECE_MAP_VECTOR

} /* anonym namespace */

inline void position::piece_map_copy_and_flip(const position& parent)
{
  /* This loop copies the piece map from the parent position, as {{{
     regenerating it from the piece board appeared to be significantly slower.
     On x86_64 the posrtable code is meant to compile
     as triples of movq - bswapq - movq instructions, or pairs of
     movbeq - movq. No advantage has been observed using 128 bit vectors,
     so there is no code for that case. The AVX2 256 bit vectorization
     might to speed things up a bit.
     Note: only the bitboards of specific piece types are needed here,
     so the first 2 bitboards containing the maps of all player::to_move pieces,
     and all player::opponent pieces are skipped. With the padding of 2 bitboards
     at the front of the piece_map_raw array, and 32 byte alignment of the array,
     this means the piece maps starting at piece_map_raw[4] are also 32 bytes
     aligned. This helps in the implementation of the 256 bit version of the
     vector_piece_map_copy_and_flip rutine.
      ( 2015 February )
      }}}*/

#ifdef USE_PIECE_MAP_VECTOR
  vector_piece_map_copy_and_flip(piece_map_raw.data(),
                                 parent.piece_map_raw.data());
#else


  for (auto piece : piece::all_pieces()) {
    map_of(piece) = chess::flip(&parent.map_of(opponent_of(piece)));
    map_of(opponent_of(piece)) = chess::flip(&parent.map_of(piece));
  }
#endif
}

inline void position::move_primary_piece(move move)
{
  /* These two lines just place the new piece to the destination square,{{{
     which might be:
       the original moving piece at move.from
       the original moving king or rook without castling right flag
       the piece a pawn is promoted to
     Meanwhile of course a captured piece is removed if there was any,
     and the square at move.from is cleared.
  }}}*/

  piece_map_remove(move.to);
  zhash.xor_piece(piece_at(move.to), move.to);
  piece_map_add(opponent_of(move.result()), move.to);
  zhash.xor_piece(opponent_of(move.result()), move.to);
  piece_map_remove(move.from);
  zhash.xor_piece(piece_at(move.from), move.from);
  board.squares[move.to.offset()] = opponent_of(move.result()).index();
  board.squares[move.from.offset()] = 0;
}

inline void position::handle_castle_rights(move move)
{
  if (clears_right_opponent_queenside(move)) {
    clear_castle_right(castle_rights::side::opponent_queenside);
    zhash.xor_castle_right(castle_rights::side::opponent_queenside);
  }
  if (clears_right_opponent_kingside(move)) {
    clear_castle_right(castle_rights::side::opponent_kingside);
    zhash.xor_castle_right(castle_rights::side::opponent_kingside);
  }
  if (clears_right_queenside(move)) {
    clear_castle_right(castle_rights::side::queenside);
    zhash.xor_castle_right(castle_rights::side::queenside);
  }
  if (clears_right_kingside(move)) {
    clear_castle_right(castle_rights::side::kingside);
    zhash.xor_castle_right(castle_rights::side::kingside);
  }
}

void position::handle_special_move(const position& parent, move move)
{
  switch (move.move_type) {
    case move::en_passant:
      {
        sq_index ep = parent.en_passant_index.flipped();

        board.squares[ep.offset()] = 0;
        piece_map_remove(ep, pawn);
        zhash.xor_piece(pawn, ep);
      }
      break;

    case move::castle_kingside:
      add_piece(f8, opponent_rook);
      clear_square(h8);
      map_of(opponent_rook) ^= bitboard(f8, h8);
      zhash.xor_piece(rook, f8);
      zhash.xor_piece(rook, h8);
      break;

    case move::castle_queenside:
      add_piece(d8, opponent_rook);
      clear_square(a8);
      map_of(opponent_rook) ^= bitboard(d8, a8);
      zhash.xor_piece(rook, d8);
      zhash.xor_piece(rook, a8);
      break;

    case move::pawn_double_push:
    case move::promotion:
    case move::general:
      break;
  }
}

/* This constructor is the actual make_move procedure */
position::position(const position& parent, move move) noexcept:
  castle_rights(parent.castle_flipped()),
  en_passant_index(sq_index::leave::uninitialized),
  zhash(parent.zhash.flipped())
{
  board_copy_and_flip(board, parent.board);
  piece_map_copy_and_flip(parent);


  /* Start with flipping the move, because the coordinates in the move{{{
     refer the squares on the original board. At this point the new board
     already shows the point of view of the next player, thus in essence
     this is the move the opponent has just made. This is also the reason
     why the opponent_of function is needed here.
  }}}*/
  move.flip();

  move_primary_piece(move);
  handle_special_move(parent, move);
  update_player_maps(piece_map_raw.data() + piece_map_padding);
  occupied = union_of(map_of(player::to_move), map_of(player::opponent));
  generate_attack_maps();
  handle_castle_rights(move);

  if (move.is_double_pawn_push() and has_potential_ep_captor(*this, move.to)) {
    en_passant_index = move.to;
  }
  else {
    en_passant_index.unset();
  }

}

} /* namespace kator::chess */
} /* namespace kator */

