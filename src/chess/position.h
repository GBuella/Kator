
#ifndef KATOR_POSITION_H
#define KATOR_POSITION_H

#include "move.h"
#include "castle_rights.h"
#include "platform/platform.h"
#include "bitboard.h"
#include "zobrist_hash.h"

#include <algorithm>
#include <array>
#include <string>
#include <istream>

namespace kator
{

class move_list;

class position
{
public: /* The public interface */
  static constexpr position_player to_move = player_to_move;
  static constexpr position_player opponent = player_opponent;

  static void* operator new(size_t);
  static void operator delete(void*);

  position(const position&, move) noexcept /* GCC __attribute__((hot)) */;
  position(const position&) = default;

  static void lookup_tables_init();

  template<typename... types> bitboard map_of(unsigned piece, types...) const;
  template<typename... types> bitboard map_of(position_player, types...) const;
  bitboard occupied() const;
  template<typename... indices> bool is_any_unoccupied(indices...) const;
  bool is_unoccupied(sq_index) const;
  bitboard bishop_queen_map() const;
  bitboard rook_queen_map() const;
  bitboard opponent_bishop_queen_map() const;
  bitboard opponent_rook_queen_map() const;

  square square_at(sq_index) const;
  square square_at(rank, file) const;
  piece piece_at(sq_index) const;
  piece piece_at(rank, file) const;
  bool has_piece_at(sq_index) const;
  bool has_piece_at(sq_index, unsigned piece) const;
  bool has_piece_at(rank, file) const;
  bool has_piece_at(rank, file, unsigned piece) const;
  position_player player_at(sq_index) const;
  position_player player_at(rank, file) const;

  sq_index ep_index() const;
  bool has_en_passant_square() const;

  bitboard attacks_of(unsigned index) const;
  bitboard attacks_of(position_player) const;
  bool is_attacked_by(position_player, sq_index) const;
  template<typename... indices>
  bool is_any_attacked_by(position_player, indices...) const;


  sq_index king_index() const;
  sq_index opponent_king_index() const;
  bool in_check() const;
  bitboard checkers() const;
  bool has_multiple_checkers() const;
  bitboard king_attack_map() const;

  bool can_castle_queenside() const;
  bool opponent_can_castle_queenside() const;
  bool can_castle_kingside() const;
  bool opponent_can_castle_kingside() const;
  bool opponent_can_castle_any() const;
  bool can_castle_any() const;
  std::string generate_castle_FEN(real_player point_of_view) const;

  zobrist_hash get_zhash() const;

private:


  /* The internal layout, all the actual members that store the {{{
     position representation. The board is for looking up piece types,
     holding no information about which side a piece belongs to. Empty
     squares are marked with zero. During make_move the board is copied
     into the new position potentially using SIMD instructions, so an
     alignment might be necessary.
     The raw64 array stores all bitboards mapping pieces. In the
     beginning of the array are the bitboards representing the
     positions of each distinct piece type, separated by side
     as in pawn, opponent_pawn etc..
     During make_move these are also copied into the new position using
     SIMD, that is why they are also aligned. The rest of the bitboards
     are not copied, but generated from the board and these piece maps,
     following the appropiate changes ( e.g.: move piece from one square
     to another square ) . The rest of the bitboards are:
     one occupancy map for each side - two bitboards at offset_player_map
     one occupancy map for all pieces - at offset_occupied
     one attack map per each side, and one per each piece - 
       14 altogether ( 2 sides + 6 piece types for each side )
       these store information about squares attacked by each piece/side
     one bitboard storing all squares that take part in currently
       attacking the king of the player to move - e.g.: in case of a bishop
       holding the king in check, the king_attack_map refers to the square
       of said bishop and the squares enclosed between the bishop and
       the king in the related diagonal. Of course, when the king is not
       in check, this bitboard is empty.
   }}}*/

  alignas(critical_alignment) std::array<unsigned char, 64> board;

  enum {
    offset_occupied,
    offset_king_attack_map,
    offset_attack_player_maps,
    offset_attack_player_map_second,
    offset_player_maps,
    offset_player_map_second,
    offset_extra_64_0,
    offset_extra_64_1,
    offset_piece_maps,
    offset_zhash_pair = offset_piece_maps + piece_array_size - 2,
    offset_zhash_pair_second,
    raw64_padding_0,
    raw64_padding_1,
    offset_attack_maps,
    uint64_array_size = offset_attack_maps + piece_array_size - 2
  };

  alignas(critical_alignment) uint64_t raw64[uint64_array_size];

  /* end of memory layout */

  castle_rights* castle()
  {
    return (castle_rights*)(raw64 + offset_extra_64_0);
  }

  const castle_rights* castle() const
  {
    return (castle_rights*)(raw64 + offset_extra_64_0);
  }

  sq_index* en_passant_index()
  {
    return (sq_index*)(raw64 + offset_extra_64_1);
  }

  const sq_index* en_passant_index() const
  {
    return (sq_index*)(raw64 + offset_extra_64_1);
  }

  static_assert(sizeof(bitboard) == sizeof(decltype(*raw64)),
                "internal type error");

  static_assert(sizeof(zobrist_hash_pair) == 2 * sizeof(decltype(*raw64)),
                "internal type error");

  static_assert(
    ((size_t(offset_piece_maps) * sizeof(decltype(*raw64)))
    % critical_alignment_divisor) == 0,
    "Alignment error");

  static_assert(
    ((size_t(offset_attack_maps) * sizeof(decltype(*raw64)))
    % critical_alignment_divisor) == 0,
    "Alignment error");

  bitboard* as_bitboard(size_t offset)
  {
    return reinterpret_cast<bitboard*>(raw64 + offset);
  }

  const bitboard* as_const_bitboard(size_t offset) const
  {
    return reinterpret_cast<const bitboard*>(raw64 + offset);
  }

  /* accessor functions for objects in the 'raw64'
   */

  bitboard* piece_map()
  {
    return as_bitboard(offset_piece_maps - 2);
  }

  const bitboard* piece_map() const
  {
    return as_const_bitboard(offset_piece_maps - 2);
  }

  bitboard* player_map()
  {
    return as_bitboard(offset_player_maps);
  }

  const bitboard* player_map() const
  {
    return as_const_bitboard(offset_player_maps);
  }

  bitboard* attack_map()
  {
    return as_bitboard(offset_attack_maps - 2);
  }

  const bitboard* attack_map() const
  {
    return as_const_bitboard(offset_attack_maps - 2);
  }

  bitboard* player_attack_map()
  {
    return as_bitboard(offset_attack_player_maps);
  }

  const bitboard* player_attack_map() const
  {
    return as_const_bitboard(offset_attack_player_maps);
  }

  bitboard* address_occupied()
  {
    return as_bitboard(offset_occupied);
  }

  const bitboard* address_occupied() const
  {
    return as_const_bitboard(offset_occupied);
  }

  bitboard* address_king_attack_map()
  {
    return as_bitboard(offset_king_attack_map);
  }

  const bitboard* address_king_attack_map() const
  {
    return as_const_bitboard(offset_king_attack_map);
  }

  zobrist_hash_pair* zhash_pair()
  {
    auto pointer = raw64 + offset_zhash_pair;

    return reinterpret_cast<zobrist_hash_pair*>(pointer);
  }

  const zobrist_hash_pair* zhash_pair() const
  {
    const auto pointer = raw64 + offset_zhash_pair;

    return reinterpret_cast<const zobrist_hash_pair*>(pointer);
  }

  void clear_board_and_piece_maps();
  void piece_map_copy_and_flip(const position& parent);

private:

  void set_board_at(sq_index, piece);
  void set_board_at(rank, file, piece);
  void clear_board_at(sq_index);
  void clear_board_at(rank, file);
  void setup_occupied();

  void add_piece(sq_index, square);

  void piece_map_remove(sq_index, square);
  void piece_map_add(square, sq_index);

  void move_primary_piece(move);
  void handle_special_move(const position&, move);
  void handle_castle_rights(move);

  void generate_attacks_by_piece();
  void generate_opponent_attacks_by_piece();
  void generate_attack_maps();


  position(const std::array<real_square, 64>&,
           castle_rights,
           const sq_index en_passant_square,
           const real_player player_to_move);

  friend struct game_state;

}; /* class position */

typedef aligned_allocator<position> position_allocator;




/* inline definitions of public interface methods */

template<>
inline bitboard
position::map_of(unsigned piece) const
{
  return piece_map()[piece];
}

template<>
inline bitboard
position::map_of(position_player player) const
{
  return player_map()[offset(player)];
}

template<typename... types>
inline bitboard
position::map_of(unsigned piece, types... tail) const
{
  return union_of(map_of(piece), map_of(tail...));
}

template<typename... types>
inline bitboard
position::map_of(position_player player, types... tail) const
{
  return union_of(map_of(player), map_of(tail...));
}


inline square
position::square_at(sq_index index) const
{
  return make_square(piece_at(index), player_at(index));
}

inline square
position::square_at(rank rank, file file) const
{
  return square_at(sq_index(rank, file));
}

inline sq_index
position::ep_index() const
{
  return en_passant_index()[0];
}

inline bool
position::has_en_passant_square() const
{
  return en_passant_index()->is_set();
}

inline piece
position::piece_at(sq_index index) const
{
  return static_cast<piece>(board[index.offset()]);
}

inline position_player
position::player_at(sq_index index) const
{
  return map_of(opponent).is_bit_set(index) ? opponent : to_move;
}

inline bool
position::has_piece_at(sq_index index) const
{
  return board[index.offset()] != 0;
}

inline bool
position::has_piece_at(sq_index index, unsigned piece) const
{
  return map_of(piece).is_bit_set(index);
}

inline piece
position::piece_at(rank rank, file file) const
{
  return piece_at(sq_index(rank, file));
}

inline bool
position::has_piece_at(rank rank, file file) const
{
  return has_piece_at(sq_index(rank, file));
}

inline bool
position::has_piece_at(rank rank, file file, unsigned piece) const
{
  return has_piece_at(sq_index(rank, file), piece);
}

inline position_player
position::player_at(rank rank, file file) const
{
  return player_at(sq_index(rank, file));
}

inline bitboard
position::king_attack_map() const
{
  return *address_king_attack_map();
}

inline bool
position::in_check() const
{
  return is_nonempty(king_attack_map());
}

inline bitboard
position::checkers() const
{
  return intersection_of(king_attack_map(), map_of(opponent));
}

inline bool
position::has_multiple_checkers() const
{
  return not is_empty_or_singular(checkers());
}

inline bitboard
position::occupied() const
{
  return *address_occupied();
}

template<typename... indices_type>
inline bool position::is_any_unoccupied(indices_type... indices) const
{
  return are_disjoint(occupied(), bitboard(indices...));
}


inline bool position::is_unoccupied(sq_index index) const
{
  return not occupied().is_bit_set(index);
}


inline bitboard
position::bishop_queen_map() const
{
  return map_of(bishop) | map_of(queen);
}

inline bitboard
position::rook_queen_map() const
{
  return map_of(rook) | map_of(queen);
}

inline bitboard
position::opponent_bishop_queen_map() const
{
  return map_of(opponent_bishop) | map_of(opponent_queen);
}

inline bitboard
position::opponent_rook_queen_map() const
{
  return map_of(opponent_rook) | map_of(opponent_queen);
}

inline bitboard
position::attacks_of(unsigned piece) const
{
  return attack_map()[piece];
}

inline bitboard
position::attacks_of(position_player player) const
{
  return player_attack_map()[offset(player)];
}

inline sq_index
position::king_index() const
{
  return map_of(king).lsb_index();
}

inline sq_index
position::opponent_king_index() const
{
  return map_of(opponent_king).lsb_index();
}

inline bool
position::is_attacked_by(position_player player, sq_index index) const
{
  return attacks_of(player).is_bit_set(index);
}

template<typename... indices_type>
inline bool
position::is_any_attacked_by(position_player player,
                                 indices_type... indices) const
{
  return is_nonempty(
           intersection_of(attacks_of(player), bitboard(indices...)));
}

inline bool position::can_castle_queenside() const
{
  return castle()->can_castle_queenside();
}

inline bool position::opponent_can_castle_queenside() const
{
  return castle()->opponent_can_castle_queenside();
}

inline bool position::can_castle_kingside() const
{
  return castle()->can_castle_kingside();
}

inline bool position::opponent_can_castle_kingside() const
{
  return castle()->opponent_can_castle_kingside();
}

inline bool position::opponent_can_castle_any() const
{
  return castle()->opponent_can_castle_any();
}

inline bool position::can_castle_any() const
{
  return castle()->can_castle_any();
}

inline zobrist_hash position::get_zhash() const
{
  return *zhash_pair();
}


} /* namespace kator */

#endif /* !defined(KATOR_POSITION_H) */

