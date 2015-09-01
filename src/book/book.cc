
#include <string>
#include <vector>
#include <stdexcept>

#include "book.h"
#include "book_fen.h"
#include "chess/move.h"

namespace kator
{

std::unique_ptr<book> book::open(book_type type, std::string path)
{
  switch (type) {
    case book_type::builtin:
      {
        const std::vector<std::string> builtin_book{{
#         include "builtin_book.inc"
        }};
        return std::make_unique<book_fen>(builtin_book);
      }
    case book_type::fen:
      return std::unique_ptr<book>(new book_fen(path));
    case book_type::polyglot: // empty for now
    case book_type::empty:
      return std::unique_ptr<book>(new book_fen(std::vector<std::string>()));
  }
  throw new std::logic_error("invalid book_type");
}

move book::get_move(const game_state& state)
{
  std::vector<move> moves = get_moves(state);

  return moves[0];
}

book::~book()
{
}

} /* namespace kator */
