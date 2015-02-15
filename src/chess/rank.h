
#ifndef KATOR_CHESS_RANK_H
#define KATOR_CHESS_RANK_H

#include "coordinate.h"

#include <string>

namespace kator
{
namespace chess
{

class rank: public coordinate<7>
{

  constexpr rank(int arg):
    coordinate<7>(arg) {}

public:

  friend class sq_index;
  friend class bitboard;

  explicit rank(char);
  static bool is_valid_char(char);
  std::string to_str(player point_of_view) const;

  constexpr rank flipped() const
  {
    return rank(7 - value);
  }

  constexpr rank effective_of(player point_of_view) const
  {
    return (point_of_view == player::to_move) ? *this : flipped();
  }

  static constexpr rank back_rank()
  {
    return rank(7);
  }

  static constexpr rank opponent_back_rank()
  {
    return rank(0);
  }

  constexpr bool is_any_back_rank() const
  {
    return value == 0 || value == 7;
  }

  constexpr rank north_shifted() const
  {
    return rank(value - 1);
  }

  constexpr rank south_shifted() const
  {
    return rank(value + 1);
  }

  void shift_north(int delta = 1)
  {
    value -= delta;
  }

  void shift_south(int delta = 1)
  {
    value += delta;
  }

  void flip()
  {
    value = 7 - value;
  }

  class range_1_to_8: public coordinate_range<7, 0, -1, rank>
  {
    public: constexpr range_1_to_8() {}
  };

  class range_8_to_1: public coordinate_range<0, 7, +1, rank>
  {
    public: constexpr range_8_to_1() {}
  };

  friend range_1_to_8::iterator;
  friend range_8_to_1::iterator;

}; /* class rank */

namespace
{

inline constexpr rank north_of(rank arg)
{
  return arg.north_shifted();
}

inline constexpr rank south_of(rank arg)
{
  return arg.south_shifted();
}

inline constexpr rank flip(rank arg)
{
  return arg.flipped();
}

constexpr rank::range_1_to_8 ranks_1_to_8;
constexpr rank::range_8_to_1 ranks_8_to_1;

constexpr rank rank_1 = rank::back_rank();
constexpr rank rank_2 = north_of(rank_1);
constexpr rank rank_3 = north_of(rank_2);
constexpr rank rank_4 = north_of(rank_3);
constexpr rank rank_5 = north_of(rank_4);
constexpr rank rank_6 = north_of(rank_5);
constexpr rank rank_7 = north_of(rank_6);
constexpr rank rank_8 = north_of(rank_7);

} /* anonym namespace */

} /* namespace kator::chess */
} /* namespace kator */

#endif /* !defined(KATOR_CHESS_RANK_H) */

