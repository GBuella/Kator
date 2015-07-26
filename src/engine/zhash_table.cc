
#include "zhash_table.h"
#include "chess/position.h"

using ::std::atomic;

namespace kator
{
namespace engine
{

size_t zhash_table::entry_count() const noexcept
{
  return static_cast<size_t>(mask + 1);
}

size_t zhash_table::size() const noexcept
{
  return entry_count() * sizeof(atomic<uint64_t>);
}

zhash_table::zhash_table():
  mask(0)
{
}

zhash_table::zhash_table(unsigned log2_size)
{
  if (log2_size < 1) {
    log2_size = 1;
  }
  if (log2_size > max_log2_size) {
    log2_size = max_log2_size;
  }
  mask = (UINT64_C(1) << log2_size) - 1;
  auto pointer = new atomic<uint64_t>[entry_count() + 1];
  memset(pointer, 0, size());
  data.reset(pointer);
}

hash_entry zhash_table::load_entry(chess::zobrist_hash key) const noexcept
{
  if (not is_set()) {
    return empty_hash_entry;
  }
  auto address = address_of(key);
  hash_entry entry(address->load(std::memory_order_relaxed));

  if (entry.match(key)) {
    return entry;
  }
  if (entry.is_empty()) {
    return empty_hash_entry;
  }
  entry = hash_entry(address[1]);
  if (entry.match(key)) {
    return entry;
  }
  else {
    return empty_hash_entry;
  }
}

hash_entry
zhash_table::load_entry(const chess::position& position) const noexcept
{
  return load_entry(position.get_zhash());
}

void
zhash_table::store_entry(chess::zobrist_hash key, hash_entry entry) noexcept
{
  if (not is_set()) {
    return;
  }

  entry.set_hash_upper(key);
  auto address = address_of(key);
  hash_entry existing(address->load(std::memory_order_relaxed));
  if (existing.match(key)) {
    address->store(entry.as_uint64(), std::memory_order_relaxed);
  }
  else {
    address[1].store(entry.as_uint64(), std::memory_order_relaxed);
  }
}

void zhash_table::store_entry(const chess::position& position,
                              hash_entry entry) noexcept
{
  store_entry(position.get_zhash(), entry);
}


} /* namespace engine */
} /* namespace kator */
