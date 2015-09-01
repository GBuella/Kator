
CHECK_CXX_COMPILER_FLAG("-Werror" KATOR_COMPILER_SUPPORTS_WERROR)
CHECK_CXX_COMPILER_FLAG("-Wall" KATOR_COMPILER_SUPPORTS_WALL)
CHECK_CXX_COMPILER_FLAG("-Wextra" KATOR_COMPILER_SUPPORTS_WEXTRA)
CHECK_CXX_COMPILER_FLAG("-pedantic" KATOR_COMPILER_SUPPORTS_PEDANTIC)
if(NOT NO_ARCHNATIVE)
  CHECK_CXX_COMPILER_FLAG("-march=native" KATOR_COMPILER_SUPPORTS_MARCHNATIVE)
endif()
if(KATOR_COMPILER_SUPPORTS_MARCHNATIVE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -march=native ")
endif()
CHECK_CXX_COMPILER_FLAG("-mno-vzeroupper"
  KATOR_COMPILER_SUPPORTS_MNO_VZEROUPPER)
if(KATOR_COMPILER_SUPPORTS_MNO_VZEROUPPER)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -mno-vzeroupper ")
endif()
CHECK_CXX_COMPILER_FLAG("-flax-vector-conversions"
  KATOR_COMPILER_SUPPORTS_FLAX_VECTOR_CONVERSIONS)
if(KATOR_COMPILER_SUPPORTS_FLAX_VECTOR_CONVERSIONS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -flax-vector-conversions ")
endif()
CHECK_CXX_COMPILER_FLAG("-mdynamic-no-pic"
  KATOR_COMPILER_SUPPORTS_MDYNAMIC_NO_PIC)
if(KATOR_COMPILER_SUPPORTS_MDYNAMIC_NO_PIC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mdynamic-no-pic ")
endif()
CHECK_CXX_COMPILER_FLAG("-mprefer-avx128"
  KATOR_COMPILER_SUPPORTS_MPREFER_AVX128)
if(KATOR_COMPILER_SUPPORTS_MPREFER_AVX128)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mprefer-avx128 ")
endif()

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>
uint64_t swapit(uint64_t x) { return __builtin_bswap64(x); }
int main() {
  uint64_t x = swapit(0x404040);
  return x & 0;
}"
  KATOR_HAS_GCC_BUILTIN_BSWAP64)

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
  KATOR_HAS_GCC_BUILTIN_POPCNTLL_UINT64)

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
  KATOR_HAS_GCC_BUILTIN_POPCNTL_UINT64)

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
  KATOR_HAS_GCC_BUILTIN_POPCNT_UINT64)

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
  KATOR_HAS_GCC_BUILTIN_CTZLL_UINT64)

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
  KATOR_HAS_GCC_BUILTIN_CTZL_UINT64)

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
  KATOR_HAS_GCC_BUILTIN_CTZ_UINT64)

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
  KATOR_HAS_GCC_VECTOR)

CHECK_CXX_SOURCE_RUNS("
int main() {
  int x[1024];
  x[5] = 9;
  __builtin_prefetch(x + 5, 0, 2);
  return x[5] & 0;
}"
  KATOR_HAS_GCC_BUILTIN_PREFETCH)

set(CMAKE_REQUIRED_FLAGS
  "${orig_cmake_required_flags} ${KATOR_STANDARD_FLAG} -ffixed-xmm7")

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>

register uint8_t test_global_vector
asm(\"xmm7\") __attribute__ ((__vector_size__ (16)));

int main() {
  return 0;
}"
  KATOR_HAS_GCC_GLOBAL_REGISTER_VARIABLE_XMM)

set(CMAKE_REQUIRED_FLAGS
  "${orig_cmake_required_flags} ${KATOR_STANDARD_FLAG} -ffixed-xmm15")

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>

register uint8_t test_global_vector
asm(\"xmm15\") __attribute__ ((__vector_size__ (32)));

int main() {
  return 0;
}"
  KATOR_HAS_GCC_GLOBAL_REGISTER_VARIABLE_YMM)

set(CMAKE_REQUIRED_FLAGS
  "${orig_cmake_required_flags} ${KATOR_STANDARD_FLAG} -ffixed-xmm31")

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>

register uint8_t test_global_vector
asm(\"xmm31\") __attribute__ ((__vector_size__ (64)));

int main() {
  return 0;
}"
  KATOR_HAS_GCC_GLOBAL_REGISTER_VARIABLE_ZMM) 
set(CMAKE_REQUIRED_FLAGS "${orig_cmake_required_flags} ${KATOR_STANDARD_FLAG}")

