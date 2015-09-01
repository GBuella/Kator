
#ifndef KATOR_BOOK_H
#define KATOR_BOOK_H

#include <string>
#include <vector>
#include <memory>

#include "chess/chess.h"

namespace kator
{

enum class book_type {
  builtin,
  polyglot,
  fen,
  empty
};

class book
{
protected:

  virtual std::vector<move> get_moves(const game_state&) = 0;

public:

  static std::unique_ptr<book> open(book_type, std::string path);

  move get_move(const game_state&);

  virtual ~book();
};

} /* namespace kator */

#endif
