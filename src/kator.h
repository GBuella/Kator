
#ifndef KATOR_H
#define KATOR_H

#include <string>
#include <iostream>
#include <memory>

#include "book/book.h"
#include "chess/chess.h"

namespace kator
{

struct conf
{
  conf();
  move_notation notation;
  unsigned main_hash_size;
  unsigned aux_hash_size;
  unsigned analyze_hash_size;
  unsigned analyze_aux_hash_size;
  std::string book_path;
  book_type book_type;
  bool use_unicode;
}; /* struct conf */

namespace engine { class engine; }

class kator
{
public:

  static std::unique_ptr<kator> create(std::unique_ptr<book> initial_book,
                                       std::unique_ptr<engine::engine>,
                                       std::unique_ptr<game>,
                                       const conf&,
                                       std::ostream& out,
                                       std::ostream& err);

  virtual void command_loop(std::istream&) = 0;
  virtual ~kator() {}
};

} /* namespace kator */

#endif /* !defined(KATOR_H) */
