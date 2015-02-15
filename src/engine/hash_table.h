/* A simple lock-free hash_table storing 64 bit entries. {{{
   On modern commodity hardware read|write accesses
   to 64 bit alogned uint64_t values are atomic, so
   std::atomic<uint64_t> is expected to mean zero-overhead
   compared to using the uint64_t type. In the case of
   compiling for targets where this is not the case, it either
   uses plain uint64_t, meaning the engine can run on only one thread,
   or std::atomic<uint64_t> uses a lock.
   The constructor and the resize method shall be called with
   the base two logarithm of the requested size in bytes, e.g.:
     hash_table(10) <- 1 kilobyte, 128 entries
     hash_table(22) <- 4 megabytes, 2^19 entries

}}}*/

#ifndef KATOR_ENGINE_ZOBRIST_HASH_H
#define KATOR_ENGINE_ZOBRIST_HASH_H

#include <cstddef>
#include <array>
#include <memory>

#include "config.h"
#include "chess/zobrist_hash.h"
#include "eval.h"

#ifdef KATOR_ENGINE_MULTITHREADED
#include <atomic>
#endif

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
public:

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

  private: uint64_t value_adjusted    : position_value::bits;
  public:  uint64_t value_type        : value_type_bits;
  public:  uint64_t depth             : depth_bits;
  public:  uint64_t move_index        : move_index_bits;
  private: uint64_t hash_upper        : hash_upper_bits;

public:


  position_value value() const noexcept;
  void set_value(position_value) noexcept;
  bool match(chess::zobrist_hash) const noexcept;
  void set_zobrist_hash(chess::zobrist_hash) noexcept;

};

static_assert(sizeof(hash_entry) == sizeof(uint64_t), //{{{
              "hash_entry struct has invalid size");
/* "If enough space remains, a bit-field that immediately
   follows another bit-field in a structure shall be packed
   into adjacent bits of the same unit."
   }}}*/

class hash_table
{
public:

  static constexpr unsigned max_log2_size = 30;

  hash_table();
  hash_table(unsigned log2_size);
  void resize(unsigned log2_size);
  unsigned log2_size() const noexcept;
  size_t size() const noexcept;
  size_t entry_count() const noexcept;

  hash_entry load_entry(chess::zobrist_hash) const noexcept;
  hash_entry load_entry(const chess::position&) const noexcept;
  void store_entry(chess::zobrist_hash, hash_entry) noexcept;
  void store_entry(const chess::position&, hash_entry) noexcept;

  void prepare_load(chess::zobrist_hash hash, int locality) const noexcept;
  void prepare_write(chess::zobrist_hash hash, int locality) const noexcept;

private:

  uint64_t mask;

#ifdef KATOR_ENGINE_MULTITHREADED
  typedef std::atomic<uint64_t> slot_t;

/* The code still expects each slot to be a raw 64 bit value. {{{
   Using std::atomic is only a pedantic extra guarantee,
   so the compiler surely will not do two 32 bit operations
   instead of an atomic 64 bit operation.
*/
  static_assert(sizeof(slot_t) == sizeof(uint64_t),
                "Non-supported data representation");
  //}}}
#else
  typedef uint64_t slot_t;
#endif

  std::unique_ptr<slot_t[]> data = nullptr;

  /* Another possible implementation
  static void slot_array_deleter(slot_t*);

  std::shared_ptr<slot_t> data
    = std::shared_ptr<slot_t>(nullptr, slot_array_deleter);
  */

  slot_t* address_of(chess::zobrist_hash) const noexcept;

}; /* class hash_table */


/*
void hash_table::slot_array_deleter(slot_t* slots)
{
  delete[] slots;
}
*/

inline hash_table::slot_t*
hash_table::address_of(chess::zobrist_hash hash) const noexcept
{
  return data.get() + static_cast<uintptr_t>(hash.get_value() & mask);
}

inline void
hash_table::prepare_load(chess::zobrist_hash hash, int locality) const noexcept
{
  switch (locality) {
    case 0:
      platform_prefetch_for_read_0(address_of(hash));
      break;
    case 1:
      platform_prefetch_for_read_1(address_of(hash));
      break;
    case 2:
      platform_prefetch_for_read_2(address_of(hash));
      break;
    case 3:
      platform_prefetch_for_read_3(address_of(hash));
      break;
  }
}

inline void
hash_table::prepare_write(chess::zobrist_hash hash, int locality) const noexcept
{
  switch (locality) {
    case 0:
      platform_prefetch_for_write_0(address_of(hash));
      break;
    case 1:
      platform_prefetch_for_write_1(address_of(hash));
      break;
    case 2:
      platform_prefetch_for_write_2(address_of(hash));
      break;
    case 3:
      platform_prefetch_for_write_3(address_of(hash));
      break;
  }
}

inline position_value hash_entry::value() const noexcept
{
  return position_value::create_from_adjusted_uint(value_adjusted);
}

inline void hash_entry::set_value(position_value value) noexcept
{
  value_adjusted = value.to_adjusted_uint();
}

inline bool hash_entry::match(chess::zobrist_hash hash) const noexcept
{
  return hash_upper == (hash.get_value() >> (64 - hash_upper_bits));
}

inline void hash_entry::set_zobrist_hash(chess::zobrist_hash hash) noexcept
{
  hash_upper = (hash.get_value() >> (64 - hash_upper_bits));
}

} /* namespace engine */
} /* namespace kator */

#endif /* !defined(KATOR_ENGINE_ZOBRIST_HASH_H) */
