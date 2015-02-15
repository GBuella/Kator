/* Platform specific configurations related {{{
   to 64 bit integers, and 64 octet alignment.
   The number 64 in the file's name refers to both.
   Taltos requires the standard library to define a
   uint64_t type, this seems to be satisfied by most
   C++11 compilers. If the library has no uint64_t in
   <cstdint>, that probably means an exotic platform,
   therefore Taltos does not attempt to guess if any
   integral type has 64 value bits.

   Some compilers support some sort of syntax for vectors
   as a language extension. This is not very useful in
   the case of this chess engine, nevertheless it might
   turn out to be useful in some future compiler/architecture.
}}}*/

#ifndef KATOR_PLATFORM_LOW_LEVEL_H
#define KATOR_PLATFORM_LOW_LEVEL_H

#pragma GCC system_header

#include "platform.h"

#include <cstddef>
#include <cstdint>

#if (defined(DEF_ULL_64_MACROS) || defined(DEF_UL_64_MACROS))
   /* Define some CSTD numeric macrors if requested.
      For the case a standard library doesn't have the the
      appropriate macros in <cstdint>
      e.g.: those supplie with android NDK
       ( 2015 January )
   */

#  ifndef UINT64_MAX
#    define UINT64_MAX UINT64_C(0xffffffffffffffff)
#  endif

#  ifndef UINT64_C
#    ifdef DEF_ULL_64_MACROS
#      ifdef DEF_UL_64_MACROS
#        error only one of DEF_ULL_64_MACROS and DEF_UL_64_MACROS may be defined
#      endif
       static_assert(sizeof(unsigned long long) == 8, "8 byte ull expected");
#      define UINT64_C(x) (x##ull)
       static_assert((unsigned long long)(-1ll) == UINT64_MAX,
                     "invalid unsigned long"); // Belt and suspenders
#    endif /* DEF_ULL_64_MACROS */
#    ifdef DEF_UL_64_MACROS
#      ifdef DEF_ULL_64_MACROS
#        error only one of DEF_ULL_64_MACROS and DEF_UL_64_MACROS may be defined
#      endif
       static_assert(sizeof(unsigned long) == 8, "8 byte ul expected");
#      define UINT64_C(x) (x##ul)
       static_assert((unsigned long long)(-1l) == UINT64_MAX,
                     "invalid unsigned long"); // Belt and suspenders
#    endif /* DEF_UL_64_MACROS */
#  endif /* !defined(UINT64_C) */

#endif /* defined(DEF_ULL_64_MACROS) || defined(DEF_UL_64_MACROS) */


#if defined(USE_BOARD_VECTOR_64) || defined(USE_PIECE_MAP_VECTOR)

#  if defined(HAS_GCC_VECTOR)

     typedef uint64_t vector64 __attribute__ ((vector_size(64)));
     typedef uint8_t vector_8_64 __attribute__ ((vector_size(64)));
     typedef uint64_t vector32 __attribute__ ((vector_size(32)));
     typedef uint8_t vector_8_32 __attribute__ ((vector_size(32)));
     typedef uint64_t vector16 __attribute__ ((vector_size(16)));
     typedef uint8_t vector_8_16 __attribute__ ((vector_size(16)));

     static constexpr vector_8_16 vector_16_zero_value = {
                   0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0};

     static constexpr vector_8_16 vector_16_one_value = {
                   1,  1,  1,  1,  1,  1,  1,  1,
                   1,  1,  1,  1,  1,  1,  1,  1};

     static constexpr vector_8_16 bswap_shuffle_key_16_value = {
                  15, 14, 13, 12, 11, 10,  9,  8,
                   7,  6,  5,  4,  3,  2,  1,  0};

     static constexpr vector_8_32 vector_32_zero_value = {
                   0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0};

     static constexpr vector_8_32 vector_32_one_value = {
                   1,  1,  1,  1,  1,  1,  1,  1,
                   1,  1,  1,  1,  1,  1,  1,  1,
                   1,  1,  1,  1,  1,  1,  1,  1,
                   1,  1,  1,  1,  1,  1,  1,  1};

     static constexpr vector_8_32 bswap_shuffle_key_32_value = {
                  15, 14, 13, 12, 11, 10,  9,  8,
                   7,  6,  5,  4,  3,  2,  1,  0,
                  15, 14, 13, 12, 11, 10,  9,  8,
                   7,  6,  5,  4,  3,  2,  1,  0};

     static constexpr vector_8_64 vector_64_zero_value = {
                   0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0,
                   0,  0,  0,  0,  0,  0,  0,  0};

     static constexpr vector_8_64 vector_64_one_value = {
                   1,  1,  1,  1,  1,  1,  1,  1,
                   1,  1,  1,  1,  1,  1,  1,  1,
                   1,  1,  1,  1,  1,  1,  1,  1,
                   1,  1,  1,  1,  1,  1,  1,  1,
                   1,  1,  1,  1,  1,  1,  1,  1,
                   1,  1,  1,  1,  1,  1,  1,  1,
                   1,  1,  1,  1,  1,  1,  1,  1,
                   1,  1,  1,  1,  1,  1,  1,  1};

     static constexpr vector_8_64 bswap_shuffle_key_64_value = {
                  15, 14, 13, 12, 11, 10,  9,  8,
                   7,  6,  5,  4,  3,  2,  1,  0,
                  15, 14, 13, 12, 11, 10,  9,  8,
                   7,  6,  5,  4,  3,  2,  1,  0,
                  15, 14, 13, 12, 11, 10,  9,  8,
                   7,  6,  5,  4,  3,  2,  1,  0,
                  15, 14, 13, 12, 11, 10,  9,  8,
                   7,  6,  5,  4,  3,  2,  1,  0};

#   ifdef HAS_INTEL_128BIT_BUILTINS
#    include <x86intrin.h>

     static inline vector_8_16
     swap_64_bit_members_128(vector_8_16 a)
     {
       return reinterpret_cast<vector_8_16>(_mm_permute_pd(
          reinterpret_cast<double __attribute__ ((vector_size(16)))>(a), 1));
     }

#    ifdef HAS_INTEL_256BIT_BUILTINS

     static inline vector_8_32
     swap_64_bit_members_256(vector_8_32 a)
     {
       return _mm256_permute4x64_epi64(a, 0 + (1 << 4) + (2 << 2) + (3 << 0));
     }

#     ifdef HAS_INTEL_512BIT_BUILTINS

     static inline vector_8_64
     swap_64_bit_members_512(vector_8_64 a)
     {
#      error not implemented yet
     }

#     endif // HAS_INTEL_512BIT_BUILTINS
#    endif // HAS_INTEL_256BIT_BUILTINS
#   endif // HAS_INTEL_128BIT_BUILTINS

   /* Permanent storage for some vector constants, used
      to avoid loading them into the CPU each time. These
      are meant to hold their value across function calls.
      Currently, only GCC seems to support this.
        ( 2015 January )
    */
#   if defined(HAS_GCC_GLOBAL_REGISTER_VARIABLE_XMM) \
       && defined(HAS_INTEL_128BIT_BUILTINS)

#    if defined(HAS_GCC_GLOBAL_REGISTER_VARIABLE_YMM) \
       && defined(HAS_INTEL_256BIT_BUILTINS)

#     if defined(HAS_GCC_GLOBAL_REGISTER_VARIABLE_ZMM) \
       && defined(HAS_INTEL_512BIT_BUILTINS)

       register vector_8_64 vector_64_zero asm("xmm29");
       register vector_8_64 vector_64_one asm("xmm30");
       register vector_8_64 bswap_shuffle_key_64 asm("xmm31");

       /*
       register vector_8_32 vector_32_zero asm("xmm29");
       register vector_8_32 vector_32_one asm("xmm30");
       register vector_8_32 bswap_shuffle_key_32 asm("xmm31");

       register vector_8_16 vector_16_zero asm("xmm29");
       register vector_8_16 vector_16_one asm("xmm30");
       register vector_8_16 bswap_shuffle_key_16 asm("xmm31");
       */

#     else // defined(HAS_GCC_GLOBAL_REGISTER_VARIABLE_ZMM)

       register vector_8_32 vector_32_zero asm("xmm13");
       register vector_8_32 vector_32_one asm("xmm14");
       register vector_8_32 bswap_shuffle_key_32 asm("xmm15");

       /*
       register vector_8_16 vector_16_zero asm("xmm13");
       register vector_8_16 vector_16_one asm("xmm14");
       register vector_8_16 bswap_shuffle_key_16 asm("xmm15");
       */

#    endif // !defined(HAS_GCC_GLOBAL_REGISTER_VARIABLE_ZMM)
#    else // defined(HAS_GCC_GLOBAL_REGISTER_VARIABLE_YMM)

     register vector_8_16 vector_16_zero asm("xmm5");
     register vector_8_16 vector_16_one asm("xmm6");
     register vector_8_16 bswap_shuffle_key_16 asm("xmm7");

#   endif // !defined(HAS_GCC_GLOBAL_REGISTER_VARIABLE_YMM)
#   else // defined(HAS_GCC_GLOBAL_REGISTER_VARIABLE_XMM)

namespace
{ // For the case no global register support is detected.

  constexpr vector_8_16 vector_16_zero = vector_16_zero_value;
  constexpr vector_8_16 vector_16_one = vector_16_one_value;
  constexpr vector_8_16 bswap_shuffle_key_16 = bswap_shuffle_key_16_value;
  constexpr vector_8_32 vector_32_zero = vector_32_zero_value;
  constexpr vector_8_32 vector_32_one = vector_32_one_value;
  constexpr vector_8_32 bswap_shuffle_key_32 = bswap_shuffle_key_32_value;
  constexpr vector_8_64 vector_64_zero = vector_64_zero_value;
  constexpr vector_8_64 vector_64_one = vector_64_one_value;
  constexpr vector_8_64 bswap_shuffle_key_64 = bswap_shuffle_key_64_value;
}


#  endif // defined(HAS_GCC_VECTOR)


#  else
#    error No compiler vector support found
#  endif
#endif // USE_BOARD_VECTOR_64 or USE_PIECE_MAP_VECTOR

#ifdef USE_ALIGNAS_64

#include <memory>

namespace kator
{

unsigned char* aligned_64_allocator_allocate(size_t);
void aligned_64_allocator_deallocate(unsigned char*);

template <class T>
class aligned_64_allocator
{
public:
  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef std::true_type propagate_on_container_move_assignment;

  static T* allocate(size_t n, const T* = nullptr)
  {
    return reinterpret_cast<T*>(aligned_64_allocator_allocate(sizeof(T) * n));
  }

  static void deallocate(T* p, size_t = 1)
  {
    aligned_64_allocator_deallocate(reinterpret_cast<unsigned char*>(p));
  }

  static T* address(T& instance)
  {
    return &instance;
  }

  static const T* address(const T& instance)
  {
    return &instance;
  }

  static size_t max_size()
  {
    return SIZE_MAX;
  }

}; /* template aligned_64_allocator */

} /* namespace kator */

#endif // !defined(USE_ALIGNAS_64)

#ifdef HAS_GCC_BMI_INTRINSICS
#  define SYSTEM_GET_LSB64 _blsi_u64
#  define SYSTEM_RESET_LSB64 _blsr_u64
#endif

#ifdef HAS_GCC_BUILTIN_POPCNTLL_UINT64
#  define SYSTEM_POPCNT64 __builtin_popcountll
#elif defined(HAS_GCC_BUILTIN_POPCNTL_UINT64)
#  define SYSTEM_POPCNT64 __builtin_popcountl
#elif defined(HAS_GCC_BUILTIN_POPCNT_UINT64)
#  define SYSTEM_POPCNT64 __builtin_popcount
#elif defined(HAS_MSVCPP_POPCOUNT_64)
#  define SYSTEM_POPCNT64 static_cast<int>__popcnt64
#endif // popcnt builtins

#ifdef HAS_GCC_BUILTIN_BSWAP64
#  define SYSTEM_BYTESWAP64 __builtin_bswap64
#elif defined(HAS_MSVCPP_BYTESWAP_UINT64)
#  define SYSTEM_BYTESWAP64 _byteswap_uint64
#endif // byte swap builtins

#ifdef HAS_GCC_BUILTIN_CTZLL_UINT64
#  define SYSTEM_CTZ64 __builtin_ctzll
#elif defined(HAS_GCC_BUILTIN_CTZL_UINT64)
#  define SYSTEM_CTZ64 __builtin_ctzl
#elif defined(HAS_GCC_BUILTIN_CTZ_UINT64)
#  define SYSTEM_CTZ64 __builtin_ctz
#elif defined(HAS_MSVCPP_BITSCANFORWARD64)

    static inline int SYSTEM_CTZ64_FUNC(uint64_t value)
    {
      unsigned long index;

      _BitScanForward64(&index, value);
      return (int)index;
    }

#  define SYSTEM_CTZ64 SYSTEM_CTZ64_FUNC

#endif // ctz builtins

namespace kator
{

void initialize_permanent_vectors();

}

#ifdef HAS_BMI2_PEXT_BITBOARD_SUPPORT
#include <immintrin.h>
#endif

#endif // !defined(KATOR_PLATFORM_LOW_LEVEL_H)

