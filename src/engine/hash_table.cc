
#include "hash_table.h"

namespace kator
{
namespace engine
{

size_t hash_table::entry_count() const noexcept
{
  return static_cast<size_t>(mask + 1);
}

size_t hash_table::size() const noexcept
{
  return entry_count() * sizeof(slot_t);
}

hash_table::hash_table():
  mask(0)
{
}

hash_table::hash_table(unsigned log2_size)
{
  if (log2_size < 1) {
    log2_size = 1;
  }
  if (log2_size > max_log2_size) {
    log2_size = max_log2_size;
  }
  mask = (UINT64_C(1) << log2_size) - 1;
  auto pointer = new slot_t[entry_count()];
  memset(pointer, 0, size());
  data.reset(pointer);
}

} /* namespace engine */
} /* namespace kator */
