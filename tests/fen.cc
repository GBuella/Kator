
#include <memory>

#include "tests.h"
#include "chess/game.h"

static const char test_filename[] = "tests/fen.cc";

namespace kator
{

[[noreturn]] void test_FEN()
{
  using namespace ::kator::chess;
  std::unique_ptr<game_state> state;

  ASSERT_NO_THROW(game_state::parse_fen(starting_fen));

  state = parse_fen(starting_fen);

  ASSERT_EQ(white, state->turn);
  ASSERT_TRUE(state->can_flip());
  ASSERT_EQ(player::to_move, state->whites_view);
  ASSERT_FALSE(state->in_check);
  ASSERT_FALSE(state->is_check_mate);
  ASSERT_FALSE(state->is_stale_mate);
  ASSERT_EQ("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w QKqk - 0 1",
            state->to_FEN());
  ASSERT_EQ("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w QKqk -",
            state->to_FEN_no_move_counts());
  ASSERT_EQ("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b QKqk - 0 1",
            state->flipped_to_FEN());
  ASSERT_EQ("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b QKqk -",
            state->flipped_to_FEN_no_move_counts());

  ASSERT_NO_THROW(state = parse_fen(
      "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"));

  ASSERT_EQ(black, state->turn);
  ASSERT_EQ(player::opponent, state->whites_view);
  ASSERT_FALSE(state->can_flip());
  ASSERT_FALSE(state->in_check);
  ASSERT_FALSE(state->is_check_mate);
  ASSERT_FALSE(state->is_stale_mate);
  ASSERT_EQ("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b QKqk e3 0 1",
            state->to_FEN());
  ASSERT_EQ("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b QKqk e3",
            state->to_FEN_no_move_counts());

  ASSERT_NO_THROW(state = parse_fen(
      "rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w KQkq b6 0 1"));

  ASSERT_EQ(white, state->turn);
  ASSERT_EQ(player::to_move, state->whites_view);
  ASSERT_FALSE(state->can_flip());
  ASSERT_FALSE(state->in_check);
  ASSERT_FALSE(state->is_check_mate);
  ASSERT_FALSE(state->is_stale_mate);
  ASSERT_EQ("rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w QKqk b6 0 1",
            state->to_FEN());
  ASSERT_EQ("rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w QKqk b6",
            state->to_FEN_no_move_counts());

  ASSERT_NO_THROW(state = parse_fen(
      "rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w - - 0 1"));

  ASSERT_EQ(white, state->turn);
  ASSERT_EQ(player::to_move, state->whites_view);
  ASSERT_TRUE(state->can_flip());
  ASSERT_FALSE(state->in_check);
  ASSERT_FALSE(state->is_check_mate);
  ASSERT_FALSE(state->is_stale_mate);
  ASSERT_EQ("rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w - - 0 1",
            state->to_FEN());
  ASSERT_EQ("rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w - -",
            state->to_FEN_no_move_counts());
  ASSERT_EQ("rnbqkbnr/pppp1ppp/8/4p3/1P6/8/P1PPPPPP/RNBQKBNR b - - 0 1",
            state->flipped_to_FEN());
  ASSERT_EQ("rnbqkbnr/pppp1ppp/8/4p3/1P6/8/P1PPPPPP/RNBQKBNR b - -",
            state->flipped_to_FEN_no_move_counts());

  ASSERT_NO_THROW(state = parse_fen(
        "1n2k3/ppppp1pp/2qrn3/r7/2b1Kp2/8/PPPPPPPP/RNBQ1BNR w - -"));

  ASSERT_EQ(white, state->turn);
  ASSERT_EQ(player::to_move, state->whites_view);
  ASSERT_TRUE(state->can_flip());
  ASSERT_TRUE(state->in_check);
  ASSERT_TRUE(state->is_check_mate);
  ASSERT_FALSE(state->is_stale_mate);

  ASSERT_NO_THROW(state = parse_fen(
        "1n2k3/ppppp1pp/3rn3/r7/2b1Kp2/7q/7P/8 w - -"));

  ASSERT_EQ(white, state->turn);
  ASSERT_EQ(player::to_move, state->whites_view);
  ASSERT_TRUE(state->can_flip());
  ASSERT_FALSE(state->in_check);
  ASSERT_FALSE(state->is_check_mate);
  ASSERT_TRUE(state->is_stale_mate);

  ASSERT_NO_THROW(state = parse_fen(
        "1n2k3/ppppp1np/3r4/r4p2/2b1K3/7q/PPPPPPPP/RNBQ1BNR w - -"));

  ASSERT_EQ(white, state->turn);
  ASSERT_EQ(player::to_move, state->whites_view);
  ASSERT_TRUE(state->can_flip());
  ASSERT_TRUE(state->in_check);
  ASSERT_FALSE(state->is_check_mate);
  ASSERT_FALSE(state->is_stale_mate);

  ASSERT_NO_THROW(state = parse_fen(
        "r2bk1nr/ppp1pppp/3p3b/8/Q6n/2P3q1/PP1PPPPP/RNB1KBNR b KQkq -"));

  ASSERT_EQ(black, state->turn);
  ASSERT_EQ(player::opponent, state->whites_view);
  ASSERT_TRUE(state->can_flip());
  ASSERT_TRUE(state->in_check);
  ASSERT_FALSE(state->is_check_mate);
  ASSERT_FALSE(state->is_stale_mate);

  ASSERT_NO_THROW(state = parse_fen(
        "rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w - - 3 8"));

  ASSERT_EQ(state->to_FEN(),
      "rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w - - 3 8");

  // "FEN with missing black king";
  ASSERT_THROW(parse_fen(
      "rnbq1bnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w - - 3 8"),
      invalid_fen);

  // "FEN with two white kings";
  ASSERT_THROW(parse_fen(
      "rnbqKbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w - - 3 8"),
      invalid_fen);

  // "FEN with invalid row";
  ASSERT_THROW(parse_fen(
      "rnbqkbnr/p2pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w - - 3 8"),
      invalid_fen);

  // "FEN with invalid castling rights";
  ASSERT_THROW(parse_fen("4k3/8/8/8/8/8/8/R3K3 w K -"),
      invalid_fen);

  // "FEN with invalid castling rights";
  ASSERT_THROW(parse_fen("4k2r/8/8/8/8/8/8/4K3 w q -"),
      invalid_fen);

  ASSERT_NO_THROW(parse_fen(
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
  ASSERT_NO_THROW(parse_fen(
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));
  ASSERT_NO_THROW(parse_fen("4k3/8/8/8/8/8/8/4K2R w K - 0 1"));
  ASSERT_NO_THROW(parse_fen("4k3/8/8/8/8/8/8/R3K3 w Q - 0 1"));
  ASSERT_NO_THROW(parse_fen("4k2r/8/8/8/8/8/8/4K3 w k - 0 1"));
  ASSERT_NO_THROW(parse_fen("r3k3/8/8/8/8/8/8/4K3 w q - 0 1"));
  ASSERT_NO_THROW(parse_fen("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1"));
  ASSERT_NO_THROW(parse_fen("r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/8/8/8/8/6k1/4K2R w K - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/8/8/8/8/1k6/R3K3 w Q - 0 1"));
  ASSERT_NO_THROW(parse_fen("4k2r/6K1/8/8/8/8/8/8 w k - 0 1"));
  ASSERT_NO_THROW(parse_fen("r3k3/1K6/8/8/8/8/8/8 w q - 0 1"));
  ASSERT_NO_THROW(parse_fen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"));
  ASSERT_NO_THROW(parse_fen("r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1"));
  ASSERT_NO_THROW(parse_fen("r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1"));
  ASSERT_NO_THROW(parse_fen("r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1"));
  ASSERT_NO_THROW(parse_fen("1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1"));
  ASSERT_NO_THROW(parse_fen("2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1"));
  ASSERT_NO_THROW(parse_fen("r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1"));
  ASSERT_NO_THROW(parse_fen("4k3/8/8/8/8/8/8/4K2R b K - 0 1"));
  ASSERT_NO_THROW(parse_fen("4k3/8/8/8/8/8/8/R3K3 b Q - 0 1"));
  ASSERT_NO_THROW(parse_fen("4k2r/8/8/8/8/8/8/4K3 b k - 0 1"));
  ASSERT_NO_THROW(parse_fen("r3k3/8/8/8/8/8/8/4K3 b q - 0 1"));
  ASSERT_NO_THROW(parse_fen("4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1"));
  ASSERT_NO_THROW(parse_fen("r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/8/8/8/8/6k1/4K2R b K - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/8/8/8/8/1k6/R3K3 b Q - 0 1"));
  ASSERT_NO_THROW(parse_fen("4k2r/6K1/8/8/8/8/8/8 b k - 0 1"));
  ASSERT_NO_THROW(parse_fen("r3k3/1K6/8/8/8/8/8/8 b q - 0 1"));
  ASSERT_NO_THROW(parse_fen("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1"));
  ASSERT_NO_THROW(parse_fen("r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1"));
  ASSERT_NO_THROW(parse_fen("r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1"));
  ASSERT_NO_THROW(parse_fen("r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1"));
  ASSERT_NO_THROW(parse_fen("1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1"));
  ASSERT_NO_THROW(parse_fen("2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1"));
  ASSERT_NO_THROW(parse_fen("r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("K7/8/2n5/1n6/8/8/8/k6N w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/8/2N5/1N6/8/8/8/K6n w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("K7/8/2n5/1n6/8/8/8/k6N b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/8/2N5/1N6/8/8/8/K6n b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("7k/RR6/8/8/8/8/rr6/7K w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("7k/RR6/8/8/8/8/rr6/7K b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("6kq/8/8/8/8/8/8/7K w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("6KQ/8/8/8/8/8/8/7k b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("K7/8/8/3Q4/4q3/8/8/7k w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("6qk/8/8/8/8/8/8/7K b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("6KQ/8/8/8/8/8/8/7k b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("K7/8/8/3Q4/4q3/8/8/7k b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/8/8/8/K7/P7/k7 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/8/8/8/7K/7P/7k w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("K7/p7/k7/8/8/8/8/8 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("7K/7p/7k/8/8/8/8/8 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/8/8/8/K7/P7/k7 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/8/8/8/7K/7P/7k b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("K7/p7/k7/8/8/8/8/8 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("7K/7p/7k/8/8/8/8/8 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/8/8/8/4k3/4P3/4K3 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("4k3/4p3/4K3/8/8/8/8/8 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/7k/7p/7P/7K/8/8 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/k7/p7/P7/K7/8/8 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/8/3p4/8/3P4/8/8/7K w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/7k/7p/7P/7K/8/8 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/k7/p7/P7/K7/8/8 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/8/3p4/8/3P4/8/8/7K b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("7k/3p4/8/8/3P4/8/8/K7 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("7k/8/8/3p4/8/8/3P4/K7 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/8/8/7p/6P1/8/8/K7 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/8/7p/8/8/6P1/8/K7 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/8/8/6p1/7P/8/8/K7 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/8/6p1/8/8/7P/8/K7 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/8/8/3p4/4p3/8/8/7K w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/8/3p4/8/8/4P3/8/7K w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("7k/3p4/8/8/3P4/8/8/K7 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("7k/8/8/3p4/8/8/3P4/K7 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/8/8/7p/6P1/8/8/K7 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/8/7p/8/8/6P1/8/K7 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/8/8/6p1/7P/8/8/K7 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/8/6p1/8/8/7P/8/K7 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/8/8/3p4/4p3/8/8/7K b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/8/3p4/8/8/4P3/8/7K b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("7k/8/8/p7/1P6/8/8/7K w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("7k/8/p7/8/8/1P6/8/7K w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("7k/8/8/1p6/P7/8/8/7K w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("7k/8/1p6/8/8/P7/8/7K w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/7p/8/8/8/8/6P1/K7 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/6p1/8/8/8/8/7P/K7 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("7k/8/8/p7/1P6/8/8/7K b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("7k/8/p7/8/8/1P6/8/7K b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("7k/8/8/1p6/P7/8/8/7K b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("7k/8/1p6/8/8/P7/8/7K b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/7p/8/8/8/8/6P1/K7 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("k7/6p1/8/8/8/8/7P/K7 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/Pk6/8/8/8/8/6Kp/8 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/Pk6/8/8/8/8/6Kp/8 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1"));
  ASSERT_NO_THROW(parse_fen("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1"));

  exit(EXIT_SUCCESS);
}

}

