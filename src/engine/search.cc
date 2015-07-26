
#include "search.h"

#include <atomic>
#include <mutex>
#include <stack>
#include <thread>

#include "engine.h"
#include "node.h"
#include "zhash_table.h"
#include "chess/position.h"

using ::std::unique_ptr;
using ::std::make_unique;

namespace kator
{
namespace engine
{

node::node(const chess::position& ctor_position):
  position(ctor_position),
  alpha(negative_infinite),
  beta(positive_infinite),
  killers({{chess::null_move, chess::null_move, chess::null_move}})
{
}

namespace 
{

class search_implementation : public search
{
  const unique_ptr<const chess::position> root;
  unsigned max_depth;
  unsigned long node_count;
  std::atomic<bool> is_running;

  std::mutex mutex;
  std::thread work;

  void process_root_node()
  {
    
  }

public:

  search_implementation(const chess::position& CTORRoot, unsigned CTORDepth):
    root(new chess::position(CTORRoot)),
    max_depth(CTORDepth),
    node_count(0),
    is_running(false)
  {
  }

  void process()
  {
    std::lock_guard<std::mutex> guard(mutex);
    if (is_running) {
      return;
    }
    is_running.store(true);
  }

  unsigned long get_node_count() const noexcept
  {
    return node_count;
  }

  bool is_done() const noexcept
  {
    return not is_running.load();
  }

  bool did_finish_first_root_move() const noexcept
  {
    return false;
  }

  void reset()
  {
  }

  void increase_depth()
  {
  }

  unsigned current_depth() const noexcept
  {
    return max_depth;
  }

  chess::move_list get_pv() const noexcept
  {
    return chess::move_list(*root);
  }

  position_value get_move_value(chess::move) const
  {
    return position_value::null_value();
  }

}; /* class search_implementation */

class search_factory_implementation : public search_factory
{
public:

  search_factory_implementation()
  {
  }

  unique_ptr<search> create_search(const chess::position& root,
                                   unsigned depth)
  {
    return make_unique<search_implementation>(root, depth);
  }

}; /* class search_factory_implementation */

} /* anonymous namespace */

unique_ptr<search_factory> search_factory::create()
{
  return make_unique<search_factory_implementation>();
}

} /* namespace kator::engine */
} /* namespace kator */
