/*  Another integer wrapper class: position_value {{{
    Storing a static evaluation value of a position,
    inline functions for updateing trivial material value
    during make_move.
    The rest of the static evaluation happens in the
    eval function, either using an already computed
    material value ( intended for use during search ),
    or from scratch ( intended for use in business logic )
}}}*/

#ifndef KATOR_ENGINE_EVAL_H
#define KATOR_ENGINE_EVAL_H

#include <array>
#include <string>
#include <ostream>
#include <istream>
#include <string>

#include "chess/chess.h"
#include "chess/move.h"

namespace kator
{
namespace engine
{

class position_value
{
  int internal;

public:

  static constexpr unsigned value_bits = 9;
  static constexpr unsigned bits = value_bits + 1;

private:

  constexpr position_value(int value): internal(value) {}

  static std::array<short, chess::piece::array_size> piece_values;
  static chess::move::change_array_t<short> move_change_table;

  static constexpr int max = (1 << value_bits) - 1;

public:

  position_value(const chess::piece& piece):
    internal(piece_values[piece.index()])
  {
  }

  static constexpr position_value create_from_int(int value)
  {
    return position_value(value);
  }

  static constexpr position_value create_from_adjusted_uint(unsigned value)
  {
    return static_cast<unsigned>(value - max);
  }

  constexpr unsigned to_adjusted_uint() const noexcept
  {
    return static_cast<unsigned>(internal + max);
  }

  constexpr int as_int() const
  {
    return internal;
  }

  constexpr short as_short() const
  {
    return short(internal);
  }

  float as_float() const
  {
    return float(internal) / piece_values[chess::pawn.index()];
  }

  int sgn() const
  {
    return (internal < 0) ? -1 : ((internal > 0) ? +1 : 0);
  }

  position_value operator+= (const position_value& value)
  {
    internal += value.internal;
    return *this;
  }

  position_value operator-= (const position_value& value)
  {
    internal -= value.internal;
    return *this;
  }

  constexpr position_value operator+ (const position_value& value) const
  {
    return position_value(internal + value.internal);
  }

  constexpr position_value operator- (const position_value& value) const
  {
    return position_value(internal - value.internal);
  }

  position_value operator+= (const chess::piece& piece)
  {
    internal += piece_values[piece.index()];
    return *this;
  }

  position_value operator-= (const chess::piece& piece)
  {
    internal -= piece_values[piece.index()];
    return *this;
  }

  constexpr position_value operator+ (const chess::piece& piece) const
  {
    return *this + piece;
  }

  constexpr position_value operator- (const chess::piece& piece) const
  {
    return *this - piece;
  }

  void flip()
  {
    internal = -internal;
  }

  constexpr position_value flipped() const
  {
    return position_value(-internal);
  }

  position_value(const chess::position&);

  void update_by_move(chess::move move)
  {
    internal += move_change_table[move.change_index()];
  }

  /* Setting up all the constants used during evaluation.
     Not thread-safe, but good enough for Kator.
   */
  static void default_initialize_lookup_tables();
  static void initialize_lookup_tables(const std::string& path);
  static void initialize_lookup_tables(std::istream&);

}; // class position_value

} // namespace kator::engine
} // namespace kator

#endif // !defined(KATOR_ENGINE_EVAL_H)
