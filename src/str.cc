
#include "str.h"
#include "chess/game_state.h"

#include <cctype>
#include <cstring>
#include <cstdlib>
#include <string>
#include <sstream>
#include <regex>
#include <stdexcept>
#include <limits>
#include <algorithm>

using ::std::string;

namespace kator
{
namespace str
{

const char u_alpha[] = "\U0001d6fc";
const char u_beta[] = "\U0001d6fd";
const char u_mu[] = "\U0001d707";

bool has_only(const string& str, const char allowed[]) noexcept
{
  return str.length() == std::strspn(str.c_str(), allowed);
}

bool has_duplicates(const string& str) noexcept
{
  bool contains[std::numeric_limits<unsigned char>::max() + 1];

  std::memset(contains, 0, sizeof(contains));
  for (char c : str) {
    unsigned char uc = static_cast<unsigned char>(c);

    if (contains[uc]) {
      return true;
    }
    contains[uc] = true;
  }
  return false;
}

bool contains(const string& str, char c) noexcept
{
  return str.find(c) != string::npos;
}

unsigned parse_uint(const string& str, unsigned min, unsigned max)
{
  unsigned long value;
  char* endp;

  /* Standard library supplied with MinGW on cygwin
     lacks std::stoul, thus using C strtoul.
     ( 2015 January )
   */

  value = strtoul(str.data(), &endp, 10);
  if (endp != str.data() + str.length()) {
    throw std::invalid_argument(str);
  }
  if (value < min or value > max) {
    throw std::out_of_range(str);
  }
  return static_cast<unsigned>(value);
}

const char unicode_table::upper_left[] = "\U0000250f";
const char unicode_table::upper_right[] = "\U00002513";
const char unicode_table::lower_left[] = "\U00002517";
const char unicode_table::lower_right[] = "\U0000251b";
const char unicode_table::horizontal[] = "\U00002501";
const char unicode_table::vertical[] = "\U00002503";
const char unicode_table::vertical_horizontal[] = "\U0000254b";
const char unicode_table::horizontal_down[] = "\U00002533";
const char unicode_table::horizontal_up[] = "\U0000253b";
const char unicode_table::vertical_right[] = "\U00002523";
const char unicode_table::vertical_left[] = "\U0000252b";

string unicode_table::upper_border() const noexcept
{
  std::stringstream row;

  row << upper_left;
  for (size_t i = 1; i < width; ++i) {
    row << horizontal << horizontal_down;
  }
  row << horizontal << upper_right;
  return row.str();
}

string unicode_table::lower_border() const noexcept
{
  std::stringstream row;

  row << lower_left;
  for (size_t i = 1; i < width; ++i) {
    row << horizontal << horizontal_up;
  }
  row << horizontal << lower_right;
  return row.str();
}

string unicode_table::row_separator() const noexcept
{
  std::stringstream row;
  
  row << vertical_right;
  for (size_t i = 1; i < width; ++i) {
    row << horizontal << vertical_horizontal;
  }
  row << horizontal << vertical_left;
  return row.str();
}

namespace // helper functions for printing board
{
  string upper_border(bool use_figurines)
  {
    string result = use_figurines ? "   A B C D E F G H\n" : "  ABCDEFGH\n";

    if (use_figurines) {
      result += "  " + unicode_table(8).upper_border() + "\n";
    }
    return result;
  }

  string lower_border(bool use_figurines)
  {
    string result;

    if (use_figurines) {
      result = "  " + unicode_table(8).lower_border() + "\n" +
        "   A B C D E F G H\n";
    }
    else {
      result = "  ABCDEFGH\n";
    }
    return result;
  }

  string board_left_side(chess::rank rank, bool use_figurines)
  {
    string result;

    result = rank.to_str(chess::player::to_move) + " ";
    if (use_figurines) {
      result += unicode_table::vertical;
    }
    return result;
  }

  string board_right_side(chess::rank rank, bool use_figurines)
  {
    string result;

    result = " " + rank.to_str(chess::player::to_move) + "\n";
    if (use_figurines && rank != chess::rank_1) {
      result += "  " + unicode_table(8).row_separator() + "\n";
    }
    return result;
  }

} // end of anonym namespace - board print helpers

string draw_game_state(const chess::game_state& state, bool use_figurines)
{
  string result = upper_border(use_figurines);

  for (auto rank : chess::ranks_8_to_1) {
    result += board_left_side(rank, use_figurines);
    for (auto file : chess::files_a_to_h) {
      result += state.piece_at(rank, file).
                  to_str(state.whites_view, use_figurines);
      if (use_figurines) {
        result += unicode_table::vertical;
      }
    }
    result += board_right_side(rank, use_figurines);
  }
  result += lower_border(use_figurines);
  return result;
}

} /* namespace str */
} /* namespace kator */

