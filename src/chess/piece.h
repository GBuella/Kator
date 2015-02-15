
#ifndef KATOR_CHESS_PIECE_H
#define KATOR_CHESS_PIECE_H

#include "player.h"

namespace kator
{
namespace chess
{

class piece
{

public:

  static constexpr unsigned array_size = 14;

  enum class piece_index : unsigned char
  {
    nonpiece =        0,
    index_1 =         1,
    pawn =            2,
    opponent_pawn =   3,
    rook =            4,
    opponent_rook =   5,
    king =            6,
    opponent_king =   7,
    bishop =          8,
    opponent_bishop = 9,
    knight =          10,
    opponent_knight = 11,
    queen =           bishop | rook,    // 4 | 8 == 12
    opponent_queen  = queen | 1         //          13
  };


private:

  piece_index value;

public:

  enum class leave { uninitialized };
  piece(leave) {}

  constexpr piece(piece_index arg): value(arg) {}

  constexpr unsigned char index() const
  {
    return static_cast<unsigned char>(value);
  }

  constexpr bool is_set() const
  {
    return value != piece_index::nonpiece;
  }

  constexpr chess::player player() const
  {
    return static_cast<chess::player>(index() & 1);
  }

  constexpr piece basic() const
  {
    return piece(static_cast<piece_index>(index() & ~1));
  }

  constexpr bool is_bishop_or_queen() const
  {
    return (index() & 0xa) == 0x8;
  }

  constexpr bool is_rook_or_queen() const
  {
    return (index() & 0x6) == 0x4;
  }

  constexpr bool is_pawn() const
  {
    return value == piece_index::pawn || value == piece_index::opponent_pawn;
  }

  std::string to_str(chess::player point_of_view) const;
  std::string to_figurine(chess::player point_of_view) const;
  std::string to_str(chess::player point_of_view, bool use_figurines) const;
  static bool is_valid_char(char);
  explicit piece(char);

  constexpr bool operator == (const piece& other) const
  {
    return value == other.value;
  }

  constexpr bool operator != (const piece& other) const
  {
    return value != other.value;
  }

  class all_pieces
  {
    class iterator
    {
      int value;

      constexpr iterator(int arg): value(arg) {}
      
    public:

      constexpr bool operator != (const iterator& other) const
      {
        return value != other.value;
      }

      iterator& operator ++ ()
      {
        value += 2;
        return *this;
      }

      piece operator * () const
      {
        return piece(static_cast<piece_index>(value));
      }

      friend all_pieces;
    };

    public:

    constexpr all_pieces() {}


    constexpr iterator begin() const
    {
      return iterator(2);
    }

    constexpr iterator end() const
    {
      return iterator(array_size);
    }

  };

}; /* class piece */

constexpr chess::piece opponent_of(chess::piece arg)
{
  return chess::piece(static_cast<chess::piece::piece_index>(arg.index() ^ 1));
}

constexpr chess::piece nonpiece(chess::piece::piece_index::nonpiece);
constexpr chess::piece pawn(chess::piece::piece_index::pawn);
constexpr chess::piece opponent_pawn(chess::piece::piece_index::opponent_pawn);
constexpr chess::piece rook(chess::piece::piece_index::rook);
constexpr chess::piece opponent_rook(chess::piece::piece_index::opponent_rook);
constexpr chess::piece king(chess::piece::piece_index::king);
constexpr chess::piece opponent_king(chess::piece::piece_index::opponent_king);
constexpr chess::piece bishop(chess::piece::piece_index::bishop);
constexpr chess::piece opponent_bishop(chess::piece::piece_index::opponent_bishop);
constexpr chess::piece knight(chess::piece::piece_index::knight);
constexpr chess::piece opponent_knight(chess::piece::piece_index::opponent_knight);
constexpr chess::piece queen(chess::piece::piece_index::queen);
constexpr chess::piece opponent_queen(chess::piece::piece_index::opponent_queen);

} /* namespace kator::chess */
} /* namespace kator */

#endif /* !defined(KATOR_CHESS_PIECE_H) */
