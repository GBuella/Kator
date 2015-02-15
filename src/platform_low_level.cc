
#include "platform_low_level.h"

namespace kator
{

void initialize_permanent_vectors()
{
#if defined(USE_BOARD_VECTOR_64) || defined(USE_PIECE_MAP_VECTOR)
# if defined(HAS_GCC_GLOBAL_REGISTER_VARIABLE_XMM) \
     && defined(HAS_INTEL_128BIT_BUILTINS)

#   if defined(HAS_GCC_GLOBAL_REGISTER_VARIABLE_YMM) \
       && defined(HAS_INTEL_256BIT_BUILTINS)

#     if defined(HAS_GCC_GLOBAL_REGISTER_VARIABLE_ZMM) \
       && defined(HAS_INTEL_512BIT_BUILTINS)

        vector_64_one = vector_64_one_value;
        vector_64_zero = vector_64_zero_value;
        bswap_shuffle_key_64 = bswap_shuffle_key_64_value;
   
#     else

        vector_32_one = vector_32_one_value;
        vector_32_zero = vector_32_zero_value;
        bswap_shuffle_key_32 = bswap_shuffle_key_32_value;

#     endif
#   else

    vector_16_one = vector_16_one_value;
    vector_16_zero = vector_16_zero_value;
    bswap_shuffle_key_16 = bswap_shuffle_key_16_value;

#   endif
# endif
#endif
}

#ifdef USE_ALIGNAS_64

unsigned char* aligned_64_allocator_allocate(size_t size)
{
  // GCC 4.9  --  Y U No have std::align ????

  unsigned char* storage = new unsigned char[size + 0x40];
  uintptr_t original_alignment = reinterpret_cast<uintptr_t>(storage) & 0x3f;
  unsigned char offset = static_cast<unsigned char>(0x40 - original_alignment);

  storage[offset - 1] = offset;
  return storage + offset;
}

void aligned_64_allocator_deallocate(unsigned char* p)
{
  unsigned char offset = *(p - 1);

  delete[](p - offset);
}

#endif // USE_ALIGNAS_64

} // namespace kator
