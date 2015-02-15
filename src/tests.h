
#ifndef KATOR_TESTS_H
#define KATOR_TESTS_H

/* Declaration of some testing routines. {{{
   The perft function walks the game-tree without make_move in the leafs,
   while the slow_perft function does a make_move for each leaf.
   The faster perft routine is sufficient for validating the move generator.

   Also defined here is a minimal assertion library.
   Test frameworks out there refuse
   to build with warnings enabled and/or require installation
   on each box this code is run on.
   This approach actually seems easier.
   Kator is a very little application anyways.

   Reason d'etre of the test_filename variable
   is avoiding builds containing strings such as
   "/Users/smiths/source/taltos/tests/game_state.cc"
}}}*/

#include <iostream>
#include <cstdlib>

#include "config.h"
#include "chess/chess.h"

namespace kator
{

unsigned long perft(const chess::position&, unsigned depth);
unsigned long slow_perft(const chess::position&, unsigned depth);
unsigned long perft(const chess::game_state&, unsigned depth);
unsigned long slow_perft(const chess::game_state&, unsigned depth);

#ifndef KATOR_BUILD_WITHOUT_TESTS

[[noreturn]] void test_FEN();
[[noreturn]] void test_move();
[[noreturn]] void test_print_move();
[[noreturn]] void test_parse_move();
[[noreturn]] void test_move_counters();

#define ASSERT_NO_THROW(expression) \
  try { \
    expression; \
  } \
  catch (const std::exception& exc) { \
    std::cerr << "Fail: " << test_filename << ":" << __LINE__ << "\n"; \
    std::cerr << " " << #expression << " throws\n"; \
    std::cerr << " " << exc.what() << "\n"; \
    exit(EXIT_FAILURE); \
  } \
  catch (...) { \
    std::cerr << "Fail: " << test_filename << ":" << __LINE__ << "\n"; \
    std::cerr << " " << #expression << " throws\n"; \
    exit(EXIT_FAILURE); \
  } 

#define ASSERT_THROW(expression, exception_type) \
  do { \
    try { \
      expression; \
    } \
    catch (const exception_type&) { \
      break; \
    } \
    catch (const std::exception& exc) { \
      std::cerr << "Fail: " << test_filename << ":" << __LINE__ << "\n"; \
      std::cerr << " " << #expression << " throws wrong exception\n"; \
      std::cerr << " " << exc.what() << "\n"; \
      exit(EXIT_FAILURE); \
    } \
    catch (...) { \
      std::cerr << "Fail: " << test_filename << ":" << __LINE__ << "\n"; \
      std::cerr << " " << #expression << " throws wrong type of exception\n"; \
      exit(EXIT_FAILURE); \
    } \
    std::cerr << "Fail: " << test_filename << ":" << __LINE__ << "\n"; \
    std::cerr << " " << #expression << " does not throw\n"; \
    exit(EXIT_FAILURE); \
  } while (false);

#define ASSERT_EQ(expected_value, actual_value) \
    try { \
      auto minimal_test_actual_value = actual_value; \
      if ((expected_value) != (minimal_test_actual_value)) { \
        std::cerr << "Fail: " << test_filename << ":" << __LINE__ << "\n"; \
        std::cerr << " value of " <<  #actual_value << "\n" \
                  << " expected: " << expected_value << "\n" \
                  << " actual: " << minimal_test_actual_value << "\n"; \
        exit(EXIT_FAILURE); \
      } \
    } \
    catch (const std::exception& exc) { \
      std::cerr << "Fail: " << test_filename << ":" << __LINE__ << "\n"; \
      std::cerr << " " << #expected_value << ", " \
                << #actual_value << " throws exception\n"; \
      std::cerr << " " << exc.what() << "\n"; \
      exit(EXIT_FAILURE); \
    } \
    catch (...) { \
      std::cerr << "Fail: " << test_filename << ":" << __LINE__ << "\n"; \
      std::cerr << " " << #expected_value << ", " \
                << #actual_value << " throws exception\n"; \
      exit(EXIT_FAILURE); \
    } \

#define ASSERT_TRUE(expression) \
  try { \
    if (not (expression)) { \
      std::cerr << "Fail: " << test_filename << ":" << __LINE__ << "\n"; \
      std::cerr << " " << #expression << " is not true\n"; \
      exit(EXIT_FAILURE); \
    } \
  } \
  catch (...) { \
    std::cerr << "Fail: " << test_filename << ":" << __LINE__ << "\n"; \
    std::cerr << " " << #expression << " throws\n"; \
    exit(EXIT_FAILURE); \
  } 

#define ASSERT_FALSE(expression) \
  try { \
    if (expression) { \
      std::cerr << "Fail: " << test_filename << ":" << __LINE__ << "\n"; \
      std::cerr << " " << #expression << " is not false\n"; \
      exit(EXIT_FAILURE); \
    } \
  } \
  catch (...) { \
    std::cerr << "Fail: " << test_filename << ":" << __LINE__ << "\n"; \
    std::cerr << " " << #expression << " throws\n"; \
    exit(EXIT_FAILURE); \
  } 

#endif // defined(KATOR_BUILD_WITHOUT_TESTS)

} /* namespace kator */

#endif // !defined(KATOR_TESTS_H)

