
#ifndef KATOR_CHESS_FILE_H
#define KATOR_CHESS_FILE_H

#include "coordinate.h"

#include <string>

namespace kator
{

class file: public coordinate<7>
{
  constexpr file(unsigned arg):
    coordinate<7>(arg) {}

public:

  friend class sq_index;
  friend class bitboard;

  std::string to_str() const;
  explicit file(char);
  static bool is_valid_char(char);

  static constexpr file leftmost()
  {
    return file(unsigned(7));
  }

  static constexpr file rigthmost()
  {
    return file(unsigned(0));
  }

  void shift_left()
  {
    --value;
  }

  void shift_right()
  {
    ++value;
  }

  constexpr file east_shifted() const
  {
    return file(value - 1);
  }

  constexpr file west_shifted() const
  {
    return file(value + 1);
  }

  void shift_east(int delta = 1)
  {
    value -= delta;
  }

  void shift_west(int delta = 1)
  {
    value += delta;
  }

  class range_a_to_h: public coordinate_range<7, 0, -1, file>
  {
    public: constexpr range_a_to_h() {}
  };

  class range_h_to_a: public coordinate_range<0, 7, +1, file>
  {
    public: constexpr range_h_to_a() {}
  };

  friend range_a_to_h::iterator;
  friend range_h_to_a::iterator;

}; /* class file */

namespace
{

inline constexpr file east_of(file arg)
{
  return arg.east_shifted();
}

inline constexpr file west_of(file arg)
{
  return arg.west_shifted();
}

constexpr file::range_a_to_h files_a_to_h;
constexpr file::range_h_to_a files_h_to_a;

constexpr file file_a = file::leftmost();
constexpr file file_b = east_of(file_a);
constexpr file file_c = east_of(file_b);
constexpr file file_d = east_of(file_c);
constexpr file file_e = east_of(file_d);
constexpr file file_f = east_of(file_e);
constexpr file file_g = east_of(file_f);
constexpr file file_h = east_of(file_g);

} /* anonym namespace */

} /* namespace kator */

#endif /* !defined(KATOR_CHESS_FILE_H) */
