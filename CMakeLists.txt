cmake_minimum_required(VERSION 3.0.0)
project(kator CXX)
set(KATOR_VERSION_MAJOR 0)
set(KATOR_VERSION_MINOR 1)


include(cmake_includes/sources.cmake)

option(USE_WEVERYTHING
    "Use the -Weverything compiler flag" OFF)

option(USE_PEXT_BITBOARD
    "Use pext bitboard instead of old-style magic bitboards" OFF)


include(CheckCXXCompilerFlag)
include(CheckCXXSourceRuns)

ADD_LIBRARY(kator_lib OBJECT ${SOURCES})
ADD_EXECUTABLE(kator src/main.cc $<TARGET_OBJECTS:kator_lib>)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(STANDARD_FLAG "")

if(NOT MSVC)

# Even after setting CXX_STANDARD to 14, cmake still doesn't
# compile the feature tests with the c++14 flag
#
  CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
  CHECK_CXX_COMPILER_FLAG("-std=c++1y" COMPILER_SUPPORTS_CXX1Y)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
  CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
  if(COMPILER_SUPPORTS_CXX14)
    set(STANDARD_FLAG "-std=c++14")
  elseif(COMPILER_SUPPORTS_CXX1Y)
    set(STANDARD_FLAG "-std=c++1y")
  elseif(COMPILER_SUPPORTS_CXX11)
    set(STANDARD_FLAG "-std=c++11")
  elseif(COMPILER_SUPPORTS_CXX0X)
    set(STANDARD_FLAG "-std=c++0x")
  endif()

  target_compile_options(kator_lib PUBLIC "${STANDARD_FLAG}")
  target_compile_options(kator PUBLIC "${STANDARD_FLAG}")

  CHECK_CXX_COMPILER_FLAG("-march=native"
    COMPILER_SUPPORTS_MARCHNATIVE)
  if(COMPILER_SUPPORTS_MARCHNATIVE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -march=native ")
  endif()
  CHECK_CXX_COMPILER_FLAG("-mno-vzeroupper"
    COMPILER_SUPPORTS_MNO_VZEROUPPER)
  if(COMPILER_SUPPORTS_MNO_VZEROUPPER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -mno-vzeroupper ")
  endif()
  CHECK_CXX_COMPILER_FLAG("-flax-vector-conversions"
    COMPILER_SUPPORTS_FLAX_VECTOR_CONVERSIONS)
  if(COMPILER_SUPPORTS_FLAX_VECTOR_CONVERSIONS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -flax-vector-conversions ")
  endif()
  CHECK_CXX_COMPILER_FLAG("-mdynamic-no-pic"
    COMPILER_SUPPORTS_MDYNAMIC_NO_PIC)
  if(COMPILER_SUPPORTS_MDYNAMIC_NO_PIC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mdynamic-no-pic ")
  endif()

  CHECK_CXX_COMPILER_FLAG("-mprefer-avx128"
    COMPILER_SUPPORTS_MPREFER_AVX128)
  if(COMPILER_SUPPORTS_MPREFER_AVX128)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mprefer-avx128 ")
  endif()

  CHECK_CXX_COMPILER_FLAG("-Werror" COMPILER_SUPPORTS_WERROR)
  if(COMPILER_SUPPORTS_WERROR)
    target_compile_options(kator_lib PUBLIC -Werror)
    target_compile_options(kator PUBLIC -Werror)
  endif()
  CHECK_CXX_COMPILER_FLAG("-Wall" COMPILER_SUPPORTS_WALL)
  if(COMPILER_SUPPORTS_WALL)
    target_compile_options(kator_lib PUBLIC -Wall)
    target_compile_options(kator PUBLIC -Wall)
  endif()
  CHECK_CXX_COMPILER_FLAG("-Wextra" COMPILER_SUPPORTS_WEXTRA)
  if(COMPILER_SUPPORTS_WEXTRA)
    target_compile_options(kator_lib PUBLIC -Wextra)
    target_compile_options(kator PUBLIC -Wextra)
  endif()
  CHECK_CXX_COMPILER_FLAG("-pedantic" COMPILER_SUPPORTS_PEDANTIC)
  if(COMPILER_SUPPORTS_PEDANTIC)
    target_compile_options(kator_lib PUBLIC -pedantic)
    target_compile_options(kator PUBLIC -pedantic)
  endif()
  if(USE_WEVERYTHING)
    CHECK_CXX_COMPILER_FLAG("-Weverything"
      COMPILER_SUPPORTS_WEVERYTHING)
    CHECK_CXX_COMPILER_FLAG("-Wno-c++98-compat"
      COMPILER_SUPPORTS_WNO98COMPAT)
    CHECK_CXX_COMPILER_FLAG("-Wno-c++98-compat-pedantic"
      COMPILER_SUPPORTS_WNO98COMPAT_PED)
    CHECK_CXX_COMPILER_FLAG("-Wno-padded" COMPILER_SUPPORTS_WNOPADDED)
    if(COMPILER_SUPPORTS_WEVERYTHING
       AND COMPILER_SUPPORTS_WNO98COMPAT
       AND COMPILER_SUPPORTS_WNO98COMPAT_PED
       AND COMPILER_SUPPORTS_WNOPADDED)
      target_compile_options(kator_lib PUBLIC
        -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded)
    endif()
  endif()
endif()

if(MSVC)
#  About warning 4224 see:
#  https://connect.microsoft.com/VisualStudio/feedback/details/586147/warning-c4
#
#  Warning 4566 complains about codepage, no practical use of it
#
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4224 /wd4566")

  target_compile_options(kator_lib PUBLIC "$<$<CONFIG:Release>:/Za /MD>")
  target_compile_options(kator_lib PUBLIC "$<$<CONFIG:Debug>:/Za /MDd>")
  target_compile_options(kator PUBLIC "$<$<CONFIG:Release>:/Za /MD>")
  target_compile_options(kator PUBLIC "$<$<CONFIG:Debug>:/Za /MDd>")
endif()

CHECK_CXX_SOURCE_RUNS("
int addit(int* restrict a, const int* restrict b) {
  a[0] = b[1];
  return a[0] + b[1];
}
int main() {
  int a[2];
  int b[2];
  a[0] = 1;a[1] = 2;
  b[0] = 1;b[1] = 2;
  return addit(a, b) & 0;
}"
  HAS_RESTRICT_KEYWORD)

CHECK_CXX_SOURCE_RUNS("
int addit(int* __restrict__ a, const int* __restrict__ b) {
  a[0] = b[1];
  return a[0] + b[1];
}
int main() {
  int a[2];
  int b[2];
  a[0] = 1;a[1] = 2;
  b[0] = 1;b[1] = 2;
  return addit(a, b) & 0;
}"
  HAS__RESTRICT__KEYWORD)

include(cmake_includes/platform_config.cmake)

if(NOT MSVC)
# This is needed to instruct GCC not to clobber the SIMD
# registers used as permanent storage
#
  if(HAS_GCC_GLOBAL_REGISTER_VARIABLE_XMM AND HAS_INTEL_128BIT_BUILTINS)
    if(HAS_GCC_GLOBAL_REGISTER_VARIABLE_YMM AND HAS_INTEL_256BIT_BUILTINS)
      if(HAS_GCC_GLOBAL_REGISTER_VARIABLE_ZMM AND HAS_INTEL_512BIT_BUILTINS)
        target_compile_options(kator_lib PUBLIC -ffixed-xmm29)
        target_compile_options(kator_lib PUBLIC -ffixed-xmm30)
        target_compile_options(kator_lib PUBLIC -ffixed-xmm31)
      else()
      target_compile_options(kator_lib PUBLIC -ffixed-xmm13)
      target_compile_options(kator_lib PUBLIC -ffixed-xmm14)
      target_compile_options(kator_lib PUBLIC -ffixed-xmm15)
      endif()
    else()
    target_compile_options(kator_lib PUBLIC -ffixed-xmm5)
    target_compile_options(kator_lib PUBLIC -ffixed-xmm6)
    target_compile_options(kator_lib PUBLIC -ffixed-xmm7)
    endif()
  endif()
endif()

configure_file(cmake_config.h.in config.h)

include_directories(src "${PROJECT_BINARY_DIR}")

enable_testing()
add_subdirectory(tests)

