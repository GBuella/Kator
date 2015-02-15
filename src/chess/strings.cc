
#include <string>

#include "chess.h"
#include "position.h"

using ::std::string;

namespace kator
{
namespace chess
{

const char* invalid_fen::what() const noexcept
{
  return "Invalid FEN";
}

const char* invalid_castle_rights::what() const noexcept
{
  return "Invalid FEN";
}

const char* invalid_en_passant_index::what() const noexcept
{
  return "Invalid en passant square";
}

const char* invalid_king_positions::what() const noexcept
{
  return "Invalid poistion of kings";
}

invalid_move_string::invalid_move_string(const char* ctor_move_str):
  move_error_string(nullptr)
{
  static const char prefix[] = "Invalid move ";
  char *str = new char[strlen(prefix) + strlen(ctor_move_str) + 1];

  strcpy(str, prefix);
  strcat(str, ctor_move_str);
  move_error_string.reset(str);
}

invalid_move_string::invalid_move_string(const string& ctor_move_str):
  invalid_move_string(ctor_move_str.data())
{
}

const char* invalid_move_string::what() const noexcept
{
  return move_error_string.get();
}


std::ostream& operator<< (std::ostream& stream, const player player)
{
  switch (player) {
    case kator::chess::player::to_move:
      return stream << "player::to_move";
    case kator::chess::player::opponent:
      return stream << "player::to_opponent";
  }
  return stream << "[invalid enum]";
}

std::ostream& operator<< (std::ostream& stream, const real_player player)
{
  switch (player) {
    case white:
      return stream << "white";
    case black:
      return stream << "black";
  }
  return stream << "[invalid enum]";
}

string file::to_str() const
{
  static char chars[] = "hgfedcba";
  string result;

  result.push_back(chars[value]);
  return result;
}
  
file::file(char c)
{
  if (is_valid_char(c)) {
    char lower = static_cast<char>(std::tolower(c));
    value = static_cast<unsigned char>('h' - lower);
  }
  else {
    throw std::exception();
  }
}

bool file::is_valid_char(char c)
{
  return (c >= 'a' && c <= 'h') || (c >= 'A' && c <= 'H');
}

string rank::to_str(chess::player point_of_view) const
{
  string result;

  if (point_of_view == player::opponent) {
    result.push_back(static_cast<char>('1' + value));
  }
  else {
    result.push_back(static_cast<char>('1' + 7 - value));
  }
  return result;
}

rank::rank(char c)
{
  if (is_valid_char(c)) {
    value = static_cast<unsigned char>('8' - c);
  }
  else {
    throw std::exception();
  }
}

bool rank::is_valid_char(char c)
{
  return c >= '1' and c <= '8';
}

static const char piece_chars[] = "  PpRrKkBbNnQq";

bool piece::is_valid_char(char c)
{
  return c != ' ' and std::strchr(piece_chars, c) != nullptr;
}

piece::piece(char c)
{
  switch (tolower(c)) {
    case 'p':
      value = piece_index::pawn;
      break;
    case 'k':
      value = piece_index::king;
      break;
    case 'n':
      value = piece_index::knight;
      break;
    case 'b':
      value = piece_index::bishop;
      break;
    case 'r':
      value = piece_index::rook;
      break;
    case 'q':
      value = piece_index::queen;
      break;
    default:
      value = piece_index::nonpiece;
      return;
  }
  if (islower(c)) {
    *this = opponent_of(*this);
  }
}

string piece::to_str(chess::player point_of_view) const
{
  string result;

  if (point_of_view == chess::player::to_move) {
    result.push_back(piece_chars[index()]);
  }
  else {
    result.push_back(piece_chars[opponent_of(*this).index()]);
  }
  return result;
}

namespace
{

static const char* u_white_pawn = "\U00002659";
static const char* u_white_rook = "\U00002656";
static const char* u_white_knight = "\U00002658";
static const char* u_white_bishop = "\U00002657";
static const char* u_white_queen = "\U00002654";
static const char* u_white_king = "\U00002657";
static const char* u_black_pawn = "\U0000265f";
static const char* u_black_rook = "\U0000265c";
static const char* u_black_knight = "\U0000265e";
static const char* u_black_bishop = "\U0000265d";
static const char* u_black_queen = "\U0000265b";
static const char* u_black_king = "\U0000265a";

}

string piece::to_figurine(chess::player point_of_view) const
{
  piece temp = *this;

  if (point_of_view == chess::player::opponent) {
    temp = opponent_of(temp);
  }
  switch (temp.value) {
    case piece_index::pawn: return u_white_pawn;
    case piece_index::rook: return u_white_rook;
    case piece_index::knight: return u_white_knight;
    case piece_index::bishop: return u_white_bishop;
    case piece_index::queen: return u_white_queen;
    case piece_index::king: return u_white_king;
    case piece_index::opponent_pawn: return u_black_pawn;
    case piece_index::opponent_rook: return u_black_rook;
    case piece_index::opponent_knight: return u_black_knight;
    case piece_index::opponent_bishop: return u_black_bishop;
    case piece_index::opponent_queen: return u_black_queen;
    case piece_index::opponent_king: return u_black_king;
    case piece_index::nonpiece:
    case piece_index::index_1:
      return " ";
  }
  return " ";
}

string piece::to_str(chess::player point_of_view, bool use_figurines) const
{
  if (use_figurines) {
    return to_figurine(point_of_view);
  }
  else {
    return to_str(point_of_view);
  }
}

std::ostream& operator<< (std::ostream& stream, const piece& piece)
{
  return stream << piece.to_str(player::to_move);
}

std::string sq_index::to_str(kator::chess::player point_of_view) const
{
  return this->file().to_str() + this->rank().to_str(point_of_view);
}

sq_index::sq_index(const std::string& str):
  sq_index(chess::rank(str.at(1)), chess::file(str.at(0)))
{}

std::ostream& operator<< (std::ostream& stream, const sq_index& index)
{
  return stream << index.to_str(player::to_move);
}

castle_rights::castle_rights(std::string FEN_string)
{
  if (FEN_string != "-") {
    if (std::strspn(FEN_string.c_str(), "QqKk") != FEN_string.size()) {
      throw invalid_fen();
    }
  }
  rights[static_cast<unsigned>(side::queenside)]
                = FEN_string.find('Q') != std::string::npos;
  rights[static_cast<unsigned>(side::kingside)]
                = FEN_string.find('K') != std::string::npos;
  rights[static_cast<unsigned>(side::opponent_queenside)]
                = FEN_string.find('q') != std::string::npos;
  rights[static_cast<unsigned>(side::opponent_kingside)]
                = FEN_string.find('k') != std::string::npos;
}

std::string castle_rights::generate_castle_FEN(real_player point_of_view) const
{
  if (point_of_view == black) {
    castle_rights temp(*this);

    temp.flip();
    return temp.generate_castle_FEN(white);
  }

  std::string result;

  if (can_castle_queenside()) {
    result.push_back('Q');
  }
  if (can_castle_kingside()) {
    result.push_back('K');
  }
  if (opponent_can_castle_queenside()) {
    result.push_back('q');
  }
  if (opponent_can_castle_kingside()) {
    result.push_back('k');
  }
  if (result.size() == 0) {
    result.push_back('-');
  }
  return result;
}

void cleanup_move_string(string& str)
{
  string original;

  if (str.size() > 2) {
    if (str.substr(str.size() - 2) == "ep") {
      str.erase(str.size() - 2);
    }
  }
  if (str.size() > 4) {
    if (str.substr(str.size() - 4) == "e.p.") {
      str.erase(str.size() - 4);
    }
  }
  original = str;
  str = "";
  for (auto c : original) {
    if (piece::is_valid_char(c)
        or rank::is_valid_char(c)
        or file::is_valid_char(c)
        or std::tolower(c) == 'o')
    {
      str.push_back(static_cast<char>(std::tolower(c)));
    }
  }
}


} /* namespace kator::chess */
} /* namespace kator */
