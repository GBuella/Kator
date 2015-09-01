
#include <memory>
#include <sstream>
#include <functional>

#include "position.h"
#include "move.h"
#include "castle_rights.h"
#include "platform/x64_vectors.h"

using ::std::string;

namespace kator
{

void* position::operator new(std::size_t requested_size)
{
  return aligned_allocate(requested_size, alignof(position));
}

void position::operator delete(void* pointer)
{
  if (pointer != nullptr) {
    anligned_deallocate(pointer);
  }
}



inline void
position::add_piece(sq_index index, square square)
{
  set_board_at(index, get_piece_type(square));
  piece_map_add(square, index);
}

inline void
position::set_board_at(sq_index index, piece piece)
{
  board[index.offset()] = offset(piece);
}

inline void
position::set_board_at(rank rank, file file, piece piece)
{
  set_board_at(sq_index(rank, file), piece);
}

inline void
position::clear_board_at(sq_index index)
{
  board[index.offset()] = 0;
}

inline void
position::clear_board_at(rank rank, file file)
{
  clear_board_at(sq_index(rank, file));
}

inline void
position::piece_map_remove(sq_index index, square square)
{
  bitboard::reset_bit_mem((piece_map() + square), index);
}

inline void
position::piece_map_add(square square, sq_index index)
{
  bitboard::set_bit_mem((piece_map() + square), index);
}

inline void
position::setup_occupied()
{
  *(address_occupied()) = union_of(map_of(to_move), map_of(opponent));
}




namespace
{

bool is_en_passant_index_valid(const position* position)
{
  if (not position->has_en_passant_square()) {
    return true;
  }

  // en passant index must be a result of a pawn double push
  sq_index index = position->ep_index();

  return position->has_piece_at(index, opponent_pawn)
        and not position->has_piece_at(north_of(index))
        and not position->has_piece_at(north_of(north_of(index)));
}

bool are_castling_rights_valid(const position* position)
{
  if (position->can_castle_any()
      and not position->has_piece_at(e1, king))
  {
    return false;
  }
  if (position->can_castle_queenside()
      and not position->has_piece_at(a1, rook))
  {
    return false;
  }
  if (position->can_castle_kingside()
      and not position->has_piece_at(h1, rook))
  {
    return false;
  }
  if (position->opponent_can_castle_any()
      and not position->has_piece_at(e8, opponent_king))
  {
    return false;
  }
  if (position->opponent_can_castle_queenside()
      and not position->has_piece_at(a8, opponent_rook))
  {
    return false;
  }
  if (position->opponent_can_castle_kingside()
      and not position->has_piece_at(h8, opponent_rook))
  {
    return false;
  }
  return true;
}

bool are_king_maps_sane(const position* position)
{
  return is_singular(position->map_of(king))
     and is_singular(position->map_of(opponent_king))
     and are_disjoint(
           bitboard::king_attacks(position->king_index()),
           position->map_of(opponent_king));
}

void setup_zhash(const position* position, zobrist_hash_pair* zhash)
{
  /*  TODO
  for (auto index : sq_index::range()) {
    zhash->xor_piece(position.square_at(index), index);
  }
  */
  if (position->can_castle_queenside()) {
    zhash->xor_castle_right(castle_rights::side::queenside);
  }
  if (position->can_castle_kingside()) {
    zhash->xor_castle_right(castle_rights::side::kingside);
  }
  if (position->opponent_can_castle_queenside()) {
    zhash->xor_castle_right(castle_rights::side::opponent_queenside);
  }
  if (position->opponent_can_castle_kingside()) {
    zhash->xor_castle_right(castle_rights::side::opponent_kingside);
  }
}

void update_player_maps(bitboard* RESTRICT dst, const bitboard* RESTRICT maps)
{
  dst[0] = union_of(maps[2], maps[4], maps[6], maps[8], maps[10], maps[12]);
  dst[1] = union_of(maps[3], maps[5], maps[7], maps[9], maps[11], maps[13]);
}

bitboard all_knight_attacks(bitboard knights)
{
  bitboard accumulator = bitboard::empty();

  for (auto index : knights) {
    accumulator.merge(bitboard::knight_attacks(index));
  }
  return accumulator;
}

bitboard all_bishop_attacks(bitboard bishops, bitboard occupied)
{
  bitboard accumulator = bitboard::empty();

  for (auto index : bishops) {
    accumulator.merge(bitboard::bishop_attacks(occupied, index));
  }
  return accumulator;
}

bitboard all_rook_attacks(bitboard rooks, bitboard occupied)
{
  bitboard accumulator = bitboard::empty();

  for (auto index : rooks) {
    accumulator.merge(bitboard::rook_attacks(occupied, index));
  }
  return accumulator;
}

bitboard ray_checkers(const position* position, bitboard bandits)
{
  bitboard rays = bandits;

  for (auto bandit_index : bandits) {
    rays.merge(bitboard::ray_between(position->king_index(), bandit_index));
  }
  return rays;
}

bitboard bishop_bandits(const position* position)
{
  return intersection_of(
      bitboard::bishop_attacks(position->occupied(), position->king_index()),
      position->opponent_bishop_queen_map());
}

bitboard rook_bandits(const position* position)
{
  return intersection_of(
      bitboard::rook_attacks(position->occupied(), position->king_index()),
      position->opponent_rook_queen_map());
}

bitboard knight_checkers(const position* position)
{
  return intersection_of(
            bitboard::knight_attacks(position->king_index()),
            position->map_of(opponent_knight));
}

bitboard pawn_checker(const position* position)
{
  return intersection_of(
            bitboard::pawn_attacks(position->map_of(king)),
            position->map_of(opponent_pawn));
}

} /* anonym namespace */

inline void position::generate_attacks_by_piece()
{
  bitboard temp_occupied;

  attack_map()[pawn] = bitboard::pawn_attacks(map_of(pawn));
  attack_map()[king] = bitboard::king_attacks(king_index());
  attack_map()[knight] = all_knight_attacks(map_of(knight));

  temp_occupied = intersection_of(occupied(), compl map_of(opponent_king));

  attack_map()[bishop] = all_bishop_attacks(map_of(bishop), temp_occupied);
  attack_map()[rook] = all_rook_attacks(map_of(rook), temp_occupied);
  attack_map()[queen] = union_of(
      all_bishop_attacks(map_of(queen), temp_occupied),
      all_rook_attacks(map_of(queen), temp_occupied));
}

inline void position::generate_opponent_attacks_by_piece()
{
  bitboard temp_occupied;

  attack_map()[opponent_pawn] =
    bitboard::opponent_pawn_attacks(map_of(opponent_pawn));
  attack_map()[opponent_king] =
    bitboard::king_attacks(opponent_king_index());
  attack_map()[opponent_knight] =
    all_knight_attacks(map_of(opponent_knight));

  temp_occupied = intersection_of(occupied(), compl map_of(king));

  attack_map()[opponent_bishop] =
    all_bishop_attacks(map_of(opponent_bishop), temp_occupied);
  attack_map()[opponent_rook] =
    all_rook_attacks(map_of(opponent_rook), temp_occupied);
  attack_map()[opponent_queen] = union_of(
      all_bishop_attacks(map_of(opponent_queen), temp_occupied),
      all_rook_attacks(map_of(opponent_queen), temp_occupied));
}

inline void position::generate_attack_maps()
{
  generate_attacks_by_piece();
  generate_opponent_attacks_by_piece();
  update_player_maps(player_attack_map(), attack_map());
  if (are_disjoint(attacks_of(opponent), map_of(king))) {
    *address_king_attack_map() = bitboard::empty();
  }
  else {
    *address_king_attack_map() =
      union_of(knight_checkers(this),
               pawn_checker(this),
               ray_checkers(this, bishop_bandits(this)),
               ray_checkers(this, rook_bandits(this)));
  }
}

#ifdef KATOR_USE_BOARD_VECTOR_64
#ifdef KATOR_HAS_X64_512BIT_BUILTINS

#error not yet implemented

static inline void vector_board_copy_and_flip(void* dst, const void* src)
{
  local_init_x64_vectors_64();

  double* vdst = (double*)dst;
  const double* vsrc = (double*)src;
  __m512d temp;

  temp = _mm512_load_pd(vsrc);
  temp = _mm512_permutexvar_pd(temp, permutation_selector);
  _mm512_store_pd(vdst);
}

#elif defined(KATOR_HAS_X64_256BIT_AVX2_BUILTINS)

static inline void vector_board_copy_and_flip(void* dst, const void* src)
{
  //local_init_x64_vectors_32();

  //constexpr uint8_t permutation_selector = 0 + (1 << 4) + (2 << 2) + (3 << 0);

  double* RESTRICT vdst = (double*)dst;
  const double* RESTRICT vsrc = (double*)src;
  __m256d temp0, temp1;

  temp0 = _mm256_load_pd(vsrc);
  temp1 = _mm256_load_pd(vsrc + 4);
  temp0 = _mm256_permute4x64_pd(temp0, 0 + (1 << 4) + (2 << 2) + (3 << 0));
  temp1 = _mm256_permute4x64_pd(temp1, 0 + (1 << 4) + (2 << 2) + (3 << 0));
  _mm256_store_pd(vdst + 4, temp0);
  _mm256_store_pd(vdst, temp1);
}

#elif defined(KATOR_HAS_X64_256BIT_BUILTINS)

static inline void vector_board_copy_and_flip(void* dst, const void* src)
{
//  local_init_x64_vectors_32();

  constexpr int permutation_selector = 1 + (1 << 2);

  double* RESTRICT vdst = static_cast<double*>(dst);
  const double* RESTRICT vsrc = static_cast<const double*>(src);
  __m256d temp0, temp1;

  temp0 = _mm256_load_pd(vsrc);
  temp1 = _mm256_load_pd(vsrc + 4);
  temp0 = _mm256_permute_pd(temp0, permutation_selector);
  temp1 = _mm256_permute_pd(temp1, permutation_selector);
  temp0 = _mm256_permute2f128_pd(temp0, temp0, 1);
  temp1 = _mm256_permute2f128_pd(temp1, temp1, 1);
  _mm256_store_pd(vdst + 4, temp0);
  _mm256_store_pd(vdst, temp1);
}

#elif defined(KATOR_HAS_X64_128BIT_BUILTINS)
// SSE2

static inline void vector_board_copy_and_flip(void* dst, const void* src)
{
  constexpr int permutation_selector = 1 + (1 << 2);

  double* RESTRICT vdst = static_cast<double*>(dst);
  const double* RESTRICT vsrc = static_cast<const double*>(src);
  __m128d temp0, temp1, temp2, temp3;

  temp0 = _mm_load_pd(vsrc);
  temp1 = _mm_load_pd(vsrc + 2);
  temp2 = _mm_load_pd(vsrc + 4);
  temp3 = _mm_load_pd(vsrc + 6);
  _mm_storel_pd(vdst + 7, temp0);
  _mm_storeh_pd(vdst + 6, temp0);
  _mm_storel_pd(vdst + 5, temp1);
  _mm_storeh_pd(vdst + 4, temp1);
  _mm_storel_pd(vdst + 3, temp2);
  _mm_storeh_pd(vdst + 2, temp2);
  _mm_storel_pd(vdst + 1, temp3);
  _mm_storeh_pd(vdst + 0, temp4);


	/*
  local_init_x64_vectors_16();

  __m64* vdst = (__m64*)dst;
  const __m128i* vsrc = (__m128i*)src;
  __m128i temp0, temp1;

  temp0 = _mm_xor_si128(vsrc[0], vector_16_one);
  temp1 = _mm_cmpeq_epi8(vsrc[0], vector_16_zero);
  temp1 = _mm_andnot_si128(temp1, temp0);
  _mm_storel_p(vdst + 7, temp1);
  _mm_storeh_pi(vdst + 6, temp1);
  temp0 = _mm_xor_si128(vsrc[1], vector_16_one);
  temp1 = _mm_cmpeq_epi8(vsrc[1], vector_16_zero);
  temp1 = _mm_andnot_si128(temp1, temp0);
  _mm_storel_pi(vdst + 5, temp1);
  _mm_storeh_pi(vdst + 4, temp1);
  temp0 = _mm_xor_si128(vsrc[2], vector_16_one);
  temp1 = _mm_cmpeq_epi8(vsrc[2], vector_16_zero);
  temp1 = _mm_andnot_si128(temp1, temp0);
  _mm_storel_pi(vdst + 3, temp1);
  _mm_storeh_pi(vdst + 2, temp1);
  temp0 = _mm_xor_si128(vsrc[3], vector_16_one);
  temp1 = _mm_cmpeq_epi8(vsrc[3], vector_16_zero);
  temp1 = _mm_andnot_si128(temp1, temp0);
  _mm_storel_pi(vdst + 1, temp1);
  _mm_storeh_pi(vdst + 0, temp1);
  */
}

#else
#  error No compiler vector support found for use with piece board
#endif

#endif // KATOR_USE_BOARD_VECTOR_64

static inline void
board_copy_and_flip(unsigned char* RESTRICT dst,
                    const unsigned char* RESTRICT src)
{
#ifdef KATOR_USE_BOARD_VECTOR_64

  vector_board_copy_and_flip(dst, src);

#else

  for (int i = 0; i < 64; i += 8) {
    std::memcpy(dst + (56 - i), src + i, 8);
  }

#endif // KATOR_USE_BOARD_VECTOR_64
}

static inline bool
has_potential_ep_captor(const position* position, sq_index victim) noexcept
{
  if (victim.file() != file_a
      and position->has_piece_at(left_of(victim), pawn))
  {
    return true;
  }
  if (victim.file() != file_h
      and position->has_piece_at(right_of(victim), pawn))
  {
    return true;
  }
  return false;
}

namespace
{

void build_board(const std::array<real_square, 64>& src,
                 std::function<void(sq_index, square)> insert,
                 const real_player player_to_move)
{
  position_player effective_player = ((player_to_move == white)
                                       ? position_player::to_move
                                       : position_player::opponent);
  for (auto index : sq_index::range()) {
    real_square src_square = src[index.offset()];
    if (not src_square.is_empty) {
      sq_index effective_index = index.effective_of(effective_player);

      position_player player = ((src_square.piece.player == player_to_move)
                                 ? position_player::to_move
                                 : position_player::opponent);

      insert(effective_index, make_square(src_square.piece.type, player));
    }
  }
}

}

position::position(const std::array<real_square, 64>& ctor_board,
                   kator::castle_rights ctor_castle,
                   const sq_index en_passant_square,
                   const real_player player_to_move)
  //castle(ctor_castle, (player_to_move == white) ? to_move : opponent),
  //en_passant_index(en_passant_square)
{
  using namespace ::std::placeholders;

  new(castle())
    castle_rights(ctor_castle, (player_to_move == white) ? to_move : opponent);
  new(en_passant_index()) sq_index(en_passant_square);
  clear_board_and_piece_maps();
  build_board(ctor_board,
              std::bind(&position::add_piece, this, _1, _2),
              player_to_move);
  update_player_maps(player_map(), piece_map());
  setup_occupied();
  generate_attack_maps();
  if (not are_king_maps_sane(this)) {
    throw invalid_king_positions();
  }
  if (not is_en_passant_index_valid(this)) {
    throw invalid_en_passant_index();
  }
  if (not are_castling_rights_valid(this)) {
    throw invalid_castle_rights();
  }
  setup_zhash(this, zhash_pair());
}

namespace
{

#ifdef KATOR_USE_PIECE_MAP_VECTOR
#  ifdef KATOR_HAS_X64_512BIT_BUILTINS

     void vector_piece_map_copy_and_flip(uint64_t* RESTRICT dst,
                                         const uint64_t* RESTRICT src)
     {
       __m512i* src64 = (__m512i*)src;
       __m512i* dst64 = (__m512i*)dst;

       dst64[0] = _mm512_shuffle_epi8(src64[0], bswap_shuffle_key_64);
       dst64[1] = _mm512_shuffle_epi8(src64[1], bswap_shuffle_key_64);
     }


#  elif defined(KATOR_HAS_X64_256BIT_AVX2_BUILTINS)

     void vector_piece_map_copy_and_flip(uint64_t* RESTRICT dst,
                                         const uint64_t* RESTRICT src)
     {
       __m256i* src32 = (__m256i*)src;
       __m256i* dst32 = (__m256i*)dst;

       dst32[0] = _mm256_shuffle_epi8(src32[0], bswap_shuffle_key_32);
       dst32[1] = _mm256_shuffle_epi8(src32[1], bswap_shuffle_key_32);
       dst32[2] = _mm256_shuffle_epi8(src32[2], bswap_shuffle_key_32);
     }

#  elif defined(KATOR_HAS_X64_128BIT_SSSE3_SHUFFLE_BUILTIN)

     void vector_piece_map_copy_and_flip(uint64_t* RESTRICT dst,
                                         const uint64_t* RESTRICT src)
     {
       __m128i* src16 = (__m128i*)src;
       __m128i* dst16 = (__m128i*)dst;

       for (size_t i = 0; i < 6; ++i) {
         dst16[i] = _mm_shuffle_epi8(src16[i], bswap_shuffle_key_16);
       }
     }


#  else
#    error No compiler vector support found for use with piece_maps
#  endif
#endif // KATOR_USE_PIECE_MAP_VECTOR

} /* anonym namespace */

inline void
position::piece_map_copy_and_flip(const position& parent)
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

#ifdef KATOR_USE_PIECE_MAP_VECTOR
  vector_piece_map_copy_and_flip(raw64 + offset_piece_maps,
                                 parent.raw64 + offset_piece_maps);
#else


  for (auto type : all_piece_types()) {
    kator::square square = make_square(type, to_move);
    kator::square opponent_square = make_square(type, opponent);

    piece_map()[square] =
      kator::flip(parent.map_of(opponent_square));
    piece_map()[opponent_square] =
      kator::flip(parent.map_of(square));
  }
#endif
}

inline void position::clear_board_and_piece_maps()
{
  board.fill(0);

  size_t piece_maps_size = sizeof(bitboard) * (piece_array_size - 2);
  std::memset(raw64 + offset_piece_maps, 0, piece_maps_size);
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

  if (move.is_capture()) {
    piece_map_remove(move.to, make_square(move.captured()));
    zhash_pair()->xor_piece(make_square(move.captured()), move.to);
  }

  auto result_piece = make_square(move.result(), opponent);

  piece_map_add(result_piece, move.to);
  zhash_pair()->xor_piece(result_piece, move.to);
  set_board_at(move.to, move.result());

  auto original_piece = make_square(piece_at(move.from), opponent);

  piece_map_remove(move.from, original_piece);
  zhash_pair()->xor_piece(original_piece, move.from);
  clear_board_at(move.from);
}

inline void position::handle_castle_rights(move move)
{
  if (castle()->clears_right_opponent_queenside(move)) {
    castle()->clear_side(castle_rights::side::opponent_queenside);
    zhash_pair()->xor_castle_right(castle_rights::side::opponent_queenside);
  }
  if (castle()->clears_right_opponent_kingside(move)) {
    castle()->clear_side(castle_rights::side::opponent_kingside);
    zhash_pair()->xor_castle_right(castle_rights::side::opponent_kingside);
  }
  if (castle()->clears_right_queenside(move)) {
    castle()->clear_side(castle_rights::side::queenside);
    zhash_pair()->xor_castle_right(castle_rights::side::queenside);
  }
  if (castle()->clears_right_kingside(move)) {
    castle()->clear_side(castle_rights::side::kingside);
    zhash_pair()->xor_castle_right(castle_rights::side::kingside);
  }
}

void position::handle_special_move(const position& parent, move move)
{
  switch (move.move_type) {
    case move::en_passant:
      {
        sq_index ep = parent.en_passant_index()->flipped();

        clear_board_at(ep);
        piece_map_remove(ep, pawn);
        zhash_pair()->xor_piece(pawn, ep);
      }
      break;

    case move::castle_kingside:
      set_board_at(f8, piece::rook);
      clear_board_at(h8);
      piece_map()[opponent_rook] ^= bitboard(f8, h8);
      zhash_pair()->xor_piece(rook, f8);
      zhash_pair()->xor_piece(rook, h8);
      break;

    case move::castle_queenside:
      set_board_at(d8, piece::rook);
      clear_board_at(a8);
      piece_map()[opponent_rook] ^= bitboard(d8, a8);
      zhash_pair()->xor_piece(rook, d8);
      zhash_pair()->xor_piece(rook, a8);
      break;

    case move::pawn_double_push:
    case move::promotion:
    case move::general:
      break;
  }
}

position::position(const position& parent, move move) noexcept
{
  board_copy_and_flip(board.data(), parent.board.data());
  piece_map_copy_and_flip(parent);
  new(castle()) castle_rights(parent.castle()->flipped());

  /* Start with flipping the move, because the coordinates in the move{{{
     refer the squares on the original board. At this point the new board
     already shows the point of view of the next player, thus in essence
     this is the move the opponent has just made. This is also the reason
     why the opponent_of function is needed here.
  }}}*/
  move.flip();

  move_primary_piece(move);
  handle_special_move(parent, move);
  update_player_maps(player_map(), piece_map());
  setup_occupied();
  generate_attack_maps();
  handle_castle_rights(move);

  if (move.is_double_pawn_push() and has_potential_ep_captor(this, move.to)) {
    en_passant_index()[0] = move.to;
  }
  else {
    en_passant_index()->unset();
  }

}

string position::generate_castle_FEN(real_player point_of_view) const
{
  return castle()->generate_castle_FEN(point_of_view);
}

} /* namespace kator */
