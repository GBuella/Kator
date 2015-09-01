cmake_minimum_required(VERSION 3.0.0)
project(kator CXX)
set(KATOR_VERSION_MAJOR 0)
set(KATOR_VERSION_MINOR 1)


include(cmake_includes/sources.cmake)

option(KATOR_USE_WEVERYTHING
    "Use the -Weverything compiler flag" OFF)

option(KATOR_USE_PEXT_BITBOARD
    "Use pext bitboard instead of old-style magic bitboards" OFF)

option(KATOR_USE_ALIGNAS_64
    "Use 64 byte alignment for the position representation" OFF)

option(KATOR_USE_BOARD_VECTOR_64
    "Use 64 byte vectors to represent the board" OFF)

option(KATOR_USE_PIECE_MAP_VECTOR
    "Use vector operations with bitboard piece maps" OFF)

if(NOT MSVC)
  option(KATOR_NO_ARCHNATIVE "Do not attempt to use the -march=native flag")
else()
  option(KATOR_MSVC_USE_AVX_FLAG "Use the /arch:AVX flag")
  option(KATOR_MSVC_USE_AVX2_FLAG "Use the /arch:AVX2 flag")
endif()

include(CheckCXXCompilerFlag)
include(CheckCXXSourceRuns)
include(CheckIncludeFiles)

ADD_LIBRARY(kator_common OBJECT ${KATOR_COMMON_SOURCES})
ADD_EXECUTABLE(kator src/main.cc $<TARGET_OBJECTS:kator_common>)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(KATOR_STANDARD_FLAG "")

if(NOT MSVC)

# Even after setting CXX_STANDARD to 14, cmake still doesn't
# compile the feature tests with the c++14 flag
#
  CHECK_CXX_COMPILER_FLAG("-std=c++14" KATOR_COMPILER_SUPPORTS_CXX14)
  CHECK_CXX_COMPILER_FLAG("-std=c++1y" KATOR_COMPILER_SUPPORTS_CXX1Y)
  if(KATOR_COMPILER_SUPPORTS_CXX14)
    set(KATOR_STANDARD_FLAG "-std=c++14")
  elseif(KATOR_COMPILER_SUPPORTS_CXX1Y)
    set(KATOR_STANDARD_FLAG "-std=c++1y")
  endif()

  target_compile_options(kator_common PUBLIC "${KATOR_STANDARD_FLAG}")
  target_compile_options(kator PUBLIC "${KATOR_STANDARD_FLAG}")


endif()

if(MSVC)
#  About warning 4224 see:
#  https://connect.microsoft.com/VisualStudio/feedback/details/586147/warning-c4
#
#  Warning 4566 complains about codepage, no practical use of it
#
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4224 /wd4566")

# /Za  brings MSVC closer to ISO
  target_compile_options(kator_common PUBLIC "$<$<CONFIG:Release>:/Za /MD>")
  target_compile_options(kator_common PUBLIC "$<$<CONFIG:Debug>:/Za /MDd>")
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
  KATOR_HAS_RESTRICT_KEYWORD)

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
  KATOR_HAS__RESTRICT__KEYWORD)

include(cmake_includes/platform_config.cmake)

if(KATOR_COMPILER_SUPPORTS_WERROR)
  target_compile_options(kator_common PUBLIC -Werror)
  target_compile_options(kator PUBLIC -Werror)
endif()
if(KATOR_COMPILER_SUPPORTS_WALL)
  target_compile_options(kator_common PUBLIC -Wall)
  target_compile_options(kator PUBLIC -Wall)
endif()
if(KATOR_COMPILER_SUPPORTS_WEXTRA)
  target_compile_options(kator_common PUBLIC -Wextra)
  target_compile_options(kator PUBLIC -Wextra)
endif()
if(KATOR_COMPILER_SUPPORTS_PEDANTIC)
  target_compile_options(kator_common PUBLIC -pedantic)
  target_compile_options(kator PUBLIC -pedantic)
endif()

# clang specific -Weverything flag, can result in a lot of warnings
#
if(KATOR_USE_WEVERYTHING)
  CHECK_CXX_COMPILER_FLAG("-Weverything"
    KATOR_COMPILER_SUPPORTS_WEVERYTHING)
  CHECK_CXX_COMPILER_FLAG("-Wno-c++98-compat"
    KATOR_COMPILER_SUPPORTS_WNO98COMPAT)
  CHECK_CXX_COMPILER_FLAG("-Wno-c++98-compat-pedantic"
    KATOR_COMPILER_SUPPORTS_WNO98COMPAT_PED)
  CHECK_CXX_COMPILER_FLAG("-Wno-padded" KATOR_COMPILER_SUPPORTS_WNOPADDED)
  if(KATOR_COMPILER_SUPPORTS_WEVERYTHING
     AND KATOR_COMPILER_SUPPORTS_WNO98COMPAT
     AND KATOR_COMPILER_SUPPORTS_WNO98COMPAT_PED
     AND KATOR_COMPILER_SUPPORTS_WNOPADDED)
    target_compile_options(kator_common PUBLIC
      -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded)
  endif()
endif()

# This is needed to instruct GCC not to clobber the SIMD
# registers used as permanent storage
#
if(KATOR_HAS_GCC_GLOBAL_REGISTER_VARIABLE_XMM
   AND KATOR_HAS_X64_128BIT_BUILTINS)
  if(KATOR_HAS_GCC_GLOBAL_REGISTER_VARIABLE_YMM
     AND KATOR_HAS_X64_256BIT_BUILTINS)
    if(KATOR_HAS_GCC_GLOBAL_REGISTER_VARIABLE_ZMM
       AND KATOR_HAS_X64_512BIT_BUILTINS)
      target_compile_options(kator_common PUBLIC -ffixed-xmm29)
      target_compile_options(kator_common PUBLIC -ffixed-xmm30)
      target_compile_options(kator_common PUBLIC -ffixed-xmm31)
    else()
    target_compile_options(kator_common PUBLIC -ffixed-xmm13)
    target_compile_options(kator_common PUBLIC -ffixed-xmm14)
    target_compile_options(kator_common PUBLIC -ffixed-xmm15)
    endif()
  else()
  target_compile_options(kator_common PUBLIC -ffixed-xmm5)
  target_compile_options(kator_common PUBLIC -ffixed-xmm6)
  target_compile_options(kator_common PUBLIC -ffixed-xmm7)
  endif()
endif()


configure_file(cmake_config.h.in config.h)

include_directories(src "${PROJECT_BINARY_DIR}")

enable_testing()
add_subdirectory(tests)

