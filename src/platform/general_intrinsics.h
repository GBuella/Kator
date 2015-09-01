
#ifndef KATOR_GENERAL_INTRINSICS_H
#define KATOR_GENERAL_INTRINSICS_H

#include "platform.h"

#ifdef KATOR_HAS_X64_BMI_INTRINSICS
#  define KATOR_SYSTEM_GET_LSB64 _blsi_u64
#  define KATOR_SYSTEM_RESET_LSB64 _blsr_u64
#endif

#ifdef KATOR_HAS_GCC_BUILTIN_POPCNTLL_UINT64
#  define KATOR_SYSTEM_POPCNT64 __builtin_popcountll
#elif defined(KATOR_HAS_GCC_BUILTIN_POPCNTL_UINT64)
#  define KATOR_SYSTEM_POPCNT64 __builtin_popcountl
#elif defined(KATOR_HAS_GCC_BUILTIN_POPCNT_UINT64)
#  define KATOR_SYSTEM_POPCNT64 __builtin_popcount
#elif defined(KATOR_HAS_MSVCPP_POPCOUNT_64)
#  define KATOR_SYSTEM_POPCNT64(value) static_cast<int>(__popcnt64(value))
#endif // popcnt builtins

#ifdef KATOR_HAS_GCC_BUILTIN_BSWAP64
#  define KATOR_SYSTEM_BYTESWAP64 __builtin_bswap64
#elif defined(KATOR_HAS_MSVCPP_BYTESWAP_UINT64)
#  define KATOR_SYSTEM_BYTESWAP64 _byteswap_uint64
#endif // byte swap builtins

#ifdef KATOR_HAS_GCC_BUILTIN_CTZLL_UINT64
#  define KATOR_SYSTEM_CTZ64 __builtin_ctzll
#elif defined(KATOR_HAS_GCC_BUILTIN_CTZL_UINT64)
#  define KATOR_SYSTEM_CTZ64 __builtin_ctzl
#elif defined(KATOR_HAS_GCC_BUILTIN_CTZ_UINT64)
#  define KATOR_SYSTEM_CTZ64 __builtin_ctz
#elif defined(KATOR_HAS_MSVCPP_BITSCANFORWARD64)

    static inline int KATOR_SYSTEM_CTZ64_FUNC(uint64_t value)
    {
      unsigned long index;

      _BitScanForward64(&index, value);
      return (int)index;
    }

#  define KATOR_SYSTEM_CTZ64 KATOR_SYSTEM_CTZ64_FUNC

#endif // ctz builtins

#ifdef KATOR_HAS_BMI2_PEXT_BITBOARD_SUPPORT
#include <immintrin.h>
#endif


#endif /* !defined(KATOR_GENERAL_INTRINSICS_H) */
