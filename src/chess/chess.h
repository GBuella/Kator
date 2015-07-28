
#ifndef KATOR_CHESS_H
#define KATOR_CHESS_H

#include "platform.h"

#include "player.h"
#include "sq_index.h"
#include "piece.h"

#include <exception>
#include <memory>
#include <ostream>

namespace kator
{
namespace chess
{

class bitboard;
class position;
struct move;
class move_list;
struct game_state;
class game;

enum class move_notation
{
  coordinate,
  SAN,
  FAN
};

extern const char starting_fen[];

class invalid_fen: public std::exception
{
public:
  virtual const char* what() const noexcept;
};

class invalid_castle_rights: public std::exception
{
public:
  virtual const char* what() const noexcept;
};

class invalid_en_passant_index: public std::exception
{
public:
  virtual const char* what() const noexcept;
};

class invalid_king_positions: public std::exception
{
public:
  virtual const char* what() const noexcept;
};

class invalid_move_string
{
public:
  invalid_move_string(const char*);
  invalid_move_string(const std::string&);
  const char* what() const noexcept;
private:
  std::string move_error_string;
};

std::unique_ptr<game_state> parse_fen(std::string);
std::unique_ptr<game_state> parse_fen(std::istream&);

std::ostream& operator<< (std::ostream&, player);
std::ostream& operator<< (std::ostream&, real_player);

void bitboard_lookup_tables_init();

void cleanup_move_string(std::string&);

std::ostream& operator<< (std::ostream&, sq_index);
std::ostream& operator<< (std::ostream&, piece);

} /* namespace kator::chess */

} /* namespace kator */

#endif /* !defined(KATOR_CHESS_H) */

