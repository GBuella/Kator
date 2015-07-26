
#include "kator.h"

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <memory>
#include <future>
#include <mutex>

#include "str.h"
#include "chess/move_list.h"
#include "tests.h"
#include "chess/game_state.h"
#include "chess/game.h"
#include "engine/engine.h"

using ::std::string;
using ::std::endl;
using ::std::unique_ptr;

namespace kator
{

conf::conf():
  move_notation(chess::move_notation::SAN),
            /* default move notation for printing move
               it is reset to coordination notation in xboard mode
            */

  main_hash_size(22),
            /* default main hash table size ( 2^23 entries )
                    - entries overwritten only with
                      entries with greater depth
            */

  aux_hash_size(21),
            /* default auxiliary hash table size
                    - entries always overwritten */

  analyze_hash_size(22),
            /* default size of hash table used
                during strict analyze search
                - entries overwritten only with
                  entries with greater depth
            */

  analyze_aux_hash_size(21),
            /* default auxiliary hash table size for analyze search
                    - entries always overwritten
            */

  book_type(book::book_type::builtin),
            /* use the builtin book by default */

  use_unicode(false)

  {}

namespace
{

class kator_implementation final : public kator
{

std::ostream& output;
std::ostream& output_error;
unique_ptr<book::book> book;
unique_ptr<chess::game> game;
unique_ptr<engine::engine> engine;
struct conf conf;

std::stringstream* input;
bool xboard_mode = false;
bool uci_mode = false;
bool computer_plays = false;
chess::real_player computer_side = chess::black;
std::mutex input_mutex;
std::mutex output_mutex;

public:

kator_implementation(unique_ptr<book::book> initial_book,
                     unique_ptr<engine::engine> CTORengine,
                     unique_ptr<chess::game> CTORgame,
                     const struct conf& CTORconf,
                     std::ostream& output_stream,
                     std::ostream& error_stream):
  output(output_stream),
  output_error(error_stream),
  book(std::move(initial_book)),
  game(std::move(CTORgame)),
  engine(std::move(CTORengine)),
  conf(CTORconf)
{
  engine->set_sub_result_callback([&](const engine::result& result)
  {
    print_search_sub_result(result);
  });
  engine->set_final_result_callback([&](const engine::result&)
  {
    (void)0;
  });
  engine->set_fixed_result_callback([&](const engine::result& result)
  {
    print_fix_depth_search_final_result(result);
  });
  (void)uci_mode;
  (void)book;
}

private:

unsigned get_uint(unsigned min, unsigned max)
{
  string str_num;

  if (!(*input >> str_num)) {
    throw std::runtime_error("missing argument");
  }
  return str::parse_uint(str_num, min, max);
}

const chess::game_state& current_state() const
{
  return game->current_state();
}

void cmd_perft()
{
  unsigned depth = get_uint(1, 128);
  unsigned long result = perft(current_state(), depth);

  output << result << endl;
}

void cmd_sperft()
{
  unsigned depth = get_uint(1, 128);
  unsigned long result = slow_perft(current_state(), depth);

  std::lock_guard<std::mutex> output_guard(output_mutex);
  output << result << endl;
}

void cmd_perfts()
{
  unsigned max_depth = get_uint(1, 128);

  for (unsigned depth = 1; depth <= max_depth; ++depth) {
    unsigned long result = perft(current_state(), depth);
    output << depth << ": " << result << endl;
  }
}

void cmd_divide()
{
  unsigned depth = get_uint(1, 128);

  for (auto move : game->legal_moves()) {
    auto child = current_state().make_move(move);

    output << current_state().print_move(move, conf.move_notation)
           << " " << perft(*child, depth - 1) << endl;
  }
}

void print_search_sub_result(const engine::result& result)
{
  (void)result;
  //std::lock_guard<std::mutex> output_guard(output_mutex);

}

void print_fix_depth_search_final_result(const engine::result& result)
{
  (void)result;
  std::lock_guard<std::mutex> output_guard(output_mutex);
  output << current_state().print_move(result.best_move, conf.move_notation)
         << endl;
  
}

void cmd_search()
{
  engine->set_max_depth(get_uint(1, 128));
  engine->start(std::make_unique<chess::game_state>(current_state()));
}

void set_xboard()
{
  xboard_mode = true;
}

void set()
{
  string key;

  *input >> key;
}

bool computer_to_move()
{
  return computer_plays && game->turn() == computer_side;
}

void cmd_echo()
{
  string str;

  if (*input >> str) {
    output << str;
    while (*input >> str) {
      output << " " << str;
    }
    output << endl;
  }
}

void cmd_print_fen()
{
  output << current_state().to_FEN() << endl;
}

void cmd_print_board()
{
  output << str::draw_game_state(current_state(), conf.use_unicode) << endl;
}

void cmd_setboard()
{
  if (!computer_to_move()) {
    game->reset(chess::game_state::parse_fen(*input));
  }
}

void cmd_force()
{
  computer_plays = false;
}

void cmd_undo()
{
  if (computer_to_move()) {
    computer_plays = false;
  }
  game->revert();
}

public:

int dispatch_command(string cmd)
{
  if      (cmd == "perft")                          cmd_perft();
  else if (cmd == "sperft")                         cmd_sperft();
  else if (cmd == "perfts")                         cmd_perfts();
  else {

    if      (cmd == "divide")                       cmd_divide();
    else if (cmd == "xboard")                       set_xboard();
    else if (cmd == "set")                          set();
    else if (cmd == "printfen")                     cmd_print_fen();
    else if (cmd == "printboard")                   cmd_print_board();
    else if (cmd == "setboard")                     cmd_setboard();
    else if (cmd == "force")                        cmd_force();
    else if (cmd == "undo")                         cmd_undo();
    else if (cmd == "echo" or cmd == "ping")        cmd_echo();
    else if (cmd == "search")                       cmd_search();
    else                                            return -1;
  }

  return 0;
}

int dispatch_move(string arg)
{
  try {
    chess::move move = current_state().parse_move(arg);

    if (!computer_to_move()) {
      game->advance(move);
    }
    return 0;
  }
  catch (...) {
    return -1;
  }
}


void command_loop(std::istream& input_stream)
{
  std::lock_guard<std::mutex> guard(input_mutex);
  string line;

  while (std::getline(input_stream, line)) {
    std::stringstream line_stream(line);
    input = &line_stream;
    string command;

    if (!(*input >> command)) {
      continue;
    }
    if (command == "q" or command == "quit" or command == "exit") {
      return;
    }
    try {
      std::lock_guard<std::mutex> output_guard(output_mutex);

      if (dispatch_command(command) != 0) {
        if (dispatch_move(command) != 0) {
          output_error << "unkown command" << endl;
        }
      }
    }
    catch (const std::exception& e) {
      output_error << e.what() << endl;
    }
  }
}

~kator_implementation()
{
}

}; /* class kator_implementation */

} /* anonymous namespace */

unique_ptr<kator> kator::create(unique_ptr<book::book> initial_book,
                                unique_ptr<engine::engine> CTORengine,
                                unique_ptr<chess::game> CTORgame,
                                const struct conf& CTORconf,
                                std::ostream& output_stream,
                                std::ostream& error_stream)
{
  return std::make_unique<kator_implementation>(std::move(initial_book),
                                                std::move(CTORengine),
                                                std::move(CTORgame),
                                                CTORconf,
                                                output_stream,
                                                error_stream);
}

} /* namespace kator */
