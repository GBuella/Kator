
#include "tests.h"

#include <type_traits>

#include "chess/position.h"
#include "chess/game_state.h"

namespace kator
{

namespace
{

enum perft_type
{
  simple, with_make_move
};

template<perft_type type>
unsigned long compute_perft(const ::kator::position& position, unsigned depth)
{
  if (depth < 1) {
    return 1;
  }

  move_list moves(position);
  if (type == perft_type::simple and depth == 1) {
    return static_cast<unsigned long>(moves.count());
  }

  unsigned long n = 0;

  for (auto move : moves) {
    class position child(position, move);

    n += compute_perft<type>(child, depth - 1);
  }
  return n;
}

} /* anonym namespace */

unsigned long
perft(const position& position, unsigned depth)
{
  initialize_permanent_vectors();
  return compute_perft<simple>(position, depth);
}

unsigned long
slow_perft(const position& position, unsigned depth)
{
  initialize_permanent_vectors();
  return compute_perft<with_make_move>(position, depth);
}

unsigned long
perft(const game_state& state, unsigned depth)
{
  return perft(*state.position, depth);
}

unsigned long
slow_perft(const game_state& state, unsigned depth)
{
  return slow_perft(*state.position, depth);
}

} /* namespace kator */
