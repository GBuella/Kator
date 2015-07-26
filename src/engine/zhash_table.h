/* A simple lock-free hash table storing 64 bit entries. {{{
   On modern commodity hardware read|write accesses
   to 64 bit alogned uint64_t values are atomic, so
   std::atomic<uint64_t> is expected to mean zero-overhead
   compared to using the uint64_t type. In the case of
   compiling for targets where this is not the case, it either
   uses plain uint64_t, meaning the engine can run on only one thread,
   or std::atomic<uint64_t> uses a lock.
   The constructor method shall be called with
   the base two logarithm of the requested size in bytes, e.g.:
     zhash_table(10) <- 1 kilobyte, 128 entries
     zhash_table(22) <- 4 megabytes, 2^19 entries

}}}*/

#ifndef KATOR_ENGINE_ZHASH_TABLE_H
#define KATOR_ENGINE_ZHASH_TABLE_H

#include <cstddef>
#include <atomic>
#include <array>
#include <memory>

#include "config.h"
#include "chess/zobrist_hash.h"
#include "eval.h"


namespace kator
{
namespace engine
{

enum hash_value_type
{
  vt_none = 0,
  vt_lower_bound = 1,
  vt_upper_bound = 2,
  vt_exact = 3
};

class hash_entry
{

  static constexpr unsigned value_type_bits = 2;
  static constexpr unsigned depth_bits = 7;
  static constexpr unsigned max_depth = (1 << depth_bits) - 1;
  static constexpr unsigned move_index_bits = 8;
  static constexpr unsigned max_move_index = (1 << move_index_bits) - 1;
  static constexpr unsigned hash_upper_bits = 64
                                              - value_type_bits
                                              - depth_bits
                                              - move_index_bits
                                              - position_value::bits;

  /*
  private: uint64_t value_adjusted    : position_value::bits;
  public:  uint64_t value_type        : value_type_bits;
  public:  uint64_t depth             : depth_bits;
  public:  uint64_t move_index        : move_index_bits;
  private: uint64_t hash_upper        : hash_upper_bits;
  */

  static constexpr unsigned value_type_start = position_value::bits;
  static constexpr unsigned depth_start = value_type_start + value_type_bits;
  static constexpr unsigned move_index_start = depth_start + depth_bits;
  static constexpr uint64_t hash_upper_mask =
    compl ((UINT64_C(1) << (64 - hash_upper_bits)) - UINT64_C(1));

  uint64_t internal;

  constexpr unsigned get_uint(unsigned from_bit, unsigned bit_count) const
  {
    return (internal >> from_bit) & ((1 << bit_count) - 1);
  }

  explicit constexpr hash_entry(uint64_t value): internal(value) {}

  constexpr uint64_t as_uint64() const
  {
    return internal;
  }

  void set_hash_upper(chess::zobrist_hash key)
  {
    internal |= key.get_value() & hash_upper_mask;
  }

public:

  constexpr position_value value() const
  {
    return position_value::create_from_adjusted_uint(
                                internal & ((1 << position_value::bits) - 1));
  }

  void set_value(position_value value)
  {
    internal |= value.to_adjusted_uint();
  }

  constexpr bool match(chess::zobrist_hash key) const
  {
    return ((key.get_value() ^ internal) & hash_upper_mask) == UINT64_C(0);
  }

  void set_zobrist_hash(chess::zobrist_hash key)
  {
    internal |= key.get_value() & hash_upper_mask;
  }

  constexpr bool is_empty() const
  {
    return internal == UINT64_C(0);
  }

  constexpr unsigned depth() const
  {
    return get_uint(depth_start, depth_bits);
  }

  void set_depth(unsigned value)
  {
    internal |= value << depth_start;
  }

  constexpr hash_value_type value_type() const
  {
    return hash_value_type(get_uint(value_type_start, value_type_bits));
  }

  void set_value_type(hash_value_type value)
  {
    internal |= value << value_type_start;
  }

  constexpr unsigned move_index() const
  {
    return get_uint(move_index_start, move_index_bits);
  }

  void set_move_index(unsigned value)
  {
    internal |= value << move_index_start;
  }

  static constexpr hash_entry empty()
  {
    return hash_entry(UINT64_C(0));
  }

  friend class zhash_table;

};

constexpr hash_entry empty_hash_entry = hash_entry::empty();

static_assert(sizeof(hash_entry) == sizeof(uint64_t), //{{{
              "hash_entry struct has invalid size");
/* "If enough space remains, a bit-field that immediately
   follows another bit-field in a structure shall be packed
   into adjacent bits of the same unit."
   }}}*/

class zhash_table
{
public:

  static constexpr unsigned max_log2_size = 30;

  zhash_table();
  zhash_table(unsigned log2_size);
  unsigned log2_size() const noexcept;
  size_t size() const noexcept;
  size_t entry_count() const noexcept;

  hash_entry load_entry(chess::zobrist_hash) const noexcept;
  hash_entry load_entry(const chess::position&) const noexcept;
  void store_entry(chess::zobrist_hash, hash_entry) noexcept;
  void store_entry(const chess::position&, hash_entry) noexcept;

  void prepare_load(chess::zobrist_hash, int locality) const noexcept;
  void prepare_write(chess::zobrist_hash, int locality) const noexcept;

private:

  uint64_t mask;

  static void slot_array_deleter(std::atomic<uint64_t>*);

  std::shared_ptr<std::atomic<uint64_t>> data
    = std::shared_ptr<std::atomic<uint64_t>>(nullptr, slot_array_deleter);

  std::atomic<uint64_t>* address_of(chess::zobrist_hash) const noexcept;

public:

  bool is_set() const noexcept
  {
    return (mask != 0);
  }

}; /* class zhash_table */

inline void zhash_table::slot_array_deleter(std::atomic<uint64_t>* slots)
{
  delete[] slots;
}

inline std::atomic<uint64_t>*
zhash_table::address_of(chess::zobrist_hash key) const noexcept
{
  return data.get() + static_cast<uintptr_t>(key.get_value() & mask);
}

inline void
zhash_table::prepare_load(chess::zobrist_hash key, int locality) const noexcept
{
  switch (locality) {
    case 0:
      platform_prefetch_for_read_0(address_of(key));
      break;
    case 1:
      platform_prefetch_for_read_1(address_of(key));
      break;
    case 2:
      platform_prefetch_for_read_2(address_of(key));
      break;
    case 3:
      platform_prefetch_for_read_3(address_of(key));
      break;
  }
}

inline void
zhash_table::prepare_write(chess::zobrist_hash key, int locality) const noexcept
{
  switch (locality) {
    case 0:
      platform_prefetch_for_write_0(address_of(key));
      break;
    case 1:
      platform_prefetch_for_write_1(address_of(key));
      break;
    case 2:
      platform_prefetch_for_write_2(address_of(key));
      break;
    case 3:
      platform_prefetch_for_write_3(address_of(key));
      break;
  }
}

} /* namespace engine */
} /* namespace kator */

#endif /* !defined(KATOR_ENGINE_ZHASH_TABLE_H) */
