
CHECK_CXX_SOURCE_RUNS("
#include <cstdlib>
#include <xmmintrin.h>

unsigned char* some_alloc(size_t size)
{
  return reinterpret_cast<unsigned char*>(_mm_malloc(size, 64));
}

void some_free(unsigned char* p)
{
  _mm_free(p);
}

int main()
{
  unsigned char* data = some_alloc(1234);
  data[4] = 3;
  some_free(data);
  return 0;
}"
  KATOR_HAS_X64_MM_ALLOC)

CHECK_CXX_SOURCE_RUNS("
    // only SSE, SSE2
#include <cstdint>
#include <xmmintrin.h>
#include <emmintrin.h>

static const __m128i key16 = _mm_setr_epi8( 7,  6,  5,  4,  3,  2,  1,  0,
                                           15, 14, 13, 12, 11, 10,  9,  8);

int main() {
  double b[2];
  __m128i c;

  c = _mm_xor_si128(key16, key16);
  c = _mm_cmpeq_epi8(c, key16);
  c = _mm_andnot_si128(c, key16);
  _mm_storel_pd(&(b[0]), _mm_castsi128_pd(c));
  _mm_storeh_pd(&(b[1]), _mm_castsi128_pd(c));
  return (reinterpret_cast<int*>(&c))[1] & 0;
}"
  KATOR_HAS_X64_128BIT_BUILTINS)

CHECK_CXX_SOURCE_RUNS("
    // only SSSE3
#include <cstdint>
#include <emmintrin.h>
#include <tmmintrin.h>

static const __m128i key16 = _mm_setr_epi8( 7,  6,  5,  4,  3,  2,  1,  0,
                                           15, 14, 13, 12, 11, 10,  9,  8);

int main() {
  __m128i a;

  a = _mm_shuffle_epi8(key16, key16);
  return (reinterpret_cast<int*>(&a))[1] & 0;
}"
  KATOR_HAS_X64_128BIT_SSSE3_SHUFFLE_BUILTIN)

CHECK_CXX_SOURCE_RUNS("
    // only AVX
#include <cstdint>
#include <immintrin.h>

static const __m256d key32 = _mm256_castsi256_pd(
                             _mm256_setr_epi8( 7,  6,  5,  4,  3,  2,  1,  0,
                                              15, 14, 13, 12, 11, 10,  9,  8,
                                               7,  6,  5,  4,  3,  2,  1,  0,
                                              15, 14, 13, 12, 11, 10,  9,  8));

int main() {
  __m256d b, c;

  b = _mm256_permute_pd(key32, 1);
  c = _mm256_xor_pd(b, key32);
  return (reinterpret_cast<int*>(&c))[1] & 0;
}"
  KATOR_HAS_X64_256BIT_BUILTINS)

CHECK_CXX_SOURCE_RUNS("
    // only AVX2
#include <cstdint>
#include <immintrin.h>

static const __m256i key32 = _mm256_setr_epi8( 7,  6,  5,  4,  3,  2,  1,  0,
                                              15, 14, 13, 12, 11, 10,  9,  8,
                                               7,  6,  5,  4,  3,  2,  1,  0,
                                              15, 14, 13, 12, 11, 10,  9,  8);

int main() {
  __m256i b, c;

  b = _mm256_permute4x64_epi64(key32, 17);
  c = _mm256_xor_si256(b, key32);
  c = _mm256_cmpeq_epi8(b, key32);
  c = _mm256_andnot_si256(c, key32);
  c = _mm256_shuffle_epi8(c, key32);
  return (reinterpret_cast<int*>(&c))[1] & 0;
}"
  KATOR_HAS_X64_256BIT_AVX2_BUILTINS)

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>
#include <immintrin.h>

uint64_t blsi(uint64_t x) { return _blsi_u64(x); }
uint64_t blsr(uint64_t x) { return _blsr_u64(x); }

int main() {
  uint64_t x = blsi(0x404040);
  uint64_t y = blsr(0x808080);
  return x & y &  0;
}"
  KATOR_HAS_X64_BMI_INTRINSICS)

CHECK_CXX_SOURCE_RUNS("
#include <cstdint>
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
  KATOR_HAS_BMI2_PEXT_BITBOARD_SUPPORT)

