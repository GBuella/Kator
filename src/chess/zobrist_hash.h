/* The zobrist_hash class is a straightforward wrapper around a 64 bit hash {{{
   value.  Calling zobrist_hash::xor_piece with a nonpiece
   is a no-op, the value will be xor'ed with zero.
   Therefore it is ok to use a piece on a board anywhere, 
   and call xor_piece with it without checking for empty squares.
}}}*/

#ifndef KATOR_CHESS_ZOBRIST_HASH_H
#define KATOR_CHESS_ZOBRIST_HASH_H

#include <cstdint>
#include <memory>

#include "chess.h"
#include "castle_rights.h"

namespace kator
{

class zobrist_hash
{
public:

  constexpr explicit zobrist_hash(uint64_t);
  void xor_piece(square, sq_index);
  void xor_castle_right(castle_rights::side);
  void xor_en_passant_file(file);
  constexpr uint64_t get_value() const;
  void clear();

private:
  uint64_t value;
  static const uint64_t z_random[piece_array_size + 1][64];

}; /* class zobrist_hash */

class zobrist_hash_pair
{
public:
  void xor_piece(square, sq_index);
  void xor_castle_right(castle_rights::side);
  void xor_en_passant_file(file);
  operator zobrist_hash() const;
  zobrist_hash_pair flipped() const;

  static zobrist_hash_pair initial();

private:
  zobrist_hash_pair();
  zobrist_hash hash;
  zobrist_hash opponent_hash;

}; /* class zobrist_hash_pair */


inline constexpr zobrist_hash::zobrist_hash(uint64_t raw):
  value(raw)
{
}

inline void zobrist_hash::xor_piece(square piece, sq_index index)
{
  value ^= z_random[piece][index.offset()];
}

inline constexpr uint64_t zobrist_hash::get_value() const
{
  return value;
}

inline void zobrist_hash::clear()
{
  value = 0;
}

inline void zobrist_hash::xor_castle_right(castle_rights::side side)
{
  value ^= z_random[piece_array_size][static_cast<unsigned>(side)];
}

inline void zobrist_hash::xor_en_passant_file(file file)
{
  value ^= z_random[piece_array_size][4 + file.offset()];
}

inline void zobrist_hash_pair::xor_piece(square piece, sq_index index)
{
  hash.xor_piece(piece, index);
  opponent_hash.xor_piece(opponent_of(piece), flip(index));
}

inline void zobrist_hash_pair::xor_castle_right(castle_rights::side side)
{
  hash.xor_castle_right(side);
  opponent_hash.xor_castle_right(opponent_of(side));
}

inline zobrist_hash_pair::operator zobrist_hash() const
{
  return hash;
}

inline zobrist_hash_pair::zobrist_hash_pair():
  hash(UINT64_C(0)),
  opponent_hash(UINT64_C(0))
{
}

inline zobrist_hash_pair zobrist_hash_pair::initial()
{
  return zobrist_hash_pair();
}

inline zobrist_hash_pair zobrist_hash_pair::flipped() const
{
  zobrist_hash_pair result;

  result.hash = opponent_hash;
  result.opponent_hash = hash;
  return result;
}

} /* namespace kator */

#endif /* !defined(KATOR_CHESS_ZOBRIST_HASH_H) */
