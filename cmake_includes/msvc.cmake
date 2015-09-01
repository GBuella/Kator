
CHECK_CXX_SOURCE_RUNS("
#include <cstdint>
#include <intrin.h>

uint64_t countit(uint64_t x) { return __popcnt64(x); }

int main() {
  uint64_t x = countit(0x404040);
  return x & 0;
}"
  KATOR_HAS_MSVCPP_POPCOUNT_64)

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>
#include <intrin.h>

uint64_t bsf(uint64_t x) { return _BitScanForward64(x); }

int main() {
  uint64_t x = bsf(0x404040);
  return x & 0;
}"
  KATOR_HAS_MSVCPP_BITSCANFORWARD64)

CHECK_CXX_SOURCE_RUNS("
#include <intrin.h>
#include <cstdint>
uint64_t swapit(uint64_t x) { return _byteswap_uint64(x); }
int main() {
uint64_t x = swapit(0x404040);
return x & 0;
}"
  KATOR_HAS_MSVCPP_BYTESWAP_UINT64)

