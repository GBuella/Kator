
#ifndef KATOR_H
#define KATOR_H

#include <string>
#include <iostream>
#include <memory>

#include "book/book.h"
#include "chess/chess.h"

namespace kator
{

namespace conf
{
  extern chess::move_notation move_notation;
  extern unsigned main_hash_size;
  extern unsigned aux_hash_size;
  extern unsigned analyze_hash_size;
  extern unsigned analyze_aux_hash_size;
  extern char* book_path;
  extern book::book_type book_type;
  extern bool use_unicode;
}

void command_loop(std::unique_ptr<book::book>&,
                  std::istream& in,
                  std::ostream& out,
                  std::ostream& err);

} /* namespace kator */

#endif /* !defined(KATOR_H) */
