cmake_minimum_required(VERSION 2.8.8)
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

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Even after setting CXX_STANDARD to 14, cmake still doesn't
# compile the feature tests with the c++14 flag
#
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
CHECK_CXX_COMPILER_FLAG("-std=c++0y" COMPILER_SUPPORTS_CXX0X)
CHECK_CXX_COMPILER_FLAG("/Qstd=c++0y" COMPILER_SUPPORTS_CXXQ0X)
CHECK_CXX_COMPILER_FLAG("/Qstd=c++14" COMPILER_SUPPORTS_CXXQ14)
if(COMPILER_SUPPORTS_CXX14)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
elseif(COMPILER_SUPPORTS_CXX0Y)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0y")
elseif(COMPILER_SUPPORTS_CXXQ0Y)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Qstd=c++0y")
elseif(COMPILER_SUPPORTS_CXXQ14)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Qstd=c++14")
endif()

CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCHNATIVE)
if(COMPILER_SUPPORTS_MARCHNATIVE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -march=native ")
endif()
CHECK_CXX_COMPILER_FLAG("-mno-vzeroupper" COMPILER_SUPPORTS_MNO_VZEROUPPER)
if(COMPILER_SUPPORTS_MNO_VZEROUPPER)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -mno-vzeroupper ")
endif()
CHECK_CXX_COMPILER_FLAG("-flax-vector-conversions" COMPILER_SUPPORTS_FLAX_VECTOR_CONVERSIONS)
if(COMPILER_SUPPORTS_FLAX_VECTOR_CONVERSIONS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -flax-vector-conversions ")
endif()
CHECK_CXX_COMPILER_FLAG("-mdynamic-no-pic" COMPILER_SUPPORTS_MDYNAMIC_NO_PIC)
if(COMPILER_SUPPORTS_MDYNAMIC_NO_PIC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mdynamic-no-pic ")
endif()

CHECK_CXX_COMPILER_FLAG("-mprefer-avx128" COMPILER_SUPPORTS_MPREFER_AVX128)
if(COMPILER_SUPPORTS_MPREFER_AVX128)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mprefer-avx128 ")
endif()


CHECK_CXX_COMPILER_FLAG("-Wall" COMPILER_SUPPORTS_WALL)
if(COMPILER_SUPPORTS_WALL)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()
CHECK_CXX_COMPILER_FLAG("-Wextra" COMPILER_SUPPORTS_WALL)
if(COMPILER_SUPPORTS_WALL)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
endif()
CHECK_CXX_COMPILER_FLAG("-pedantic" COMPILER_SUPPORTS_WALL)
if(COMPILER_SUPPORTS_WALL)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
endif()
if(USE_WEVERYTHING)
  CHECK_CXX_COMPILER_FLAG("-Weverything" COMPILER_SUPPORTS_WEVERYTHING)
  CHECK_CXX_COMPILER_FLAG("-Wno-c++98-compat" COMPILER_SUPPORTS_WNO98COMPAT)
  CHECK_CXX_COMPILER_FLAG("-Wno-c++98-compat-pedantic" COMPILER_SUPPORTS_WNO98COMPAT_PED)
  CHECK_CXX_COMPILER_FLAG("-Wno-padded" COMPILER_SUPPORTS_WNOPADDED)
  if(COMPILER_SUPPORTS_WEVERYTHING
     AND COMPILER_SUPPORTS_WNO98COMPAT
     AND COMPILER_SUPPORTS_WNO98COMPAT_PED
     AND COMPILER_SUPPORTS_WNOPADDED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded")
  endif()
endif()
CHECK_CXX_COMPILER_FLAG("-Werror" COMPILER_SUPPORTS_WERROR)
if(COMPILER_SUPPORTS_WERROR)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
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

# This is needed to instruct GCC not to clobber the SIMD
# registers used as permanent storage
#
if(HAS_GCC_GLOBAL_REGISTER_VARIABLE_XMM AND HAS_INTEL_128BIT_BUILTINS)
  if(HAS_GCC_GLOBAL_REGISTER_VARIABLE_YMM AND HAS_INTEL_256BIT_BUILTINS)
    if(HAS_GCC_GLOBAL_REGISTER_VARIABLE_ZMM AND HAS_INTEL_512BIT_BUILTINS)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffixed-xmm29")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffixed-xmm30")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffixed-xmm31")
    else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffixed-xmm13")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffixed-xmm14")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffixed-xmm15")
    endif()
  else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffixed-xmm5")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffixed-xmm6")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffixed-xmm7")
  endif()
endif()

configure_file(cmake_config.h.in config.h)

include_directories(src "${PROJECT_BINARY_DIR}")

ADD_LIBRARY(kator_lib ${SOURCES})
ADD_EXECUTABLE(kator src/main.cc)
TARGET_LINK_LIBRARIES(kator kator_lib)

include(cmake_includes/tests.cmake)

ADD_CUSTOM_TARGET(debug
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target kator
  COMMENT "Switch CMAKE_BUILD_TYPE to Debug"
)

ADD_CUSTOM_TARGET(release
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target kator
  COMMENT "Switch CMAKE_BUILD_TYPE to Release"
)

