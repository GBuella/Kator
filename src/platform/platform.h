
#ifndef KATOR_PLATFORM_H
#define KATOR_PLATFORM_H

#ifdef __GNUC__
#  pragma GCC system_header
#endif

#include "config.h"



#include <cstddef>
#include <cstdint>
#include <climits>
#include <memory>

#if (defined(KATOR_DEF_ULL_64_MACROS) || defined(KATOR_DEF_UL_64_MACROS))
   /* Define some CSTD numeric macrors if requested.{{{
      For the case a standard library doesn't have the the
      appropriate macros in <cstdint>
      e.g.: those supplie with android NDK
       ( 2015 January )
   }}}*/

#  ifndef UINT64_MAX
#    define UINT64_MAX UINT64_C(0xffffffffffffffff)
#  endif

#  ifndef UINT64_C
#    ifdef KATOR_DEF_ULL_64_MACROS
#      ifdef KATOR_DEF_UL_64_MACROS
#        error only one of KATOR_DEF_ULL_64_MACROS and \
               KATOR_DEF_UL_64_MACROS may be defined
#      endif
       static_assert((sizeof(unsigned long long) * CHAR_BIT) == 64,
                     "unsigned long long with exactly 64 bits expected");
#      define UINT64_C(x) (x##ull)
       static_assert(((0ull) - (1ull)) == (0xffffffffffffffffull),
                     "invalid unsigned long long"); // Belt and suspenders
#    endif /* KATOR_DEF_ULL_64_MACROS */
#    ifdef KATOR_DEF_UL_64_MACROS
#      ifdef KATOR_DEF_ULL_64_MACROS
#        error only one of KATOR_DEF_ULL_64_MACROS and \
               KATOR_DEF_UL_64_MACROS may be defined
#      endif
       static_assert((sizeof(unsigned long) * CHAR_BIT) == 64,
                     "unsigned long with exactly 64 bits expected");
#      define UINT64_C(x) (x##ul)
       static_assert(((0ul) - (1ul)) == (0xfffffffffffffffful),
                     "invalid unsigned long"); // Belt and suspenders
#    endif /* KATOR_DEF_UL_64_MACROS */
#  endif /* !defined(UINT64_C) */

#endif /* defined(KATOR_DEF_ULL_64_MACROS) || defined(KATOR_DEF_UL_64_MACROS) */





/* general compiler related macros
 */

#ifdef KATOR_HAS_RESTRICT_KEYWORD
#  define RESTRICT restrict
#elif defined(KATOR_HAS__RESTRICT__KEYWORD)
#  define RESTRICT __restrict__
#else
#  define RESTRICT
#endif

#ifdef KATOR_HAS_GCC_BUILTIN_UNREACHABLE
#  define ASSUME(x) { (x) ? (void)0 : __builtin_unreachable(); }
#elif defined(KATOR_HAS_MSVCPP_ASSUME)
#  define ASSUME __assume
#else
#  include <cassert>
#  define ASSUME(x) assert(x)
#endif

#ifdef KATOR_HAS_GCC_BUILTIN_EXPECT
#  define COMPILER_EXPECT(x, y) __builtin_expect(x, y)
#else
#  define COMPILER_EXPECT(x, y) (x)
#endif





/* CPU cache related features
 */
#ifdef KATOR_HAS_GCC_BUILTIN_PREFETCH

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

#ifdef KATOR_HAS_MSVC_M_PREFETCH

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

#endif // KATOR_HAS_MSVC_M_PREFETCH

#ifdef KATOR_HAS_MSVC_M_PREFETCHW

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

#endif // KATOR_HAS_MSVC_M_PREFETCHW

#endif // KATOR_HAS_GCC_BUILTIN_PREFETCH



namespace kator
{

void* aligned_allocate(size_t size, size_t alignment);
void anligned_deallocate(void*);

#if defined(KATOR_USE_ALIGNAS_64) || defined(KATOR_USE_ALIGNAS_32)

#  ifdef KATOR_USE_ALIGNAS_64
     constexpr size_t critical_alignment = 64;
#  else 
     constexpr size_t critical_alignment = 32;
#  endif

template <typename type> class aligned_allocator
{
public:
  typedef type value_type;
  typedef type* pointer;
  typedef const type* const_pointer;
  typedef type& reference;
  typedef const type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef std::true_type propagate_on_container_move_assignment;

  static type* allocate(size_t n, const type* = nullptr)
  {
    return aligned_allocate(sizeof(type) * n, alignof(type));
  }

  static void deallocate(type* p, size_t = 1)
  {
    anligned_deallocate(p);
  }

  static type* address(type& instance)
  {
    return &instance;
  }

  static const type* address(const type& instance)
  {
    return &instance;
  }

  static size_t max_size()
  {
    return SIZE_MAX;
  }

}; /* template aligned_allocator */

constexpr size_t critical_alignment_divisor = critical_alignment;

#else

constexpr size_t critical_alignment = 0;
constexpr size_t critical_alignment_divisor = 1;

using ::std::allocator aligned_allocator;

#endif // !defined(KATOR_USE_ALIGNAS_64)


void initialize_permanent_vectors();

} /* namespace kator */

#endif /* !defined(KATOR_PLATFORM_H) */
