
#include "book_fen.h"
#include "chess/game_state.h"
#include "chess/move.h"

#include <istream>
#include <sstream>
#include <fstream>
#include <memory>

using ::std::string;
using ::std::unique_ptr;

namespace kator
{
namespace book
{

namespace
{

string get_fen_prefix(std::istream& stream)
{
  string token;
  string result;

  while (stream >> token) {
    if (token == ";") {
      break;
    }
    result += token;
    result.push_back(' ');
  }
  return result;
}

void
add_entry(const string& line,
          std::unordered_map<string, std::vector<chess::move>>& data)
{
  std::istringstream iss(line);
  unique_ptr<chess::game_state> state =
    chess::game_state::parse_fen(get_fen_prefix(iss));
  string move_str;
  std::vector<chess::move> moves; 

  while (iss >> move_str) {
    moves.push_back(state->parse_move(move_str));
  }
  auto existing = data.find(state->to_FEN());
  if (existing == data.end()) {
    data[state->to_FEN()] = moves;
  }
  else {
    existing->second.insert(existing->second.end(), moves.begin(), moves.end());
  }
}

}

book_fen::book_fen(string path)
{
  string line;
  std::ifstream file;

  file.open(path);
  while (std::getline(file, line)) {
    if (line.length() > 0) {
      add_entry(line, data);
    }
  }
  file.close();
}

book_fen::book_fen(const std::vector<std::string>& lines)
{
  for (auto const & line : lines) {
    add_entry(line, data);
  }
}

std::vector<chess::move> book_fen::get_moves(const chess::game_state& state)
{
  std::vector<chess::move> moves = data.at(state.to_FEN());
  if (state.can_flip) {
    for (auto move : data.at(state.flipped_to_FEN())) {
      moves.push_back(move.flipped());
    }
  }
  return moves;
}

} /* namespace kator::book */
} /* namespace kator */

