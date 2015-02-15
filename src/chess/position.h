
#ifndef KATOR_POSITION_H
#define KATOR_POSITION_H

#include "move.h"
#include "castle_rights.h"
#include "platform_low_level.h"
#include "bitboard.h"
#include "zobrist_hash.h"

#include <array>
#include <string>

namespace kator
{
namespace chess
{

class move_list;

class alignas(64) position:
  public castle_rights
{

private:

  static constexpr unsigned piece_map_padding = 2;


#ifdef USE_ALIGNAS_64

public:
  void* operator new(size_t);
  void operator delete(void*);

private:

  struct alignas(64) board_internal
  {
#ifdef USE_BOARD_VECTOR_64
    vector_8_64 squares;
#else
    alignas(64) std::array<unsigned char, 64> squares;
#endif
  };

  board_internal board;

  alignas(64)
    std::array<bitboard, piece_map_padding + piece::array_size> piece_map_raw;

#else // USE_ALIGNAS_64

  struct board_internal
  {
#ifdef USE_BOARD_VECTOR_64
    vector_8_64 squares;
#else
    std::array<unsigned char, 64> squares;
#endif
  };

  board_internal board;

  std::array<bitboard, piece_map_padding + piece::array_size> piece_map_raw;

#endif // USE_ALIGNAS_64

  std::array<bitboard, piece::array_size> attack_map;
  bitboard occupied;
  bitboard king_attack_map;
  sq_index en_passant_index;
  zobrist_hash_pair zhash;


  bitboard& piece_map(unsigned index)
  {
    return piece_map_raw[piece_map_padding + index];
  }

  const bitboard& piece_map(unsigned index) const
  {
    return piece_map_raw[piece_map_padding + index];
  }

  bitboard& attacks_of(piece piece)
  {
    return attack_map[piece.index()];
  }

  bitboard& map_of(piece piece)
  {
    return piece_map(piece.index());
  }

  bitboard& map_of(player player)
  {
    return piece_map(static_cast<unsigned>(player));
  }

public:

  class piece_board
  {
    alignas(64) board_internal board;

  public:
    
    piece_board(std::string fen);
    friend class position;
  }; // class position::piece_board

  position(const position& parent, move) noexcept;

  position(const piece_board&,
           castle_rights,
           const sq_index en_passant_square,
           const player point_of_view);

  position(const position&) = default;

  sq_index ep_index() const
  {
    return en_passant_index;
  }

  bool has_en_passant_square() const
  {
    return en_passant_index.is_set();
  }

  zobrist_hash get_zhash() const
  {
    return zhash;
  }

  bitboard get_king_attack_map() const
  {
    return king_attack_map;
  }

  std::string generate_board_FEN(player point_of_view) const noexcept;
  std::string generate_ep_FEN(real_player point_of_view) const noexcept;

  bool multiple_checkers() const
  {
    return not is_empty_or_singular(
                  intersection_of(king_attack_map,
                                  map_of(player::opponent)));
  }

  static void lookup_tables_init();

  piece piece_at(sq_index index) const
  {
    unsigned char piece_index = board.squares[index.offset()];

    return piece(static_cast<piece::piece_index>(piece_index));
  }

  player player_at(sq_index index) const
  {
    static_assert(static_cast<int>(player::to_move) == 0
                  and static_cast<int>(player::opponent) == 1,
                  "invalid player enum values");
    return static_cast<player>(board.squares[index.offset()] & 1);
  }

  bool has_piece_at(sq_index index) const
  {
    return board.squares[index.offset()] != 0;
  }

  piece piece_at(rank rank, file file) const
  {
    return piece_at(sq_index(rank, file));
  }

  bool has_piece_at(rank rank, file file) const
  {
    return has_piece_at(sq_index(rank, file));
  }

  player player_at(rank rank, file file) const
  {
    return player_at(sq_index(rank, file));
  }

  const bitboard& map_of(piece piece) const
  {
    return piece_map(piece.index());
  }

  const bitboard& map_of(player player) const
  {
    return piece_map(static_cast<unsigned>(player));
  }

  bitboard occupied_map() const
  {
    return occupied;
  }

  bitboard bishop_queen_map() const
  {
    return map_of(bishop) | map_of(queen);
  }

  bitboard rook_queen_map() const
  {
    return map_of(rook) | map_of(queen);
  }

  bitboard opponent_bishop_queen_map() const
  {
    return map_of(opponent_bishop) | map_of(opponent_queen);
  }

  bitboard opponent_rook_queen_map() const
  {
    return map_of(opponent_rook) | map_of(opponent_queen);
  }

  bitboard attacks_of(piece piece) const
  {
    return attack_map[piece.index()];
  }

  bitboard attacks_of(player player) const
  {
    return attack_map[static_cast<unsigned>(player)];
  }

  bool in_check() const
  {
    return is_nonempty(king_attack_map);
  }

  sq_index king_index() const
  {
    return map_of(king).lsb_index();
  }

  sq_index opponent_king_index() const
  {
    return map_of(opponent_king).lsb_index();
  }

  bool is_unoccupied(sq_index index) const
  {
    return not occupied.is_bit_set(index);
  }

  template<typename... indices_type>
  bool is_any_unoccupied(indices_type... indices) const
  {
    return are_disjoint(occupied, bitboard(indices...));
  }

  bool is_attacked_by(player player, sq_index index) const
  {
    return attacks_of(player).is_bit_set(index);
  }

  template<typename... indices_type>
  bool is_any_attacked_by(player player, indices_type... indices) const
  {
    return is_nonempty(
             intersection_of(attacks_of(player), bitboard(indices...)));
  }

private:

  void add_piece(sq_index, piece);
  void add_piece(rank, file, piece);
  void clear_square(sq_index);
  void clear_square(rank, file);

  void piece_map_remove(sq_index);
  void piece_map_remove(sq_index, piece);
  void piece_map_add(piece, sq_index);

  void move_primary_piece(move);
  void handle_special_move(const position&, move);
  void handle_castle_rights(move);
  void board_copy_and_flip(board_internal& RESTRICT,
                           const board_internal& RESTRICT);
  void piece_map_copy_and_flip(const position&);
  void generate_piece_maps();
  void generate_attacks_by_piece();
  void generate_opponent_attacks_by_piece();
  void generate_attack_maps();

}; /* class position */

#ifdef USE_ALIGNAS_64
typedef aligned_64_allocator<position> position_allocator;
#else
typedef std::allocator<position> position_allocator;
#endif

} /* namespace kator::chess */
} /* namespace kator */

#endif /* !defined(KATOR_POSITION_H) */

