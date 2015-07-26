
#include "tests.h"
#include "chess/position.h"
#include "chess/game_state.h"

namespace kator
{

namespace
{

enum class perft_type
{
  simple, with_make_move
};

template<perft_type type>
unsigned long compute_perft(const chess::position& position, unsigned depth)
{
  if (depth < 1) {
    return 1;
  }

  chess::move_list moves(position);
  if (type == perft_type::simple and depth == 1) {
    return moves.count();
  }

  unsigned long n = 0;

  for (auto move : moves) {
    chess::position child(position, move);

    n += compute_perft<type>(child, depth - 1);
  }
  return n;
}

} /* anonym namespace */

unsigned long perft(const chess::position& position, unsigned depth)
{
  initialize_permanent_vectors();
  return compute_perft<perft_type::simple>(position, depth);
}

unsigned long slow_perft(const chess::position& position, unsigned depth)
{
  initialize_permanent_vectors();
  return compute_perft<perft_type::with_make_move>(position, depth);
}

unsigned long perft(const chess::game_state& state, unsigned depth)
{
  initialize_permanent_vectors();
  return compute_perft<perft_type::simple>(*state.position, depth);
}

unsigned long slow_perft(const chess::game_state& state, unsigned depth)
{
  initialize_permanent_vectors();
  return compute_perft<perft_type::with_make_move>(*state.position, depth);
}

} /* namespace kator */
