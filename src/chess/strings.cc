
#include <cctype>
#include <string>

#include "chess.h"
#include "position.h"

using ::std::string;

namespace kator
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

invalid_move_string::invalid_move_string(const char* ctor_move_str)
{
  static const string prefix = "Invalid move ";
  string move_str((ctor_move_str));

  move_error_string = prefix;
  move_error_string += move_str;
}

invalid_move_string::invalid_move_string(const string& ctor_move_str):
  invalid_move_string(ctor_move_str.data())
{
}

const char* invalid_move_string::what() const noexcept
{
  return move_error_string.c_str();
}


std::ostream& operator<< (std::ostream& stream, real_player player)
{
  switch (player) {
    case white:
      return stream << "white";
    case black:
      return stream << "black";
    default:
      return stream << "[internal error]";
  }
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

string rank::to_str(position_player point_of_view) const
{
  string result;

  if (point_of_view == player_opponent) {
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

bool real_piece::is_valid_char(char c)
{
  return c != ' ' and std::strchr(piece_chars, c) != nullptr;
}

real_piece::real_piece(char c)
{
  switch (tolower(c)) {
    case 'p':
      type = piece::pawn;
      break;
    case 'k':
      type = piece::king;
      break;
    case 'n':
      type = piece::knight;
      break;
    case 'b':
      type = piece::bishop;
      break;
    case 'r':
      type = piece::rook;
      break;
    case 'q':
      type = piece::queen;
      break;
    default:
      throw;
  }
  player = islower(c) ? real_player::black : real_player::white;
}

char real_piece::to_char() const
{
  return piece_chars[offset()];
}

string real_piece::to_str() const
{
  string result;

  result.push_back(to_char());
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

string real_piece::to_figurine() const
{
  switch (player) {
    case white:
      switch (type) {
        case piece::pawn: return u_white_pawn;
        case piece::rook: return u_white_rook;
        case piece::knight: return u_white_knight;
        case piece::bishop: return u_white_bishop;
        case piece::queen: return u_white_queen;
        case piece::king: return u_white_king;
        default: return " ";
      }
    case black:
      switch (type) {
        case piece::pawn: return u_black_pawn;
        case piece::rook: return u_black_rook;
        case piece::knight: return u_black_knight;
        case piece::bishop: return u_black_bishop;
        case piece::queen: return u_black_queen;
        case piece::king: return u_black_king;
        default: return " ";
      }
    default: return " ";
  }
}

string real_piece::to_str(bool use_figurines) const
{
  if (use_figurines) {
    return to_figurine();
  }
  else {
    return to_str();
  }
}

std::ostream& operator<< (std::ostream& stream, real_piece piece)
{
  return stream << piece.to_str();
}

sq_index::sq_index(const std::string& str):
  sq_index(kator::rank(str.at(1)), kator::file(str.at(0)))
{}

string sq_index::to_str(position_player point_of_view) const
{
  return file().to_str() + rank().to_str(point_of_view);
}

std::ostream& operator<< (std::ostream& stream, sq_index index)
{
  return stream << index.to_str(player_to_move);
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

  /*{{{ "Those letters which appear will be ordered first uppercase
        before lowercase and second kingside before queenside.
        There is no white space between the letters."
  }}}*/

  if (can_castle_kingside()) {
    result.push_back('K');
  }
  if (can_castle_queenside()) {
    result.push_back('Q');
  }
  if (opponent_can_castle_kingside()) {
    result.push_back('k');
  }
  if (opponent_can_castle_queenside()) {
    result.push_back('q');
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
    if (real_piece::is_valid_char(c)
        or rank::is_valid_char(c)
        or file::is_valid_char(c)
        or std::tolower(c) == 'o')
    {
      str.push_back(static_cast<char>(std::tolower(c)));
    }
  }
}

} /* namespace kator */
