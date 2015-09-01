
#include "platform.h"
#include "x64_vectors.h"

#include <new>

#if defined(KATOR_HAS_X64_512BIT_BUILTINS)

static const __m512i bswap_shuffle_key_64_value = {
                  15, 14, 13, 12, 11, 10,  9,  8,
                   7,  6,  5,  4,  3,  2,  1,  0,
                  15, 14, 13, 12, 11, 10,  9,  8,
                   7,  6,  5,  4,  3,  2,  1,  0,
                  15, 14, 13, 12, 11, 10,  9,  8,
                   7,  6,  5,  4,  3,  2,  1,  0,
                  15, 14, 13, 12, 11, 10,  9,  8,
                   7,  6,  5,  4,  3,  2,  1,  0};

#  ifndef  KATOR_HAS_GCC_GLOBAL_REGISTER_VARIABLE_ZMM

     const __m512i kator::bswap_shuffle_key_64 = bswap_shuffle_key_64_value;

#  endif

#elif defined(KATOR_HAS_X64_256BIT_AVX2_BUILTINS)

static const __m256i bswap_shuffle_key_32_value = _mm256_setr_epi8(
                  15, 14, 13, 12, 11, 10,  9,  8,
                   7,  6,  5,  4,  3,  2,  1,  0,
                  15, 14, 13, 12, 11, 10,  9,  8,
                   7,  6,  5,  4,  3,  2,  1,  0);

#  ifndef KATOR_HAS_GCC_GLOBAL_REGISTER_VARIABLE_YMM

     const __m256i kator::bswap_shuffle_key_32 = bswap_shuffle_key_32_value;

#  endif

#elif defined(KATOR_HAS_X64_128BIT_SSSE3_SHUFFLE_BUILTIN)

static const __m128i bswap_shuffle_key_16_value = _mm_setr_epi8(
                  15, 14, 13, 12, 11, 10,  9,  8,
                   7,  6,  5,  4,  3,  2,  1,  0);

#  ifndef KATOR_HAS_GCC_GLOBAL_REGISTER_VARIABLE_XMM

     const __m128i kator::bswap_shuffle_key_16 = bswap_shuffle_key_16_value;

#  endif

#endif

void kator::initialize_permanent_vectors()
{
# if defined(KATOR_HAS_GCC_GLOBAL_REGISTER_VARIABLE_XMM) \
     && defined(KATOR_HAS_X64_128BIT_BUILTINS)

#   if defined(KATOR_HAS_GCC_GLOBAL_REGISTER_VARIABLE_YMM) \
       && defined(KATOR_HAS_X64_256BIT_BUILTINS)

#     if defined(KATOR_HAS_GCC_GLOBAL_REGISTER_VARIABLE_ZMM) \
       && defined(KATOR_HAS_X64_512BIT_BUILTINS)

        
        vector_64_one = _mm512_set1_epi8(1);
        bswap_shuffle_key_64 = bswap_shuffle_key_64_value;
   
#     else

        vector_32_one = _mm256_set1_epi8(1);
        bswap_shuffle_key_32 = bswap_shuffle_key_32_value;

#     endif
#   else

    vector_16_one = _mm_set1_epi8(1);

#   ifdef KATOR_HAS_X64_128BIT_SSSE3_SHUFFLE_BUILTIN
    bswap_shuffle_key_16 = bswap_shuffle_key_16_value;
#   endif

#   endif
# endif
}

#if defined(KATOR_USE_ALIGNAS_64) || defined(KATOR_USE_ALIGNAS_32)

#ifdef KATOR_HAS_X64_MM_ALLOC_NOPE  // TODO: check all cases

#include <xmmintrin.h>

void* kator::aligned_allocate(size_t size, size_t alignment)
{
  void* pointer = _mm_malloc(size, alignment);

  if (pointer == nullptr) {
    throw std::bad_alloc();
  }
  return pointer;
}

void kator::anligned_deallocate(void* pointer)
{
  _mm_free(pointer);
}

#else

void* kator::aligned_allocate(size_t size, size_t alignment)
{
  // GCC 4.9  --  Y U No have std::align ????

  unsigned char* storage = new unsigned char[size + alignment];
  if (storage == nullptr) {
    throw std::bad_alloc();
  }
  uintptr_t original_alignment =
    reinterpret_cast<uintptr_t>(storage) & (alignment-1);
  unsigned char offset =
    static_cast<unsigned char>(alignment - original_alignment);

  storage[offset - 1] = offset;
  return storage + offset;
}

void kator::anligned_deallocate(void* pointer)
{
  unsigned char* pchar = reinterpret_cast<unsigned char*>(pointer);
  unsigned char offset = *(pchar - 1);

  delete[](pchar - offset);
}

#endif // KATOR_HAS_MM_ALLOC

#else

void* kator::aligned_allocate(size_t size, size_t)
{
  return ::operator new(size);
}

void kator::anligned_deallocate(void* pointer)
{
  delete pointer;
}

#endif // KATOR_USE_ALIGNAS_64 || KATOR_USE_ALIGNAS_32

