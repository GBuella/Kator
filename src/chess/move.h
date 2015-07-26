
#ifndef KATOR_CHESS_MOVE_H
#define KATOR_CHESS_MOVE_H

#include <cassert>
#include <array>
#include <cstring>

#include "chess.h"
#include "bitboard.h"

namespace kator
{

namespace engine
{
class node;
}

namespace chess
{

struct move
{
  sq_index from;
  sq_index to;

  enum move_type_enum {
    general = 0,
    pawn_double_push = 1,
    castle_kingside = 2,
    castle_queenside = 3,
    en_passant = 4,
    promotion = 5,
  };


  constexpr bool is_general_move() const;
  constexpr bool is_special_move() const;
  constexpr bool is_pawn_push() const;
  constexpr bool is_double_pawn_push() const;
  constexpr bool is_en_passant() const;
  constexpr bool is_promotion() const;
  constexpr bool is_capture() const;
  constexpr bool is_reversible() const;
  constexpr bool is_irreversible() const;
  constexpr bool is_castle_kingside() const;
  constexpr bool is_castle_queenside() const;
  constexpr bool operator== (const move&) const;
  constexpr bool operator!= (const move&) const;
  constexpr move flipped() const;


  move_type_enum move_type:3;

  /* The resulting piece on the to square,
     either the piece being moved, or the promotion. */
  //piece result_piece;
  unsigned char result_piece:5;

  constexpr piece result() const;

  /* Stored solely for bookkeeping */
  piece captured_piece;

  /* for indexing a lookup table */
  constexpr short change_index() const;
  static constexpr short change_index(piece result_piece,
                                      piece captured_piece,
                                      move_type_enum);

  template<typename T>
  using change_array_t = std::array<T, 0x8000>;

  constexpr move(sq_index ctor_from,
                 sq_index ctor_to,
                 piece ctor_piece);

  constexpr move(sq_index ctor_from,
                 sq_index ctor_to,
                 piece ctor_piece,
                 move_type_enum);

  constexpr move(sq_index ctor_from,
                 sq_index ctor_to,
                 piece ctor_piece,
                 piece ctor_captured,
                 move_type_enum);

  void flip();

  friend class move_list;

  static constexpr move null();

private:

  move();

}; /* class move */

constexpr piece move::result() const
{
  return piece(static_cast<piece::piece_index>(result_piece));
}

constexpr move::move(sq_index ctor_from,
                     sq_index ctor_to,
                     piece ctor_piece):
    from(ctor_from),
    to(ctor_to),
    move_type(general),
    result_piece(ctor_piece.index()),
    captured_piece(nonpiece)
  {}

constexpr move::move(sq_index ctor_from,
                     sq_index ctor_to,
                     piece ctor_piece,
                     move::move_type_enum ctor_move_type):
    from(ctor_from),
    to(ctor_to),
    move_type(ctor_move_type),
    result_piece(ctor_piece.index()),
    captured_piece(nonpiece)
  {}

constexpr move::move(sq_index ctor_from,
                     sq_index ctor_to,
                     piece ctor_piece,
                     piece ctor_captured,
                     move::move_type_enum ctor_move_type = move::general):
    from(ctor_from),
    to(ctor_to),
    move_type(ctor_move_type),
    result_piece(ctor_piece.index()),
    captured_piece(ctor_captured)
  {}

inline void move::flip()
{
  from.flip();
  to.flip();
}

constexpr move move::flipped() const
{
  return move(from.flipped(), to.flipped(),
              result(), captured_piece, move_type);
}

inline move::move():
    from(sq_index::leave::uninitialized),
    to(sq_index::leave::uninitialized),
    //result_piece(piece::leave::uninitialized),
    captured_piece(piece::leave::uninitialized)
  {}

constexpr bool move::is_general_move() const
{
  return move_type == general;
}

constexpr bool move::is_special_move() const
{
  return not is_general_move();
}

constexpr bool move::is_pawn_push() const
{
  return (result_piece == pawn.index() or is_promotion()) and not is_capture();
}

constexpr bool move::is_double_pawn_push() const
{
  return move_type == pawn_double_push;
}

constexpr bool move::is_en_passant() const
{
  return move_type == en_passant;
}

constexpr bool move::is_capture() const
{
  return captured_piece.is_set();
}

constexpr bool move::is_castle_kingside() const
{
  return move_type == castle_kingside;
}

constexpr bool move::is_castle_queenside() const
{
  return move_type == castle_queenside;
}

constexpr bool move::is_promotion() const
{
  return move_type == promotion;
}

constexpr bool move::is_reversible() const
{
  return is_general_move()
         and not is_capture()
         and result_piece != pawn.index();
}

constexpr bool move::is_irreversible() const
{
  return not is_reversible();
}

constexpr bool move::operator== (const move& other) const
{
  return from               == other.from
         and to             == other.to
         and result_piece   == other.result_piece;
}

constexpr bool move::operator!= (const move& other) const
{
  return not (*this == other);
}

constexpr short move::change_index() const
{
  return move_type + (result_piece << 3) + (captured_piece.index() << 8);
}

constexpr short move::change_index(piece result_piece,
                                   piece captured_piece,
                                   move::move_type_enum type = move::general)
{
  return move(a1, a1, result_piece, captured_piece, type).change_index();
}

constexpr move move::null()
{
  return move(sq_index::null(), sq_index::null(), nonpiece);
}

constexpr move null_move = move::null();

constexpr move castle_queenside(e1, c1, king, move::castle_queenside);
constexpr move castle_kingside(e1, g1, king, move::castle_kingside);

constexpr move white_castle_queenside(e1, c1, king, move::castle_queenside);
constexpr move white_castle_kingside(e1, g1, king, move::castle_kingside);
constexpr move black_castle_queenside(e8, c8, king, move::castle_queenside);
constexpr move black_castle_kingside(e8, g8, king, move::castle_kingside);

} /* namespace kator::chess */
} /* namespace kator */

#endif /* !defined(KATOR_CHESS_MOVE_H) */
