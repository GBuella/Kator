
set(orig_cmake_required_flags "${CMAKE_REQUIRED_FLAGS}")
set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${STANDARD_FLAG}")

if(NOT MSVC)

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>
uint64_t swapit(uint64_t x) { return __builtin_bswap64(x); }
int main() {
  uint64_t x = swapit(0x404040);
  return x & 0;
}"
  HAS_GCC_BUILTIN_BSWAP64)

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>
#include <cassert>
#include <type_traits>

uint64_t countit(uint64_t x) { return __builtin_popcountll(x); }

int main() {
  bool same = std::is_same<unsigned long long, std::uint64_t>::value;
  assert(same);
  uint64_t x = countit(0x404040);
  return x & 0;
}"
  HAS_GCC_BUILTIN_POPCNTLL_UINT64)

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>
#include <cassert>
#include <type_traits>

uint64_t countit(uint64_t x) { return __builtin_popcountl(x); }

int main() {
  bool same = std::is_same<unsigned long, std::uint64_t>::value;
  assert(same);
  uint64_t x = countit(0x404040);
  return x & 0;
}"
  HAS_GCC_BUILTIN_POPCNTL_UINT64)

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>
#include <cassert>
#include <type_traits>

uint64_t countit(uint64_t x) { return __builtin_popcount(x); }

int main() {
  bool same = std::is_same<unsigned, std::uint64_t>::value;
  assert(same);
  uint64_t x = countit(0x404040);
  return x & 0;
}"
  HAS_GCC_BUILTIN_POPCNT_UINT64)

endif()

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>
#include <intrin.h>

uint64_t countit(uint64_t x) { return __popcnt64(x); }

int main() {
  uint64_t x = countit(0x404040);
  return x & 0;
}"
  HAS_MSVCPP_POPCOUNT_64)

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>
#include <intrin.h>

uint64_t bsf(uint64_t x) { return _BitScanForward64(x); }

int main() {
  uint64_t x = bsf(0x404040);
  return x & 0;
}"
  HAS_MSVCPP_BITSCANFORWARD64)

if(NOT MSVC)

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>
#include <cassert>
#include <type_traits>

uint64_t ctz(uint64_t x) { return __builtin_ctzll(x); }

int main() {
  bool same = std::is_same<unsigned long long, std::uint64_t>::value;
  assert(same);
  uint64_t x = ctz(0x404040);
  return x & 0;
}"
  HAS_GCC_BUILTIN_CTZLL_UINT64)

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>
#include <cassert>
#include <type_traits>

uint64_t ctz(uint64_t x) { return __builtin_ctzl(x); }

int main() {
  bool same = std::is_same<unsigned long, std::uint64_t>::value;
  assert(same);
  uint64_t x = ctz(0x404040);
  return x & 0;
}"
  HAS_GCC_BUILTIN_CTZL_UINT64)

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>
#include <cassert>
#include <type_traits>

uint64_t ctz(uint64_t x) { return __builtin_ctz(x); }

int main() {
  bool same = std::is_same<unsigned, std::uint64_t>::value;
  assert(same);
  uint64_t x = ctz(0x404040);
  return x & 0;
}"
  HAS_GCC_BUILTIN_CTZ_UINT64)

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>

uint64_t blsi(uint64_t x) { return _blsi_u64(x); }
uint64_t blsr(uint64_t x) { return _blsr_u64(x); }

int main() {
  uint64_t x = blsi(0x404040);
  uint64_t y = blsr(0x808080);
  return x & y &  0;
}"
  HAS_GCC_BMI_INTRINSICS)

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>
 
typedef uint64_t vector64 __attribute__ ((vector_size(64)));

vector64 test = {9, 32, 4839834, 1024};

int main() {
  uint64_t x;
  unsigned char* c = (unsigned char*)(&test);


  c[12] = 99;
  x = test[1];
  return x & 0;
}"
    HAS_GCC_VECTOR)

endif()

option(USE_ALIGNAS_64
    "Use 64 byte alignment for the position representation" OFF)

option(USE_BOARD_VECTOR_64
    "Use 64 byte vectors to represent the board" OFF)

option(USE_PIECE_MAP_VECTOR
    "Use vector operations with bitboard piece maps" OFF)

CHECK_CXX_SOURCE_RUNS("
#include <intrin.h>
#include <cstdint>
uint64_t swapit(uint64_t x) { return _byteswap_uint64(x); }
int main() {
uint64_t x = swapit(0x404040);
return x & 0;
}"
  HAS_MSVCPP_BYTESWAP_UINT64)
 

CHECK_CXX_SOURCE_RUNS("
#undef __STRICT_ANSI__
#include <cstdio>
#include <unistd.h>
int main() {
if (isatty(fileno(stdin))) {
  (void)setvbuf(stdout, NULL, _IOLBF, 0x1000);
}
return 0;
}"
  CAN_DO_SETVBUF)

if(NOT MSVC)
 
CHECK_CXX_SOURCE_RUNS("
int main() {
  int x[1024];
  x[5] = 9;
  __builtin_prefetch(x + 5, 0, 2);
  return x[5] & 0;
}"
  HAS_GCC_BUILTIN_PREFETCH)

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>
#include <x86intrin.h>

typedef uint8_t vector_8_16 __attribute__ ((vector_size(16)));

static const vector_8_16 key16 = { 7,  6,  5,  4,  3,  2,  1,  0,
                                  15, 14, 13, 12, 11, 10,  9,  8};

int main() {
  vector_8_16 a = key16;
  vector_8_16 b, c;

  b = reinterpret_cast<vector_8_16>(_mm_permute_pd(
          reinterpret_cast<double __attribute__ ((vector_size(16)))>(a), 1));
  c = _mm_xor_si128(b, a);
  c = _mm_cmpeq_epi8(b, a);
  c = _mm_andnot_si128(c, a);
  c = _mm_shuffle_epi8(c, key16);
  return c[2] & 0;
}"
  HAS_INTEL_128BIT_BUILTINS)

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>
#include <x86intrin.h>

typedef uint8_t vector_8_32 __attribute__ ((vector_size(32)));

static const vector_8_32 key32 = { 7,  6,  5,  4,  3,  2,  1,  0,
                                  15, 14, 13, 12, 11, 10,  9,  8,
                                   7,  6,  5,  4,  3,  2,  1,  0,
                                  15, 14, 13, 12, 11, 10,  9,  8};

int main() {
  vector_8_32 a = key32;
  vector_8_32 b, c;

  b = _mm256_permute4x64_epi64(a, 17);
  c = _mm256_xor_si256(b, a);
  c = _mm256_cmpeq_epi8(b, a);
  c = _mm256_andnot_si256(c, a);
  c = _mm256_shuffle_epi8(c, key32);
  return c[2] & 0;
}"
  HAS_INTEL_256BIT_BUILTINS)

set(CMAKE_REQUIRED_FLAGS
  "${orig_cmake_required_flags} ${STANDARD_FLAG} -ffixed-xmm7")

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>

register uint8_t test_global_vector
asm(\"xmm7\") __attribute__ ((__vector_size__ (16)));

int main() {
  return 0;
}"
  HAS_GCC_GLOBAL_REGISTER_VARIABLE_XMM)

set(CMAKE_REQUIRED_FLAGS
  "${orig_cmake_required_flags} ${STANDARD_FLAG} -ffixed-xmm15")

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>

register uint8_t test_global_vector
asm(\"xmm15\") __attribute__ ((__vector_size__ (32)));

int main() {
  return 0;
}"
  HAS_GCC_GLOBAL_REGISTER_VARIABLE_YMM)

set(CMAKE_REQUIRED_FLAGS
  "${orig_cmake_required_flags} ${STANDARD_FLAG} -ffixed-xmm31")

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>

register uint8_t test_global_vector
asm(\"xmm31\") __attribute__ ((__vector_size__ (64)));

int main() {
  return 0;
}"
  HAS_GCC_GLOBAL_REGISTER_VARIABLE_ZMM)

endif()

set(CMAKE_REQUIRED_FLAGS "${orig_cmake_required_flags} ${STANDARD_FLAG}")

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>
#include <x86intrin.h>
#include <immintrin.h>

uint64_t pext(uint64_t x, uint64_t mask)
{
  return _pext_u64(x, mask);
}

uint64_t pdep(uint64_t x, uint64_t mask)
{
  return _pdep_u64(x, mask);
}
 
int main () {
  uint64_t x = 9;

  return pext(x, 231) & 0;
}"
    HAS_BMI2_PEXT_BITBOARD_SUPPORT)

set(CMAKE_REQUIRED_FLAGS "${orig_cmake_required_flags}")
