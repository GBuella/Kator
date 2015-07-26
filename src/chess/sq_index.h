
#ifndef KATOR_CHESS_SQ_INDEX_H
#define KATOR_CHESS_SQ_INDEX_H

#include "player.h"
#include "rank.h"
#include "file.h"

namespace kator
{
namespace chess
{

class sq_index: public coordinate<63>
{

  using coordinate::coordinate;

public:

  constexpr sq_index(chess::rank r, chess::file f):
    coordinate((r.value << 3) | f.value) {}

  static constexpr sq_index none()
  {
    return sq_index(0x80);
  }

  static constexpr sq_index null()
  {
    return sq_index(0);
  }

  std::string to_str(player point_of_view = player::to_move) const;
  sq_index(const std::string& str);

  enum class leave { uninitialized };
  sq_index(leave) {}

  constexpr chess::rank rank() const
  {
    return chess::rank(value >> 3);
  }

  constexpr chess::file file() const
  {
    return chess::file(value & 7);
  }

  void shift_east(int delta = 1)
  {
    ASSUME(is_valid());
    value -= delta;
  }

  void shift_west(int delta = 1)
  {
    ASSUME(is_valid());
    value += delta;
  }

  void shift_left(int delta = 1)
  {
    shift_west(delta);
  }

  void shift_right(int delta = 1)
  {
    shift_east(delta);
  }

  void shift_north(int delta = 1)
  {
    ASSUME(is_valid());
    value -= 8 * delta;
  }

  void shift_south(int delta = 1)
  {
    ASSUME(is_valid());
    value += 8 * delta;
  }

  void shift_north_east()
  {
    shift_north();
    shift_east();
  }

  void shift_north_west()
  {
    shift_north();
    shift_west();
  }

  void shift_south_east()
  {
    shift_north();
    shift_east();
  }

  void shift_south_west()
  {
    shift_south();
    shift_west();
  }

  constexpr sq_index north_shifted() const
  {
    return sq_index(value - 8);
  }

  constexpr sq_index south_shifted() const
  {
    return sq_index(value + 8);
  }

  constexpr sq_index left_shifted() const
  {
    return sq_index(value + 1);
  }

  constexpr sq_index right_shifted() const
  {
    return sq_index(value - 1);
  }

  constexpr sq_index flipped() const
  {
    return sq_index(value ^ 0x38);
  }

  constexpr sq_index effective_of(player point_of_view) const
  {
    return (point_of_view == player::to_move) ? *this : flipped();
  }

  void flip()
  {
    ASSUME(is_valid());
    value ^= 0x38;
  }

  void unset()
  {
    value = 0x80;
  }

  constexpr bool is_set() const
  {
    return value <= 0x3f;
  }

  class direction {
    int rank_delta;
    int file_delta;

    constexpr direction(int rank_arg, int file_arg):
      rank_delta(rank_arg), file_delta(file_arg) {}

    friend sq_index;

  public:

    constexpr direction operator + (const direction& other) const
    {
      return direction(rank_delta + other.rank_delta,
                       file_delta + other.file_delta);
    }
  };

  bool stays_in_board(const direction& direction) const
  {
    if (!chess::rank(this->rank().value + direction.rank_delta).is_valid()) {
      return false;
    }
    if (!chess::file(this->file().value + direction.file_delta).is_valid()) {
      return false;
    }
    return true;
  }

  static constexpr direction east()
  {
    return direction(0, -1);
  }

  static constexpr direction west()
  {
    return direction(0, +1);
  }

  static constexpr direction north()
  {
    return direction(-1, 0);
  }

  static constexpr direction south()
  {
    return direction(1, 0);
  }

  static constexpr direction east(int delta)
  {
    return direction(0, -1 * delta);
  }

  static constexpr direction west(int delta)
  {
    return direction(0, +1 * delta);
  }

  static constexpr direction north(int delta)
  {
    return direction(-1 * delta, 0);
  }

  static constexpr direction south(int delta)
  {
    return direction(+1 * delta, 0);
  }

  constexpr sq_index operator + (const direction& dir) const
  {
    return sq_index(value + dir.rank_delta * 8 + dir.file_delta);
  }

  sq_index& operator += (const direction& dir)
  {
    value += dir.rank_delta * 8 + dir.file_delta;
    return *this;
  }

  class range: public coordinate_range<0, 63, 1, sq_index>
  {
    public: constexpr range() {}
  };

  friend range::iterator;
  friend class bitboard;
  friend struct move;

}; /* class sq_index */

namespace
{

inline constexpr sq_index north_of(sq_index index)
{
  return index.north_shifted();
}

inline constexpr sq_index south_of(sq_index index)
{
  return index.south_shifted();
}

inline constexpr sq_index left_of(sq_index index)
{
  return index.left_shifted();
}

inline constexpr sq_index right_of(sq_index index)
{
  return index.right_shifted();
}

constexpr sq_index::direction south = sq_index::south();
constexpr sq_index::direction north = sq_index::north();
constexpr sq_index::direction west = sq_index::west();
constexpr sq_index::direction east = sq_index::east();

inline constexpr sq_index flip(sq_index index)
{
  return index.flipped();
}

constexpr sq_index a1(rank_1, file_a);
constexpr sq_index a2(rank_2, file_a);
constexpr sq_index a3(rank_3, file_a);
constexpr sq_index a4(rank_4, file_a);
constexpr sq_index a5(rank_5, file_a);
constexpr sq_index a6(rank_6, file_a);
constexpr sq_index a7(rank_7, file_a);
constexpr sq_index a8(rank_8, file_a);

constexpr sq_index b1(rank_1, file_b);
constexpr sq_index b2(rank_2, file_b);
constexpr sq_index b3(rank_3, file_b);
constexpr sq_index b4(rank_4, file_b);
constexpr sq_index b5(rank_5, file_b);
constexpr sq_index b6(rank_6, file_b);
constexpr sq_index b7(rank_7, file_b);
constexpr sq_index b8(rank_8, file_b);

constexpr sq_index c1(rank_1, file_c);
constexpr sq_index c2(rank_2, file_c);
constexpr sq_index c3(rank_3, file_c);
constexpr sq_index c4(rank_4, file_c);
constexpr sq_index c5(rank_5, file_c);
constexpr sq_index c6(rank_6, file_c);
constexpr sq_index c7(rank_7, file_c);
constexpr sq_index c8(rank_8, file_c);

constexpr sq_index d1(rank_1, file_d);
constexpr sq_index d2(rank_2, file_d);
constexpr sq_index d3(rank_3, file_d);
constexpr sq_index d4(rank_4, file_d);
constexpr sq_index d5(rank_5, file_d);
constexpr sq_index d6(rank_6, file_d);
constexpr sq_index d7(rank_7, file_d);
constexpr sq_index d8(rank_8, file_d);

constexpr sq_index e1(rank_1, file_e);
constexpr sq_index e2(rank_2, file_e);
constexpr sq_index e3(rank_3, file_e);
constexpr sq_index e4(rank_4, file_e);
constexpr sq_index e5(rank_5, file_e);
constexpr sq_index e6(rank_6, file_e);
constexpr sq_index e7(rank_7, file_e);
constexpr sq_index e8(rank_8, file_e);

constexpr sq_index f1(rank_1, file_f);
constexpr sq_index f2(rank_2, file_f);
constexpr sq_index f3(rank_3, file_f);
constexpr sq_index f4(rank_4, file_f);
constexpr sq_index f5(rank_5, file_f);
constexpr sq_index f6(rank_6, file_f);
constexpr sq_index f7(rank_7, file_f);
constexpr sq_index f8(rank_8, file_f);

constexpr sq_index g1(rank_1, file_g);
constexpr sq_index g2(rank_2, file_g);
constexpr sq_index g3(rank_3, file_g);
constexpr sq_index g4(rank_4, file_g);
constexpr sq_index g5(rank_5, file_g);
constexpr sq_index g6(rank_6, file_g);
constexpr sq_index g7(rank_7, file_g);
constexpr sq_index g8(rank_8, file_g);

constexpr sq_index h1(rank_1, file_h);
constexpr sq_index h2(rank_2, file_h);
constexpr sq_index h3(rank_3, file_h);
constexpr sq_index h4(rank_4, file_h);
constexpr sq_index h5(rank_5, file_h);
constexpr sq_index h6(rank_6, file_h);
constexpr sq_index h7(rank_7, file_h);
constexpr sq_index h8(rank_8, file_h);

} /* anonym namespace */

} /* namespace kator::chess */
} /* namespace kator */

#endif /* !defined(KATOR_CHESS_SQ_INDEX_H) */
