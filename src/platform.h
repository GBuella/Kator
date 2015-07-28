
#ifndef KATOR_PLATFORM_H
#define KATOR_PLATFORM_H

#ifdef __GNUC__
#  pragma GCC system_header
#endif

#include "config.h"

#ifdef HAS_RESTRICT_KEYWORD
#  define RESTRICT restrict
#elif defined(HAS__RESTRICT__KEYWORD)
#  define RESTRICT __restrict__
#else
#  define RESTRICT
#endif

#ifdef HAS_GCC_BUILTIN_UNREACHABLE
#  define ASSUME(x) { (x) ? (void)0 : __builtin_unreachable(); }
#elif defined(HAS_MSVCPP_ASSUME)
#  define ASSUME __assume
#else
#  include <cassert>
#  define ASSUME(x) assert(x)
#endif

#ifdef HAS_GCC_BUILTIN_EXPECT
#  define COMPILER_EXPECT(x, y) __builtin_expect(x, y)
#else
#  define COMPILER_EXPECT(x, y) (x)
#endif

#ifdef HAS_GCC_BUILTIN_PREFETCH

static inline void
platform_prefetch_for_read_0(const void* address)
{
  __builtin_prefetch(address, 0, 0);
}

static inline void
platform_prefetch_for_read_1(const void* address)
{
  __builtin_prefetch(address, 0, 1);
}

static inline void
platform_prefetch_for_read_2(const void* address)
{
  __builtin_prefetch(address, 0, 2);
}

static inline void
platform_prefetch_for_read_3(const void* address)
{
  __builtin_prefetch(address, 0, 3);
}

static inline void
platform_prefetch_for_write_0(const void* address)
{
  __builtin_prefetch(address, 1, 0);
}

static inline void
platform_prefetch_for_write_1(const void* address)
{
  __builtin_prefetch(address, 1, 1);
}

static inline void
platform_prefetch_for_write_2(const void* address)
{
  __builtin_prefetch(address, 1, 2);
}

static inline void
platform_prefetch_for_write_3(const void* address)
{
  __builtin_prefetch(address, 1, 3);
}

#else

#ifdef HAS_MSVC_M_PREFETCH

static inline void
platform_prefetch_for_read_0(const void* address)
{
  _m_prefetch(address);
}

static inline void
platform_prefetch_for_read_1(const void* address)
{
  _m_prefetch(address);
}

static inline void
platform_prefetch_for_read_2(const void* address)
{
  _m_prefetch(address);
}

static inline void
platform_prefetch_for_read_3(const void* address)
{
  _m_prefetch(address);
}

#else

static inline void platform_prefetch_for_read_0(const void*) { }
static inline void platform_prefetch_for_read_1(const void*) { }
static inline void platform_prefetch_for_read_2(const void*) { }
static inline void platform_prefetch_for_read_3(const void*) { }

#endif

#ifdef HAS_MSVC_M_PREFETCHW

static inline void
platform_prefetch_for_write_0(volatile const void* address)
{
  _m_prefetchw(address);
}

static inline void
platform_prefetch_for_write_1(volatile const void* address)
{
  _m_prefetchw(address);
}

static inline void
platform_prefetch_for_write_2(volatile const void* address)
{
  _m_prefetchw(address);
}

static inline void
platform_prefetch_for_write_3(volatile const void* address)
{
  _m_prefetchw(address);
}

#else

static inline void platform_prefetch_for_write_0(const void*) { }
static inline void platform_prefetch_for_write_1(const void*) { }
static inline void platform_prefetch_for_write_2(const void*) { }
static inline void platform_prefetch_for_write_3(const void*) { }

#endif

#endif

#if defined(HAS_MSVCPP_BYTESWAP_UINT64) \
    || defined(HAS_MSVCPP_POPCOUNT_64) \
    || defined(HAS_MSVCPP_BITSCANFORWARD64)
#  include <intrin.h>
#  define HAS_MSVCPP_INTRINSICS
#endif

#endif /* !defined(KATOR_PLATFORM_H) */
