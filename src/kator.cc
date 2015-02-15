
#include "kator.h"

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <memory>

#include "str.h"
#include "chess/move_list.h"
#include "tests.h"
#include "chess/game.h"

using ::std::string;
using ::std::endl;
using ::std::unique_ptr;

namespace kator
{

namespace conf
{
  chess::move_notation move_notation = chess::move_notation::SAN;
            /* default move notation for printing move
               it is reset to coordination notation in xboard mode
            */

  unsigned main_hash_size = 22;
            /* default main hash table size ( 2^23 entries )
                    - entries overwritten only with
                      entries with greater depth
            */

  unsigned aux_hash_size = 21;
            /* default auxiliary hash table size
                    - entries always overwritten */

  unsigned analyze_hash_size = 22;
            /* default size of hash table used
                during strict analyze search
                - entries overwritten only with
                  entries with greater depth
            */

  unsigned analyze_aux_hash_size = 21;
            /* default auxiliary hash table size for analyze search
                    - entries always overwritten
            */

  book::book_type book_type = book::book_type::builtin;
            /* use the builtin book by default */

  bool use_unicode = false;

  char* book_path = nullptr;

}

namespace
{

class kator : protected chess::game
{

public:
std::stringstream *input;

private:
bool xboard_mode = false;
bool uci_mode = false;
bool computer_plays = false;
chess::real_player computer_side = chess::black;
std::ostream& output;
std::unique_ptr<book::book> book;

public:


kator(std::ostream& output_stream,
       std::unique_ptr<book::book>& initial_book):
  input(nullptr),
  output(output_stream),
  book(std::move(initial_book))
{
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

  for (auto move : legal_moves()) {
    auto child = current_state().make_move(move);

    output << current_state().print_move(move, conf::move_notation)
           << " " << perft(*child, depth - 1) << endl;
  }
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
  return computer_plays && turn() == computer_side;
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
  output << str::draw_game_state(current_state(), conf::use_unicode) << endl;
}

void cmd_setboard()
{
  if (!computer_to_move()) {
    reset(*input);
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
  revert();
}

public:

int dispatch_command(string cmd)
{
  if      (cmd == "perft")                          cmd_perft();
  else if (cmd == "sperft")                         cmd_sperft();
  else if (cmd == "perfts")                         cmd_perfts();
  else if (cmd == "divide")                         cmd_divide();
  else if (cmd == "xboard")                         set_xboard();
  else if (cmd == "set")                            set();
  else if (cmd == "printfen")                       cmd_print_fen();
  else if (cmd == "printboard")                     cmd_print_board();
  else if (cmd == "setboard")                       cmd_setboard();
  else if (cmd == "force")                          cmd_force();
  else if (cmd == "undo")                           cmd_undo();
  else if (cmd == "echo" or cmd == "ping")          cmd_echo();
  else                                              return -1;

  return 0;
}

int dispatch_move(string arg)
{
  try {
    chess::move move = current_state().parse_move(arg);

    if (!computer_to_move()) {
      advance(move);
    }
    return 0;
  }
  catch (...) {
    return -1;
  }
}

}; /* class kator */

} /* anonymous namespace */

void command_loop(std::unique_ptr<book::book>& initial_book,
                  std::istream& input_stream,
                  std::ostream& output_stream,
                  std::ostream& error_stream)
{
  string line;
  class kator protocol(output_stream, initial_book);

  while (std::getline(input_stream, line)) {
    std::stringstream line_stream(line);
    protocol.input = &line_stream;
    string command;

    if (!(*protocol.input >> command)) {
      continue;
    }
    if (command == "q" or command == "quit" or command == "exit") {
      return;
    }
    try {
      if (protocol.dispatch_command(command) != 0) {
        if (protocol.dispatch_move(command) != 0) {
          error_stream << "unkown command" << endl;
        }
      }
    }
    catch (const std::exception& e) {
      error_stream << e.what() << endl;
    }
  }
}

} /* namespace kator */
