
#ifndef KATOR_CHESS_COORDINATE_H
#define KATOR_CHESS_COORDINATE_H

namespace kator
{
namespace chess
{

template<int max_value>
class coordinate
{
protected:

  unsigned char value;
  coordinate() {}

public:

  constexpr unsigned char offset() const
  {
    return value;
  }

  constexpr bool is_valid() const
  {
    return value >= 0 and value <= max_value;
  }

  constexpr bool operator == (const coordinate& other) const
  {
    return value == other.value;
  }

  constexpr bool operator != (const coordinate& other) const
  {
    return value != other.value;
  }

protected:

  constexpr coordinate(int arg):
    value(static_cast<unsigned char>(arg)) {}
  constexpr coordinate(unsigned char arg): value(arg) {}

  template <int first, int last, int delta, typename T>
  class coordinate_range
  {
  public:

    class iterator
    {
      int value;

      constexpr iterator(int start): value(start) {}

      public:

      bool operator != (const iterator& other) const
      {
        return other.value != value;
      }

      bool operator == (const iterator& other) const
      {
        return other.value == value;
      }

      T operator* () const
      {
        return T(value);
      }

      const iterator& operator++ ()
      {
        value += delta;
        return *this;
      }

      friend class coordinate_range;
    };

    constexpr coordinate_range() {}

    constexpr iterator begin() const
    {
      return iterator(first);
    }

    constexpr iterator end() const
    {
      return iterator(last + delta);
    }

  }; /* class coordinate_range */

}; /* template coordinate */

} /* namespace kator::chess */
} /* namespace kator */

#endif /* !defined(KATOR_CHESS_COORDINATE_H) */
