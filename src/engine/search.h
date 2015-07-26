
#ifndef KATOR_ENGINE_SEARCH_H
#define KATOR_ENGINE_SEARCH_H

#include <map>

#include "chess/chess.h"
#include "chess/move_list.h"
#include "eval.h"

namespace kator
{
namespace chess
{
class position;
}

namespace engine
{

class search
{
public:


  virtual void process() = 0;
  virtual unsigned long get_node_count() const noexcept = 0;
  virtual bool is_done() const noexcept = 0;
  virtual bool did_finish_first_root_move() const noexcept = 0;
  virtual void reset() = 0;
  virtual void increase_depth() = 0;
  virtual unsigned current_depth() const noexcept = 0;
  virtual chess::move_list get_pv() const noexcept = 0;
  virtual position_value get_move_value(chess::move) const = 0;
  //virtual void set_transposition_table(zhash_table&) = 0;

  virtual ~search() {}

}; /* class search */

class search_factory
{
public:
  virtual std::unique_ptr<search> create_search(const chess::position&,
                                                unsigned depth) = 0;

  static std::unique_ptr<search_factory> create();

}; /* class search_factory */

} /* namespace kator::engine */
} /* namespace kator */

#endif /* !defined(KATOR_ENGINE_SEARCH_H) */

