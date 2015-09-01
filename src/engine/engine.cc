
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

#include "chess/game_state.h"
#include "engine.h"
#include "search.h"
#include "chess/position.h"

using ::std::unique_ptr;
using ::std::make_unique;
using ::std::function;

namespace kator
{
namespace engine
{

namespace
{

class engine_implementation : public engine
{
  std::mutex mutex;
  unique_ptr<search_factory> factory;
  unsigned max_depth;
  unsigned max_time;
  bool is_search_running;
  std::vector<unique_ptr<search> > workers;
  function<void(result)> sub_result_callback;
  function<void(result)> final_result_callback;
  function<void(result)> fixed_result_callback;

  unique_ptr<game_state> root;

  static constexpr unsigned absolute_max_depth = 128;

public:

  engine_implementation(unique_ptr<search_factory> ctor_factory):
    max_depth(0),
    is_search_running(false)
  {
    if (ctor_factory == nullptr) {
      throw std::exception();
    }
    factory = std::move(ctor_factory);
  }

  void set_max_depth(unsigned depth)
  {
    std::lock_guard<std::mutex> guard(mutex);
    if (not is_search_running) {
      if (depth > absolute_max_depth) {
        depth = absolute_max_depth;
      }
      max_depth = depth;
      if (max_depth > 0) {
        max_time = 0;
      }
    }
  }

  unsigned get_max_depth() const noexcept
  {
    return max_depth;
  }

  bool is_running() const noexcept
  {
    return is_search_running;
  }

  void set_sub_result_callback(function<void(result)> callback)
  {
    sub_result_callback = callback;
  }

  void set_final_result_callback(function<void(result)> callback)
  {
    final_result_callback = callback;
  }

  void set_fixed_result_callback(function<void(result)> callback)
  {
    fixed_result_callback = callback;
  }

  void set_max_time(unsigned ms)
  {
    std::lock_guard<std::mutex> guard(mutex);
    if (not is_search_running) {
      max_time = ms;
      if (ms > 0 ) {
        max_depth = 0;
      }
    }
  }

  void start(unique_ptr<game_state> search_root)
  {
    std::lock_guard<std::mutex> guard(mutex);

    if (is_search_running) {
      throw std::exception();
    }
    root = std::move(search_root);
    is_search_running = true;
  }

  void shutdown()
  {
    std::lock_guard<std::mutex> guard(mutex);
    if (not is_search_running) {
      return;
    }
    workers.clear();
    is_search_running = false;
  }

  ~engine_implementation()
  {
    workers.clear();
  }

}; /* class engine_implementation */

} /* anonymous namespace */

unique_ptr<engine> engine::create(unique_ptr<search_factory> factory)
{
  return make_unique<engine_implementation>(std::move(factory));
}

} // namespace kator::engine
} // namespace kator
