
#ifndef KATOR_BOOKFEN_H
#define KATOR_BOOKFEN_H

#include <unordered_map>
#include <vector>

#include "book.h"

namespace kator
{
namespace book
{

class book_fen : public book
{

  std::unordered_map<std::string, std::vector<chess::move>> data;

public:
  book_fen(std::string path);
  book_fen(const std::vector<std::string>& lines);

  std::vector<chess::move> get_moves(const chess::game_state&);

};

} /* namespace kator::book */
} /* namespace kator */

#endif /* !defined(KATOR_BOOK_FEN_H) */
