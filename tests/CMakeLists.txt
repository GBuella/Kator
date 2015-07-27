

SET(TEST_SOURCES_BASIC
  move.cc
  game_state.cc
  game.cc
)

add_subdirectory(googletest)
include_directories(googletest/include)

set(GTEST_INCLUDE_DIR googletest/include)
set(GTEST_LIBRARY googletest)
set(GTEST_MAIN_LIBRARY googletest)

find_package(GTest REQUIRED)
add_executable(kator_gtest main.cc ${TEST_SOURCES_BASIC})
target_include_directories(kator_gtest PRIVATE ${GTEST_INCLUDE_DIRS})
target_link_libraries(kator_gtest gtest_main kator_lib)

GTEST_ADD_TESTS(kator_gtest "" ${TEST_SOURCES_BASIC})

include(perft_tests.cmake)
