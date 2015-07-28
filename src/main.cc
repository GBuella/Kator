
#include "config.h"

#ifdef CAN_DO_SETVBUF
#  if defined(CYGWIN) && defined(__STRICT_ANSI__)
#    undef __STRICT_ANSI__
#    include <stdio.h>
#    include <unistd.h>
#    define __STRICT_ANSI__
#  else
#    include <cstdio>
#    include <unistd.h>
#  endif
#endif

#include <chrono>
#include <cstdlib>
#include <csignal>
#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>

#include "platform_low_level.h"
#include "chess/chess.h"
#include "kator.h"
#include "chess/game.h"
#include "tests.h"
#include "engine/eval.h"
#include "engine/engine.h"
#include "engine/search.h"

#ifdef ANDROID_NDK_PROFILING
extern "C" void monstartup(const char *);
extern "C" void moncleanup(void);
#endif

static const char kator_version_string[] =
  "Kator chess engine\n2015 Gabor Buella\n";

using ::std::string;
using ::std::unique_ptr;

typedef std::chrono::time_point<std::chrono::high_resolution_clock> time_point;

static time_point* start_time = nullptr;
static bool timing = false;
static const char* progname;
static void startup();
static void onexit();
[[noreturn]] static void usage(int status);
static void process_args(char **arg);
static unique_ptr<kator::book::book> open_initial_book();
static kator::conf conf;

static void setup_testing(char**&);
static bool is_testing_mode = false;
static string test_path;
static int test_run(unique_ptr<kator::book::book>,
                    unique_ptr<kator::engine::engine>,
                    unique_ptr<kator::chess::game>);

int main(int, char **argv)
{
  using ::std::move;

  startup();
  process_args(argv);
  auto initial_book = open_initial_book();
  auto game = kator::chess::game::create();
  auto engine =
    kator::engine::engine::create(kator::engine::search_factory::create());

  if (is_testing_mode) {
    return test_run(move(initial_book), move(engine), move(game));
  }
  else {
    auto app = kator::kator::create(move(initial_book),
                                    move(engine), move(game),
                                    conf, std::cout, std::cerr);
    app->command_loop(std::cin);

    return EXIT_SUCCESS;
  }
}

static void startup()
{
# ifdef CAN_DO_SETVBUF
  if (isatty(fileno(stdin))) {
    (void)setvbuf(stdout, NULL, _IOLBF, 0x1000);
  }
# endif

# ifdef ANDROID_NDK_PROFILING
  monstartup("kator");
# endif

  if (atexit(onexit) != 0) {
    exit(EXIT_FAILURE);
  }

  ::kator::chess::bitboard_lookup_tables_init();
  ::kator::engine::position_value::default_initialize_lookup_tables();
}

static void onexit()
{
#ifdef ANDROID_NDK_PROFILING
  moncleanup();
#endif

  if (timing) {
    using namespace std::chrono;

    ::time_point end = high_resolution_clock::now();
    duration<double> time_span =
        duration_cast<duration<double>>(end - *start_time);
    std::cerr << time_span.count() << std::endl;
  }
  //log_close();
}

static void setup_timing()
{
  timing = true;
  start_time = new time_point(std::chrono::high_resolution_clock::now());
}

static void setup_book(char**& arg, kator::book::book_type type)
{
  using namespace kator::book;

  if (arg[1] == nullptr or conf.book_type != book_type::builtin) {
    usage(EXIT_FAILURE);
  }
  conf.book_type = type;
  ++arg;
  conf.book_path = std::string(*arg);
}

static void setup_polyglot_book(char**& arg)
{
  setup_book(arg, kator::book::book_type::polyglot);
}

static void setup_fen_book(char**& arg)
{
  setup_book(arg, kator::book::book_type::fen);
}

static void setup_nobook()
{
  using namespace kator::book;

  if (conf.book_type != book_type::builtin) {
    usage(EXIT_FAILURE);
  }
  conf.book_type = book_type::empty;
}

static void print_version_banner()
{
  std::cout << kator_version_string
            << "version "
            << kator::kator_version_major
            << "." << kator::kator_version_minor << " dev\n";
}

static void print_version()
{
  print_version_banner();
  exit(0);
}

static void print_version_verbose()
{
  print_version_banner();
  std::cout << "\nBuild date: " __DATE__ " " __TIME__ "\n"
#ifdef CMAKE_VERSION
      << "Built with cmake " CMAKE_VERSION "\n"
      << "Compiler: " CMAKE_CXX_COMPILER_ID
#  ifdef CMAKE_COMPILER_IS_GNUCXX
      << " g++"
#  endif
#  ifdef CMAKE_CXX_COMPILER_VERSION
      << " " CMAKE_CXX_COMPILER_VERSION
#  endif
      << "\n"
      << "Compiler flags: " CMAKE_CXX_FLAGS
#  ifdef CMAKE_BUILD_TYPE
     << ( (string(CMAKE_BUILD_TYPE) == string("Release")) ?
           (" " CMAKE_CXX_FLAGS_RELEASE)
           :
           ( (string(CMAKE_BUILD_TYPE) == string("Debug")) ?
             (" " CMAKE_CXX_FLAGS_DEBUG)
             :
             ""
           )
     )
#  endif
       << "\n"
#endif
       << "Build configuration:\n"
#ifdef USE_ALIGNAS_64
       << "USE_ALIGNAS_64\n"
#endif
#ifdef USE_BOARD_VECTOR_64
       << "USE_BOARD_VECTOR_64\n"
#endif
#ifdef USE_PIECE_MAP_VECTOR
       << "USE_PIECE_MAP_VECTOR\n"
#endif
#ifdef USE_PEXT_BITBOARD
       << "USE_PEXT_BITBOARD\n"
#endif
#ifdef SYSTEM_POPCNT64
       << "SYSTEM_POPCNT64\n"
#endif
#ifdef SYSTEM_GET_LSB64
       << "SYSTEM_GET_LSB64\n"
#endif
#ifdef SYSTEM_RESET_LSB64
       << "SYSTEM_RESET_LSB64\n"
#endif
#ifdef SYSTEM_BYTESWAP64
       << "SYSTEM_BYTESWAP64\n"
#endif
#ifdef SYSTEM_CTZ64
       << "SYSTEM_CTZ64\n"
#endif
#ifdef HAS_INTEL_128BIT_BUILTINS
       << "INTEL_128BIT_BUILTINS\n"
#endif
#ifdef HAS_INTEL_256BIT_BUILTINS
       << "INTEL_256BIT_BUILTINS\n"
#endif
#ifdef HAS_INTEL_512BIT_BUILTINS
       << "INTEL_512BIT_BUILTINS\n"
#endif
#ifdef HAS_BMI2_PEXT_BITBOARD_SUPPORT
       << "HAS_BMI2_PEXT_BITBOARD_SUPPORT\n"
#endif
#ifdef HAS_GCC_GLOBAL_REGISTER_VARIABLE_XMM
       << "HAS_GCC_GLOBAL_REGISTER_VARIABLE_XMM\n"
#endif
#ifdef HAS_GCC_GLOBAL_REGISTER_VARIABLE_YMM
       << "HAS_GCC_GLOBAL_REGISTER_VARIABLE_YMM\n"
#endif
#ifdef HAS_GCC_GLOBAL_REGISTER_VARIABLE_ZMM
       << "HAS_GCC_GLOBAL_REGISTER_VARIABLE_ZMM\n"
#endif
       ;
  exit(0);
}

static void process_args(char **arg)
{
  progname = *arg;
  for (++arg; *arg != nullptr; ++arg) {
    string sarg(*arg);

    if      (sarg == "-t")                   setup_timing();
    else if (sarg == "-v")                   print_version();
    else if (sarg == "--version")            print_version_verbose();
    else if (sarg == "--book")               setup_polyglot_book(arg);
    else if (sarg == "--fenbook")            setup_fen_book(arg);
    else if (sarg == "--nobook")             setup_nobook();
    else if (sarg == "--unicode")            conf.use_unicode = true;
    else if (sarg == "--help")               usage(EXIT_SUCCESS);
    else if (sarg == "-help")                usage(EXIT_SUCCESS);
    else if (sarg == "--h")                  usage(EXIT_SUCCESS);
    else if (sarg == "-h")                   usage(EXIT_SUCCESS);
    else if (sarg == "--test_file")          setup_testing(arg);
  }
}

[[noreturn]] static void usage(int status)
{
  ((status == EXIT_SUCCESS) ? std::cout : std::cerr)
    << "kator chess engine\n"
    << "usage: " << progname << " \n" << std::endl;
  exit(status);
}

static unique_ptr<kator::book::book> open_initial_book()
{
  using kator::book::book;

  unique_ptr<book> initial_book;

  try {
    initial_book = book::open(conf.book_type, conf.book_path);
  }
  catch (...) {
    std::cerr << "Unable to open book " << conf.book_path << "\n";
    exit(EXIT_FAILURE);
  }
  return initial_book;
}

static void setup_testing(char**& arg)
{
  is_testing_mode = true;

  ++arg;
  if (*arg == nullptr) {
    usage(EXIT_FAILURE);
  }
  test_path = string(*arg);
}

static void compare_streams(std::istream& expected, std::istream& actual)
{
  string expected_token;
  string token;

  while (expected >> expected_token) {
    if (!(actual >> token)) {
      std::cerr << "Error: value: none "
                << " expected value: \"" << expected_token << "\"\n";
      exit(EXIT_FAILURE);
    }
    if (token != expected_token) {
      std::cerr << "Error: value: \"" << token << "\" "
                << " expected value: \"" << expected_token << "\"\n";
      exit(EXIT_FAILURE);
    }
  }
  if (actual >> token) {
    std::cerr << "Error: value: \"" << token << "\" "
              << " expected value: none\n";
    exit(EXIT_FAILURE);
  }
}

static bool test_is_running = false;

static void on_test_run_exit()
{
  if (test_is_running) {
    std::cerr << "Unexpected exit during test run\n";
  }
}

static int test_run(unique_ptr<kator::book::book> initial_book,
                    unique_ptr<kator::engine::engine> engine,
                    unique_ptr<kator::chess::game> game)
{
  std::ifstream input;
  std::ifstream expected_out;
  std::ifstream expected_err;
  std::stringstream mock_out;
  std::stringstream mock_err;
  unique_ptr<kator::kator> app;

  if (atexit(on_test_run_exit) != 0) {
    return EXIT_FAILURE;
  }
  try {
    input.open(test_path + ".in");
  }
  catch (...) {
    std::cerr << "Unable to open " << test_path << ".in\n";
    return EXIT_FAILURE;
  }
  try {
    expected_out.open(test_path + ".out");
  }
  catch (...) {
    std::cerr << "Unable to open " << test_path << ".out\n";
    return EXIT_FAILURE;
  }
  try {
    expected_err.open(test_path + ".err");
  }
  catch (...) {
  }
  test_is_running = true;

  app = kator::kator::create(std::move(initial_book), std::move(engine),
                             std::move(game), conf, mock_out, mock_err);
  app->command_loop(input);
  app.reset();

  test_is_running = false;
  compare_streams(expected_out, mock_out);
  compare_streams(expected_err, mock_err);
  return EXIT_SUCCESS;
}

