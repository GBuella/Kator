

SET(TEST_SOURCES_BASIC
  move.cc
  game_state.cc
  game.cc
)

set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" )
add_subdirectory(googletest)
include_directories(googletest/include)

set(GTEST_INCLUDE_DIR googletest/include)
set(GTEST_LIBRARY googletest)
set(GTEST_MAIN_LIBRARY googletest)

find_package(GTest REQUIRED)
add_executable(kator_gtest main.cc ${TEST_SOURCES_BASIC} $<TARGET_OBJECTS:kator_lib>)
target_include_directories(kator_gtest PRIVATE ${GTEST_INCLUDE_DIRS})
target_link_libraries(kator_gtest gtest_main)

if(COMPILER_SUPPORTS_WALL)
  target_compile_options(kator_gtest PUBLIC -Wall)
endif()

if(MSVC)
  target_compile_options(kator_gtest PUBLIC "$<$<CONFIG:Release>:/Za /MD>")
  target_compile_options(kator_gtest PUBLIC "$<$<CONFIG:Debug>:/Za /MDd>")
else()
  target_compile_options(kator_gtest PUBLIC "${STANDARD_FLAG}")
endif()

GTEST_ADD_TESTS(kator_gtest "" ${TEST_SOURCES_BASIC})

include(perft_tests.cmake)
