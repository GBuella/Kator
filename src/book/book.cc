
#include <string>
#include <vector>
#include <stdexcept>

#include "book.h"
#include "book_fen.h"
#include "chess/move.h"

std::unique_ptr< kator::book::book > kator::book::book::open(kator::book::book_type book_type, std::string path)
{
  switch (book_type) {
    case kator::book::book_type::builtin:
      {
        const std::vector<std::string> builtin_book{{
#         include "builtin_book.inc"
        }};
        return std::make_unique< class kator::book::book_fen>(builtin_book);
      }
    case kator::book::book_type::fen:
      return std::unique_ptr<book>(new book_fen(path));
    case kator::book::book_type::polyglot: // empty for now
    case kator::book::book_type::empty:
      return std::unique_ptr<book>(new book_fen(std::vector<std::string>()));
  }
  throw new std::logic_error("invalid book_type");
}

kator::chess::move kator::book::book::get_move(const kator::chess::game_state& state)
{
  std::vector<kator::chess::move> moves = get_moves(state);

  return moves[0];
}

kator::book::book::~book()
{
}

