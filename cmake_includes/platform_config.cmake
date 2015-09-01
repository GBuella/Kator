
set(orig_cmake_required_flags "${CMAKE_REQUIRED_FLAGS}")
set(CMAKE_REQUIRED_FLAGS "${orig_cmake_required_flags} ${KATOR_STANDARD_FLAG}")

if(NOT MSVC)

  include(cmake_includes/gcc.cmake)

else()

  CHECK_CXX_COMPILER_FLAG("/arch:AVX" KATOR_MSVC_SUPPORTS_AVX_FLAG)
  CHECK_CXX_COMPILER_FLAG("/arch:AVX2" KATOR_MSVC_SUPPORTS_AVX2_FLAG)

  if(KATOR_MSVC_SUPPORTS_AVX_FLAG AND KATOR_MSVC_USE_AVX_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX")
    set(orig_cmake_required_flags "${orig_cmake_required_flags} /arch:AVX")
  endif()

  if(KATOR_MSVC_SUPPORTS_AVX2_FLAG AND KATOR_MSVC_USE_AVX2_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
    set(orig_cmake_required_flags "${orig_cmake_required_flags} /arch:AVX2")
  endif()

  set(CMAKE_REQUIRED_FLAGS
      "${orig_cmake_required_flags} ${KATOR_STANDARD_FLAG}")

  include(cmake_includes/msvc.cmake)

endif()

set(CMAKE_REQUIRED_FLAGS "${orig_cmake_required_flags} ${KATOR_STANDARD_FLAG}")

include(cmake_includes/x64.cmake)

set(CMAKE_REQUIRED_FLAGS "${orig_cmake_required_flags} ${KATOR_STANDARD_FLAG}")

CHECK_CXX_SOURCE_RUNS("
#undef __STRICT_ANSI__
#include <cstdio>
#include <unistd.h>
int main() {
if (isatty(fileno(stdin))) {
  (void)setvbuf(stdout, NULL, _IOLBF, 0x1000);
}
return 0;
}"
  KATOR_CAN_DO_SETVBUF)


set(CMAKE_REQUIRED_FLAGS "${orig_cmake_required_flags}")
