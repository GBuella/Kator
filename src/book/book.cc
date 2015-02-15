
#include <string>
#include <vector>
#include <stdexcept>

#include "book.h"
#include "book_fen.h"
#include "chess/move.h"

using ::std::unique_ptr;

namespace kator
{
namespace book
{

unique_ptr<book> book::open(book_type book_type, std::string path)
{
  switch (book_type) {
    case kator::book::book_type::builtin:
      {
        const std::vector<std::string> builtin_book{{
#         include "builtin_book.inc"
        }};
        return unique_ptr<book>(new book_fen(builtin_book));
      }
    case kator::book::book_type::fen:
      return unique_ptr<book>(new book_fen(path));
    case kator::book::book_type::polyglot: // empty for now
    case kator::book::book_type::empty:
      return unique_ptr<book>(new book_fen(std::vector<std::string>()));
  }
  throw new std::logic_error("invalid book_type");
}

chess::move book::get_move(const chess::game_state& state)
{
  std::vector<chess::move> moves = get_moves(state);

  return moves[0];
}

book::~book()
{
}

} /* namespace kator::book */
} /* namespace kator */
