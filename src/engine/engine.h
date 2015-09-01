
#ifndef KATOR_ENGINE_H
#define KATOR_ENGINE_H

#include <functional>
#include <stdexcept>
#include <memory>

#include "chess/chess.h"
#include "chess/move.h"
#include "chess/move_list.h"
#include "eval.h"
#include "search.h"

namespace kator
{
namespace engine
{

class search;

struct result
{
  unsigned depth;
  move_list pv;
  move best_move;
  position_value value;
};

class engine
{
public:

  static std::unique_ptr<engine> create(std::unique_ptr<search_factory>);

  virtual void set_max_depth(unsigned) = 0;
  virtual unsigned get_max_depth() const noexcept = 0;
  virtual bool is_running() const noexcept = 0;
  virtual void shutdown() = 0;
  virtual void set_max_time(unsigned ms) = 0;
  //virtual void set_thread_count(unsigned) = 0;
  //virtual void get_thread_count(unsigned) const noexcept = 0;
  virtual void set_sub_result_callback(std::function<void(result)>) = 0;
  virtual void set_final_result_callback(std::function<void(result)>) = 0;
  virtual void set_fixed_result_callback(std::function<void(result)>) = 0;
  virtual void start(std::unique_ptr<game_state> root) = 0;
  virtual ~engine() {}
};

struct searh_parameters
{
  std::function<void(const result&)> sub_result_callback;
  std::function<void(const result&)> final_result_callback;
};

void start_search(const game_state&, const searh_parameters&);
void search_shutdown();

} // namespace kator::engine
} // namespace kator

#endif // !defined(KATOR_ENGINE_H)
