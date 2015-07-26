
#include "gtest/gtest.h"
#include "chess/move.h"
#include "chess/game_state.h"

using namespace ::kator::chess;

std::ostream& operator<< (std::ostream& stream, move)
{
  return stream << move.from << move.to;
}

TEST(parse_move, chess::game_state)
{
  std::unique_ptr<game_state> state = parse_fen(starting_fen);

  ASSERT_THROW(state->parse_move("klmkmm"), invalid_move_string);
  ASSERT_THROW(state->parse_move(""), invalid_move_string);
  ASSERT_THROW(state->parse_move("00"), invalid_move_string);
  ASSERT_THROW(state->parse_move("a1a1"), invalid_move_string);
  ASSERT_THROW(state->parse_move("g7g1"), invalid_move_string);
  ASSERT_THROW(state->parse_move("e1e2"), invalid_move_string);
  ASSERT_THROW(state->parse_move("d7d6"), invalid_move_string);

  move push(a2, a4, pawn, move::pawn_double_push);

  ASSERT_EQ(push, state->parse_move("a2a4"));

  state = parse_fen(
      "r3k2r/p1ppqpb1/bnP1pnp1/4N3/Pp2P3/2N2Q2/1PPBBPpP/R3K2R b KQkq a3 0 1");

  ASSERT_EQ(a3, state->parse_move("bxa3").to);
  ASSERT_EQ(b4, state->parse_move("bxa3").from);
  ASSERT_EQ(a3, state->parse_move("bxa3ep").to);
  ASSERT_EQ(b4, state->parse_move("bxa3ep").from);
  ASSERT_EQ(a3, state->parse_move("bxa3e.p.").to);
  ASSERT_EQ(b4, state->parse_move("bxa3e.p.").from);
  ASSERT_TRUE(state->parse_move("bxa3e.p.").is_en_passant());
  ASSERT_TRUE(state->parse_move("bxa3").is_en_passant());
  ASSERT_TRUE(state->parse_move("bxa3").is_capture());

  state = parse_fen(
      "r1k4r/p1ppqpb1/bn2pNp1/8/Pp2P3/2N2Q2/1PPBBPPP/R3K2R w KQ - 0 1");

  ASSERT_EQ(castle_kingside, state->parse_move("o-O"));
  ASSERT_EQ(castle_queenside, state->parse_move("o-O-O"));
  ASSERT_EQ(castle_kingside, state->parse_move("e1g1"));
  ASSERT_EQ(castle_queenside, state->parse_move("e1c1"));
}

