
#ifndef KATOR_X64_VECTORS_H
#define KATOR_X64_VECTORS_H

#include "platform.h"

#ifdef KATOR_HAS_X64_512BIT_BUILTINS

#  include <immintrin.h>

#  ifdef KATOR_HAS_GCC_GLOBAL_REGISTER_VARIABLE_ZMM

     register __m512 vector_64_one asm("xmm30");
     register __m512 bswap_shuffle_key_64 asm("xmm31");

     namespace kator
     {
       static inline void local_init_x64_vectors_32()
       {
       }
     }

#  else // KATOR_HAS_GCC_GLOBAL_REGISTER_VARIABLE_ZMM

     namespace kator
     {
       static __m512i vector_64_one;
       extern const __m512i bswap_shuffle_key_64;

       static inline void local_init_x64_vectors_64()
       {
         vector_64_one = _mm512_set1_epi8(1);
       }

     }

#  endif // KATOR_HAS_GCC_GLOBAL_REGISTER_VARIABLE_ZMM

#elif defined(KATOR_HAS_X64_256BIT_BUILTINS)

#  include <immintrin.h>

#  ifdef KATOR_HAS_GCC_GLOBAL_REGISTER_VARIABLE_YMM

     register __m256i vector_32_one asm("xmm14");

#    ifdef KATOR_HAS_X64_256BIT_AVX2_BUILTINS
       register __m256i bswap_shuffle_key_32 asm("xmm15");
#    else
       register __m128i bswap_shuffle_key_16 asm("xmm15");
#    endif

     namespace kator
     {
       static inline void local_init_x64_vectors_32()
       {
       }
     }
#  else

     namespace kator
     {
       static __m256i vector_32_one;
#      ifdef KATOR_HAS_X64_256BIT_AVX2_BUILTINS
         extern const __m256i bswap_shuffle_key_32;
#      else
         extern const __m128i bswap_shuffle_key_16;
#      endif

       static inline void local_init_x64_vectors_32()
       {
         vector_32_one = _mm256_set1_epi8(1);
       }

     }

#  endif

#elif defined(KATOR_HAS_X64_128BIT_BUILTINS)

#  include <emmintrin.h>

#  ifdef KATOR_HAS_GCC_GLOBAL_REGISTER_VARIABLE_XMM

     register __m128i vector_16_one asm("xmm6");

#    ifdef KATOR_HAS_X64_128BIT_SSSE3_SHUFFLE_BUILTIN
       register __m128i bswap_shuffle_key_16 asm("xmm7");
#    endif

     namespace kator
     {
       static inline void local_init_x64_vectors_16()
       {
       }
     }

#  else

     namespace kator
     {
       static __m128i vector_16_one;

#      ifdef KATOR_HAS_X64_128BIT_SSSE3_SHUFFLE_BUILTIN
         extern const __m128i bswap_shuffle_key_16;
#      endif

       static inline void local_init_x64_vectors_16()
       {
         vector_16_one = _mm_set1_epi8(1);
       }

     }

#  endif

#endif


#endif /* !defined(KATOR_X64_VECTORS_H) */
