
#include "tests.h"
#include "chess/game.h"

static const char test_filename[] = "tests/move_counter.cc";

[[noreturn]] void kator::test_move_counters()
{
  using namespace ::kator::chess;

  std::unique_ptr<game_state> state = parse_fen(starting_fen);

  ASSERT_EQ(0, state->half_moves);
  ASSERT_EQ(1, state->full_moves);

  move push(a2, a4, pawn, move::pawn_double_push);

  state = state->make_move(push);

  ASSERT_EQ(0, state->half_moves);
  ASSERT_EQ(1, state->full_moves);

  state = state->make_move(move(g8, f6, knight));

  ASSERT_EQ(1, state->half_moves);
  ASSERT_EQ(2, state->full_moves);

  state = state->make_move(move(a1, a3, rook));

  ASSERT_EQ(2, state->half_moves);
  ASSERT_EQ(2, state->full_moves);

  exit(EXIT_SUCCESS);
}

