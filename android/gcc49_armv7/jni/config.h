
#ifndef TALTOS_CONFIG_H
#define TALTOS_CONFIG_H

/* system specific configurations,
   maintained manually for ARMv5 32bit Android
   GCC 4.9
    ( 2015 January )
*/

#define DEF_ULL_64_MACROS
#define HAS__RESTRICT__KEYWORD
#define HAS_GCC_BUILTIN_BSWAP64
#define HAS_GCC_BUILTIN_POPCNTLL_UINT64
#define HAS_GCC_BUILTIN_CTZLL_UINT64
#define HAS_GCC_BUILTIN_UNREACHABLE
#define HAS_GCC_BUILTIN_EXPECT
#define HAS_GCC_BUILTIN_PREFETCH
#define CAN_DO_SETVBUF

namespace taltos
{

constexpr unsigned taltos_version_major = 0;
constexpr unsigned taltos_version_minor = 1;

} /* namespace taltos */

#endif /* !define(TALTOS_CONFIG_H) */

