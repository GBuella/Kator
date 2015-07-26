/* The bitboard class is a zero-overhead wrapper class
   around a uint64_t value. The advantage over using
   a uint64_t directly is in type safety, the author
   already had problems before originating from adding
   int values to uint64_t values.
   Another benefit of this approach, is in the begin() and
   end() methods, useful in range based for loops
   iterating over the set bits of the bitboard.
   Since uint64_t is a typedef for a fundamental type,
   C++11 doesn't lookup begin() and end() functions for it
   anywhere, this seems to be only way to use this feature.

   The technics, and naming conventions are derived from common
   chess programming literature,
   i.e. countless websites on the topic, and other chess engines,
   therefore they are not described here.

   The more complicated algorithms can be found in the book
   titled Hackers Delight as well, coupled with some number theory
   as explanation. Buy it, it contains a lot of beauty.
 */

#ifndef KATOR_CHESS_BITBOARD_H
#define KATOR_CHESS_BITBOARD_H

#include <cassert>
#include <array>
#include <cstddef>

#include "platform_low_level.h"

#include "chess.h"

namespace kator
{
namespace chess
{

class bitboard
{
  uint64_t value;

public:

  explicit constexpr bitboard(uint64_t arg): value(arg) {}
  bitboard() {}

private:

  static uint64_t reset_lsb_uint64_t(uint64_t arg);

  static constexpr uint64_t bit(sq_index index)
  {
    return UINT64_C(1) << index.offset();
  }

  static std::array<bitboard, 64> knight_table;
  static std::array<bitboard, 64> bishop_pattern_table;
  static std::array<bitboard, 64> rook_pattern_table;
  static std::array<bitboard, 64> king_table;
  static std::array<bitboard, 64*64> rays;

public:

  uint64_t pext(bitboard mask) const;
  static bitboard pdep(uint64_t value, bitboard mask);

#ifdef USE_PEXT_BITBOARD

  struct magical {
    uint64_t pre_mask;
    uint64_t post_mask;
    const uint16_t* attacks;

    size_t offset(uint64_t occupied) const
    {
      assert(bitboard(occupied & pre_mask).popcnt() <= 16);

      return bitboard(occupied).pext(bitboard(pre_mask));
    }

    bitboard pattern(uint64_t occupied) const
    {
      return pdep(attacks[offset(occupied)], bitboard(post_mask));
    }

    friend bitboard;

    static std::array<magical, 64> bishop;
    static std::array<magical, 64> rook;

    static void lookup_tables_init();
  };

#else // defined(USE_PEXT_BITBOARD)

  struct magical {
    uint64_t pre_mask;
    uint64_t multiplier;
    const bitboard *attacks;
    uint64_t shift;

    size_t offset(uint64_t occupied) const
    {
      uint64_t result = ((occupied & pre_mask) * multiplier) >> shift;
      return static_cast<size_t>(result);
    }

    bitboard pattern(uint64_t occupied) const
    {
      return attacks[offset(occupied)];
    }

    friend bitboard;

    static std::array<magical, 64> bishop;
    static std::array<magical, 64> rook;

    static void lookup_tables_init();
  };

#endif // !defined(USE_PEXT_BITBOARD)

public:

  constexpr uint64_t to_uint64_t() const;
  constexpr bitboard(file);
  constexpr bitboard(rank);
  static constexpr bitboard empty();
  static constexpr bitboard universe();
  static bitboard rook_pattern(sq_index from);
  static bitboard bishop_pattern(sq_index from);
  static bitboard knight_attacks(sq_index from);
  static bitboard king_attacks(sq_index from);
  static bitboard ray_between(sq_index, sq_index);
  static bitboard sliding_attacks(const std::array<magical, 64>&,
                                  bitboard occupied, sq_index);
  static bitboard bishop_attacks(bitboard occupied, sq_index);
  static bitboard rook_attacks(bitboard occupied, sq_index);
  static constexpr bitboard north_of(bitboard);
  static constexpr bitboard south_of(bitboard);
  static constexpr bitboard left_of(bitboard);
  static constexpr bitboard right_of(bitboard);
  static constexpr bitboard pawn_attacks_left(bitboard pawns);
  static constexpr bitboard pawn_attacks_right(bitboard pawns);
  static constexpr bitboard pawn_attacks(bitboard pawns);
  static constexpr bitboard opponent_pawn_attacks(bitboard pawns);
  constexpr bitboard(sq_index);
  template<typename... indices> constexpr bitboard(sq_index, indices...);
  constexpr bool operator == (const bitboard&) const;
  constexpr bool operator != (const bitboard&) const;
  constexpr bitboard operator & (const bitboard&) const;
  constexpr bitboard operator | (const bitboard&) const;
  constexpr bitboard operator ^ (const bitboard&) const;
  constexpr bitboard operator compl  () const;
  bitboard operator ^= (const bitboard&);
  bitboard operator &= (const bitboard&);
  bitboard operator |= (const bitboard&);
  void filter_by(bitboard);
  template<typename... bitboards> void filter_by(bitboard, bitboards...);
  void merge(bitboard);
  template<typename... bitboards> void merge(bitboard, bitboards...);
  int popcnt() const;
  constexpr bool is_empty() const;
  constexpr bool is_nonempty() const;
  bool is_singular() const;
  bool is_empty_or_singular() const;
  bitboard lsb() const;
  void reset_lsb();
  void reset_bit(sq_index);
  bool is_bit_set(sq_index) const;
  bool is_bit_non_set(sq_index) const;
  static void reset_bit_mem(bitboard*, sq_index);
  void set_bit(sq_index);
  static void set_bit_mem(bitboard*, sq_index);
  void complement_bit(sq_index);
  static void complement_bit_mem(bitboard*, sq_index);
  void flip();
  sq_index lsb_index() const;
  constexpr bool is_subset_of(bitboard) const;
  void generate_next_subset(bitboard& subset) const;

  class iterator
  {
  protected:

    uint64_t board;
    iterator(uint64_t arg): board(arg) {}

  public:

    iterator& operator++ ()
    {
      board = reset_lsb_uint64_t(board);
      return *this;
    }

    bool operator != (const iterator& other) const
    {
      return board != other.board;
    }

    sq_index operator* () const
    {
      return bitboard(board).lsb_index();
    }

    friend bitboard;
  };

  iterator begin() const
  {
    return iterator(value);
  }

  iterator end() const
  {
    return iterator(UINT64_C(0));
  }

  std::string to_str() const;


}; /* class bitboard */

constexpr uint64_t bitboard::to_uint64_t() const
{
  return value;
}

constexpr bitboard::bitboard(file file):
  value(UINT64_C(0x0101010101010101) << file.value) {}

constexpr bitboard::bitboard(rank rank):
  value(UINT64_C(0x00000000000000ff) << (rank.value * 8)) {}

constexpr bitboard bitboard::empty()
{
  return bitboard(UINT64_C(0));
}

constexpr bitboard bitboard::universe()
{
  return bitboard(UINT64_MAX);
}

inline bitboard bitboard::rook_pattern(sq_index from)
{
  ASSUME(from.is_valid());
  return bitboard(rook_pattern_table[from.offset()]);
}

inline bitboard bitboard::bishop_pattern(sq_index from)
{
  ASSUME(from.is_valid());
  return bitboard(bishop_pattern_table[from.offset()]);
}

inline bitboard bitboard::knight_attacks(sq_index from)
{
  ASSUME(from.is_valid());
  return knight_table[from.offset()];
}

inline bitboard bitboard::king_attacks(sq_index from)
{
  ASSUME(from.is_valid());
  return king_table[from.offset()];
}

inline bitboard bitboard::ray_between(sq_index a, sq_index b)
{
  ASSUME(a.is_valid() and b.is_valid());
  return bitboard(rays[a.offset() * 64 + b.offset()]);
}

inline bitboard
bitboard::sliding_attacks(const std::array<magical, 64>& magics,
                          bitboard occupied,
                          sq_index index)
{
  ASSUME(index.is_valid());
  return magics[index.offset()].pattern(occupied.value);
}

inline bitboard bitboard::bishop_attacks(bitboard occupied, sq_index index)
{
  ASSUME(index.is_valid());
  return magical::bishop[index.offset()].pattern(occupied.value);
}

inline bitboard bitboard::rook_attacks(bitboard occupied, sq_index index)
{
  ASSUME(index.is_valid());
  return magical::rook[index.offset()].pattern(occupied.value);
}

constexpr bitboard bitboard::north_of(bitboard arg)
{
  return bitboard(arg.value >> 8);
}

constexpr bitboard bitboard::south_of(bitboard arg)
{
  return bitboard(arg.value << 8);
}

constexpr bitboard bitboard::left_of(bitboard arg)
{
  return bitboard(arg.value << 1);
}

constexpr bitboard bitboard::right_of(bitboard arg)
{
  return bitboard(arg.value >> 1);
}

constexpr bitboard bitboard::pawn_attacks_left(bitboard pawns)
{
  return left_of(north_of(pawns) & compl bitboard(file_a));
}

constexpr bitboard bitboard::pawn_attacks_right(bitboard pawns)
{
  return right_of(north_of(pawns) & compl bitboard(file_h));
}

constexpr bitboard bitboard::pawn_attacks(bitboard pawns)
{
  return pawn_attacks_left(pawns) | pawn_attacks_right(pawns);
}

constexpr bitboard bitboard::opponent_pawn_attacks(bitboard pawns)
{
  return right_of(south_of(pawns) & compl bitboard(file_h)) |
          left_of(south_of(pawns) & compl bitboard(file_a));
}

constexpr bitboard::bitboard(sq_index index):
  value(bit(index)) {}

template<typename... indices>
constexpr bitboard::bitboard(sq_index index, indices... others):
  value(bit(index) | bitboard(others...).value) {}

constexpr bool bitboard::is_empty() const
{
  return value == UINT64_C(0);
}

constexpr bool bitboard::is_nonempty() const
{
  return value != UINT64_C(0);
}

constexpr bool bitboard::operator == (const bitboard& other) const
{
  return value == other.value;
}

constexpr bool bitboard::operator != (const bitboard& other) const
{
  return value != other.value;
}

constexpr bitboard bitboard::operator & (const bitboard& other) const
{
  return bitboard(value & other.value);
}

constexpr bitboard bitboard::operator | (const bitboard& other) const
{
  return bitboard(value | other.value);
}

constexpr bitboard bitboard::operator ^ (const bitboard& other) const
{
  return bitboard(value ^ other.value);
}

constexpr bitboard bitboard::operator compl  () const
{
  return bitboard(compl value);
}

inline bitboard bitboard::operator ^= (const bitboard& other)
{
  return bitboard(value ^= other.value);
}

inline bitboard bitboard::operator &= (const bitboard& other)
{
  return bitboard(value &= other.value);
}

inline bitboard bitboard::operator |= (const bitboard& other)
{
  return bitboard(value |= other.value);
}


inline bitboard bitboard::lsb() const
{
#ifdef SYSTEM_GET_LSB64

  return bitboard(SYSTEM_GET_LSB64(value));

#else

  return bitboard(value & (UINT64_C(0) - value));

#endif
}

inline uint64_t bitboard::reset_lsb_uint64_t(uint64_t arg)
{
#ifdef SYSTEM_RESET_LSB64

  return SYSTEM_RESET_LSB64(arg);

#else

  return arg & (arg - UINT64_C(1));

#endif
}

inline void bitboard::reset_lsb()
{
  value = reset_lsb_uint64_t(value);
}

inline int bitboard::popcnt() const
{
#ifdef SYSTEM_POPCNT64

  return SYSTEM_POPCNT64(value);

#else

  static constexpr uint64_t k1 = UINT64_C(0x5555555555555555);
  static constexpr uint64_t k2 = UINT64_C(0x3333333333333333);
  static constexpr uint64_t k4 = UINT64_C(0x0f0f0f0f0f0f0f0f);
  static constexpr uint64_t kf = UINT64_C(0x0101010101010101);

  uint64_t temp = value;

  temp =  temp       - ((temp >> 1)  & k1);
  temp = (temp & k2) + ((temp >> 2)  & k2);
  temp = (temp       +  (temp >> 4)) & k4 ;
  temp = (temp * kf) >> 56;
  return static_cast<int>(temp);

#endif
}

inline bool bitboard::is_singular() const
{
  return reset_lsb_uint64_t(value) == UINT64_C(0) and value != UINT64_C(0);
}

inline bool bitboard::is_empty_or_singular() const
{
  return reset_lsb_uint64_t(value) == UINT64_C(0);
}

inline bool bitboard::is_bit_set(sq_index index) const
{
  ASSUME(index.is_valid());
  return value & bit(index);
}

inline bool bitboard::is_bit_non_set(sq_index index) const
{
  ASSUME(index.is_valid());
  return value & bit(index);
}

inline void bitboard::reset_bit(sq_index index)
{
  ASSUME(index.is_valid());
  value &= compl bit(index);
}

inline void bitboard::reset_bit_mem(bitboard* board, sq_index index)
{
  ASSUME(index.is_valid());
  board->value &= compl bit(index);
}

inline void bitboard::set_bit(sq_index index)
{
  ASSUME(index.is_valid());
  value |= bit(index);
}

inline void bitboard::set_bit_mem(bitboard* board, sq_index index)
{
  ASSUME(index.is_valid());
  board->value |= bit(index);
}

inline void bitboard::complement_bit(sq_index index)
{
  ASSUME(index.is_valid());
  value ^= bit(index);
}

inline void bitboard::complement_bit_mem(bitboard* board, sq_index index)
{
  ASSUME(index.is_valid());
  board->value ^= bit(index);
}

inline void bitboard::flip()
{
#ifdef SYSTEM_BYTESWAP64

  value = SYSTEM_BYTESWAP64(value);

#else

	static constexpr uint64_t k1 = UINT64_C(0x00ff00ff00ff00ff);
	static constexpr uint64_t k2 = UINT64_C(0x0000ffff0000ffff);
	static constexpr uint64_t k3 = UINT64_C(0x00000000ffffffff);

	value = ((value >> 8) & k1) | ((value & k1) << 8);
	value = ((value >> 16) & k2) | ((value & k2) << 16);
	value = ((value >> 32) & k3) | ((value & k3) << 32);

#endif
}

static inline bitboard flip(const bitboard* value)
{
#ifdef USE_X86_MOVBE_GASM

  uint64_t temp;

  asm("movbe %1,%0" :
      "m" (*value) :
      "+r" (temp));
  return temp;

#else

  bitboard temp = *value;
  temp.flip();
  return temp;

#endif
}

inline sq_index bitboard::lsb_index() const
{
  ASSUME(is_nonempty());

#ifdef SYSTEM_CTZ64

  return SYSTEM_CTZ64(value);

#else

  static const char lsb_64_table[64] =
  {
    63, 30,  3, 32, 59, 14, 11, 33,
    60, 24, 50,  9, 55, 19, 21, 34,
    61, 29,  2, 53, 51, 23, 41, 18,
    56, 28,  1, 43, 46, 27,  0, 35,
    62, 31, 58,  4,  5, 49, 54,  6,
    15, 52, 12, 40,  7, 42, 45, 16,
    25, 57, 48, 13, 10, 39,  8, 44,
    20, 47, 38, 22, 17, 37, 36, 26
  };

  unsigned folded;

  folded =  static_cast<unsigned>(((value ^ (value-1)) >> 32));
  folded ^= static_cast<unsigned>( (value ^ (value-1)));
  return sq_index(lsb_64_table[folded * 0x78291ACF >> 26]);

#endif
}

constexpr bool bitboard::is_subset_of(bitboard other) const
{
  return (value & other.value) == value;
}

inline void bitboard::generate_next_subset(bitboard& subset) const
{
  subset.value = (subset.value - 1) & value;
}

inline void bitboard::filter_by(bitboard arg)
{
  value &= arg.value;
}

template<typename... bitboards>
inline void bitboard::filter_by(bitboard arg0, bitboards... others)
{
  filter_by(arg0);
  filter_by(others...);
}

inline void bitboard::merge(bitboard arg)
{
  value |= arg.value;
}

template<typename... bitboards>
inline void bitboard::merge(bitboard arg0, bitboards... others)
{
  merge(arg0);
  merge(others...);
}

namespace
{

constexpr bitboard edges =
    bitboard(file_a) | bitboard(file_h) |
    bitboard(rank_1) | bitboard(rank_8);

/*
    pre C++14 without binary literals, or digit separators

constexpr bitboard center(UINT64_C(0x0000001818000000));
constexpr bitboard black_squares(UINT64_C(0xaa55aa55aa55aa55));
constexpr bitboard white_squares(UINT64_C(0x55aa55aa55aa55aa));
*/

constexpr bitboard center(UINT64_C(
  0b00000000'00000000'00000000'00011000'00011000'00000000'00000000'00000000));
constexpr bitboard black_squares(UINT64_C(
  0b10101010'10101010'10101010'10101010'10101010'10101010'10101010'10101010));
constexpr bitboard white_squares(UINT64_C(
  0b01010101'01010101'01010101'01010101'01010101'01010101'01010101'01010101));

inline constexpr bool is_empty(bitboard arg)
{
  return arg.is_empty();
}

inline constexpr bool is_nonempty(bitboard arg)
{
  return arg.is_nonempty();
}

inline int cardinality_of(bitboard arg)
{
  return arg.popcnt();
}

inline bool is_singular(bitboard arg)
{
  return arg.is_singular();
}

inline bool is_empty_or_singular(bitboard arg)
{
  return arg.is_empty_or_singular();
}

inline constexpr bitboard complement_of(bitboard arg)
{
  return compl arg;
}

inline constexpr bitboard intersection_of(bitboard arg)
{
  return arg;
}

template<typename... bitboards>
inline bitboard intersection_of(bitboard arg0, bitboards... others)
{
  return arg0 & intersection_of(others...);
}

inline bitboard union_of(bitboard arg)
{
  return arg;
}

template<typename... bitboards>
inline bitboard union_of(bitboard arg0, bitboards... others)
{
  return arg0 | union_of(others...);
}

template<typename... bitboards>
inline bool are_disjoint(bitboards... args)
{
  return is_empty(intersection_of(args...));
}

typedef std::array<bitboard::magical, 64> magic_array;

inline constexpr bitboard north_of(bitboard arg)
{
  return bitboard::north_of(arg);
}

inline constexpr bitboard south_of(bitboard arg)
{
  return bitboard::south_of(arg);
}

inline constexpr bitboard left_of(bitboard arg)
{
  return bitboard::left_of(arg);
}

inline constexpr bitboard right_of(bitboard arg)
{
  return bitboard::right_of(arg);
}

}

inline uint64_t bitboard::pext(bitboard mask) const
{
#ifdef HAS_BMI2_PEXT_BITBOARD_SUPPORT
  return _pext_u64(value, mask.value);
#else

  uint64_t result = UINT64_C(0);
  uint64_t dst_bit = UINT64_C(1);

  for (auto index : mask) {
    if (is_bit_set(index)) {
      result |= dst_bit;
    }
    dst_bit <<= 1;
  }
  return result;

#endif
}

inline bitboard bitboard::pdep(uint64_t value, bitboard mask)
{
#ifdef HAS_BMI2_PEXT_BITBOARD_SUPPORT
  return bitboard(_pdep_u64(value, mask.value));
#else

  bitboard result(UINT64_C(0));
  uint64_t src_bit = UINT64_C(1);

  for (auto index : mask) {
    if ((value & src_bit) != UINT64_C(0)) {
      result.set_bit(index);
    }
    src_bit <<= 1;
  }
  return result;

#endif
}

} /* namespace kator::chess */
} /* namespace kator */

#endif /* !defined(KATOR_CHESS_BITBOARD_H) */
