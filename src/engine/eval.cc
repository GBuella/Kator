
#include <sstream>
#include <fstream>
#include <regex>
#include <map>
#include <string>
#include <cstdlib>
#include <climits>

#include "eval.h"
#include "chess/position.h"

using namespace ::kator::chess; // should look for a way to get rid of this

using ::std::string;

namespace kator
{

namespace engine
{

std::array<short, chess::piece::array_size> position_value::piece_values;
chess::move::change_array_t<short> position_value::move_change_table;

namespace
{

typedef std::map<string, short> conf_t;

#include "default_values.inc"

void setup_piece_values(std::array<short, chess::piece::array_size>& values,
                        const conf_t& conf)
{
  values[pawn.index()] =   conf.at("pawn");
  values[rook.index()] =   conf.at("rook");
  values[knight.index()] = conf.at("knight");
  values[bishop.index()] = conf.at("bishop");
  values[queen.index()] =  conf.at("queen");

  for (auto piece : chess::piece::all_pieces()) {
    values[chess::opponent_of(piece).index()] = -values[piece.index()];
  }
}

void add_promotion_values(chess::move::change_array_t<short>& move_change_table,
                          chess::piece captured)
{
  auto promotion = chess::move::promotion;

  move_change_table[chess::move::change_index(queen, captured, promotion)] =
                    (position_value(queen) - position_value(pawn)).as_short();
  move_change_table[chess::move::change_index(rook, captured, promotion)] =
                    (position_value(rook) - position_value(pawn)).as_short();
  move_change_table[chess::move::change_index(knight, captured, promotion)] =
                    (position_value(knight) - position_value(pawn)).as_short();
  move_change_table[chess::move::change_index(bishop, captured, promotion)] =
                    (position_value(bishop) - position_value(pawn)).as_short();
}

void setup_move_changes(chess::move::change_array_t<short>& move_change_table)
{
  for (auto captured : chess::piece::all_pieces()) {
    for (auto result : chess::piece::all_pieces()) {
      move_change_table[chess::move::change_index(result, captured)] =
                                          position_value(captured).as_short();
    }
    add_promotion_values(move_change_table, captured);
  }
  add_promotion_values(move_change_table, chess::nonpiece);
  move_change_table[chess::move::change_index(pawn, pawn, chess::move::en_passant)] =
                                          position_value(pawn).as_short();
}

} // anonym namespace

void position_value::default_initialize_lookup_tables()
{
  std::stringstream stream(default_evaluation_values);

  initialize_lookup_tables(stream);
}

void position_value::initialize_lookup_tables(const string& path)
{
  std::ifstream file;

  file.open(path);
  initialize_lookup_tables(file);
  file.close();
}

namespace
{

bool is_line_empty(const string& line)
{
  static std::regex regex("^(\\s*|\\s*#.*)$");

  return (std::regex_match(line, regex));
}

short parse_number(const string& value, int base)
{
  char* pend;
  long result;

  if (value.size() > 5) {
    throw;
  }
  result = strtol(value.c_str(), &pend, base);
  if (result < SHRT_MIN or result > SHRT_MAX or *pend != '\0') {
    throw;
  }
  return short(result);
}

void get_key_value_strings(const string& line, string& key, short& value)
{
  static std::regex expr_decimal("\\s*([a-z_]+)\\s*=\\s*(-?\\d+)\\s*");
  static std::regex expr_hexadecimal("\\s*([a-z_]+)\\s*=\\s*(0x[a-f0-9]+)\\s*");

  std::smatch key_value;

  std::regex_match(line, key_value, expr_decimal);
  if (key_value.size() == 3) {
    value = parse_number(key_value[2], 10);
  }
  else {
    std::regex_match(line, key_value, expr_hexadecimal);
    if (key_value.size() == 3) {
      value = parse_number(key_value[2], 16);
    }
    else {
      throw;
    }
  }
  key = key_value[1];
}

} // anonym namespace

static void read_conf(std::istream& stream, conf_t& conf)
{
  string line;

  while (std::getline(stream, line)) {
    if (is_line_empty(line)) {
      continue;
    }

    string key;
    short value;

    get_key_value_strings(line, key, value);
    conf.emplace(key, value);
  }
}

void position_value::initialize_lookup_tables(std::istream& stream)
{
  conf_t conf;

  read_conf(stream, conf);
  setup_piece_values(piece_values, conf);
  setup_move_changes(move_change_table);
}

position_value::position_value(const chess::position& position):
  internal(0)
{
  for (auto index : chess::sq_index::range()) {
    operator+(position.piece_at(index));
  }
}

} /* namespace kator::engine */
} /* namespace kator */
