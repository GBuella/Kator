
#include "game.h"
#include "game_state.h"
#include "position.h"

#include <vector>
#include <memory>
#include <sstream>

using ::std::unique_ptr;
using ::std::make_unique;
using ::std::string;
using ::std::istream;


namespace kator
{

const char starting_fen[] =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

class game_implementation final : public game
{

  std::vector< unique_ptr<game_state> > states;
  size_t current_index;

public:

  game_implementation()
  {
    reset();
  }

  game_implementation(unique_ptr<game_state> state)
  {
    states.push_back(std::move(state));
    current_index = 0;
  }

  void reset() override
  {
    reset(std::string(starting_fen));
  }

  void reset(std::string FEN) override
  {
    reset(game_state::parse_fen(std::move(FEN)));
  }

  void reset(unique_ptr<game_state> state) override
  {
    states.clear();
    states.push_back(std::move(state));
    current_index = 0;
  }

  const game_state& current_state() const noexcept override
  {
    return *(states[current_index]);
  }

  void advance(move move) override
  {
    unique_ptr<game_state> new_state = current_state().make_move(move);
    auto begin = states.begin() + static_cast<ptrdiff_t>(current_index) + 1;

    states.erase(begin, states.end());
    states.push_back(std::move(new_state));
    ++current_index;
  }

  void advance() override
  {
    if (not is_at_last_state()) {
      ++current_index;
    }
  }

  bool is_at_last_state() const noexcept override
  {
    return current_index + 1 == states.size();
  }

  bool is_at_first_state() const noexcept override
  {
    return current_index == 0;
  }

  void revert() override
  {
    if (not is_at_first_state()) {
      --current_index;
    }
  }

  size_t length() const noexcept override
  {
    return states.size();
  }

  real_player turn() const noexcept override
  {
    return current_state().turn;
  }

  const move_list& legal_moves() const noexcept override
  {
    return current_state().moves;
  }

  ~game_implementation()
  {
  }

}; /* class game_implementation */

game::~game()
{
}

unique_ptr<game> game::create()
{
  return make_unique<game_implementation>();
}

unique_ptr<game> game::create(unique_ptr<game_state> state)
{
  return make_unique<game_implementation>(std::move(state));
}

} /* namespace kator */
