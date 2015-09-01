
#include <sstream>
#include <fstream>
#include <regex>
#include <map>
#include <string>
#include <cstdlib>
#include <climits>

#include "eval.h"
#include "chess/position.h"

using ::std::string;

namespace kator
{

namespace engine
{

std::array<short, piece_array_size> position_value::piece_values;
move::change_array_t<short> position_value::move_change_table;

namespace
{

typedef std::map<string, short> conf_t;

#include "default_values.inc"

void setup_piece_values(std::array<short, piece_array_size>& values,
                        const conf_t& conf)
{
  values[pawn] =   conf.at("pawn");
  values[rook] =   conf.at("rook");
  values[knight] = conf.at("knight");
  values[bishop] = conf.at("bishop");
  values[queen] =  conf.at("queen");
  values[opponent_pawn] =   -conf.at("pawn");
  values[opponent_rook] =   -conf.at("rook");
  values[opponent_knight] = -conf.at("knight");
  values[opponent_bishop] = -conf.at("bishop");
  values[opponent_queen] =  -conf.at("queen");
}

template<typename... extra_argument>
void add_promotion_values(move::change_array_t<short>& move_change_table,
                          extra_argument... extra)
{
  auto entry = [&](piece type) -> short&
  {
    auto index = move::change_index(type, extra..., move::promotion);
    return move_change_table[index];
  };

  entry(piece::queen) =
                    (position_value(queen) - position_value(pawn)).as_short();
  entry(piece::rook) =
                    (position_value(rook) - position_value(pawn)).as_short();
  entry(piece::knight) =
                    (position_value(knight) - position_value(pawn)).as_short();
  entry(piece::bishop) =
                    (position_value(bishop) - position_value(pawn)).as_short();
}

void setup_move_changes(move::change_array_t<short>& move_change_table)
{
  auto set = [&](position_value value, piece result, piece captured,
                 move::move_type_enum type)
  {
    short short_value = value.as_short();

    move_change_table[move::change_index(result, captured, type)] = short_value;
  };

  for (auto captured : all_piece_types()) {
    for (auto result : all_piece_types()) {
      set(position_value(captured), result, captured, move::general);
    }
    add_promotion_values(move_change_table, captured);
  }
  add_promotion_values(move_change_table);
  set(position_value(pawn), piece::pawn, piece::pawn, move::en_passant);
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

position_value::position_value(const position& position):
  internal(0)
{
  for (auto index : sq_index::range()) {
    operator+(position.piece_at(index));
  }
}

} /* namespace kator::engine */
} /* namespace kator */
