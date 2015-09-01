
#include "gtest.h"
#include "chess/move.h"
#include "chess/game_state.h"

using namespace ::kator;

TEST(chess_game_state, parse_fen)
{
  std::unique_ptr<game_state> state;

  ASSERT_NO_THROW(game_state::parse_fen(starting_fen));

  state = parse_fen(starting_fen);

  ASSERT_EQ(white, state->turn);
  ASSERT_TRUE(state->can_flip);
  ASSERT_EQ(player_to_move, state->whites_view);
  ASSERT_FALSE(state->in_check);
  ASSERT_FALSE(state->is_check_mate);
  ASSERT_FALSE(state->is_stale_mate);
  ASSERT_TRUE(state->position->can_castle_queenside());
  ASSERT_TRUE(state->position->can_castle_kingside());
  ASSERT_TRUE(state->position->opponent_can_castle_queenside());
  ASSERT_TRUE(state->position->opponent_can_castle_kingside());
  ASSERT_EQ(unsigned(0), state->half_moves);
  ASSERT_EQ(unsigned(1), state->full_moves);

  ASSERT_NO_THROW(state = parse_fen(
      "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"));

  ASSERT_EQ(black, state->turn);
  ASSERT_EQ(player_opponent, state->whites_view);
  ASSERT_FALSE(state->can_flip);
  ASSERT_FALSE(state->in_check);
  ASSERT_FALSE(state->is_check_mate);
  ASSERT_FALSE(state->is_stale_mate);
  ASSERT_TRUE(state->position->can_castle_queenside());
  ASSERT_TRUE(state->position->can_castle_kingside());
  ASSERT_TRUE(state->position->opponent_can_castle_queenside());
  ASSERT_TRUE(state->position->opponent_can_castle_kingside());
  ASSERT_EQ(unsigned(0), state->half_moves);
  ASSERT_EQ(unsigned(1), state->full_moves);

  ASSERT_NO_THROW(state = parse_fen(
      "rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w KQkq b6 0 1"));

  ASSERT_EQ(white, state->turn);
  ASSERT_EQ(player_to_move, state->whites_view);
  ASSERT_FALSE(state->can_flip);
  ASSERT_FALSE(state->in_check);
  ASSERT_FALSE(state->is_check_mate);
  ASSERT_FALSE(state->is_stale_mate);
  ASSERT_TRUE(state->position->can_castle_queenside());
  ASSERT_TRUE(state->position->can_castle_kingside());
  ASSERT_TRUE(state->position->opponent_can_castle_queenside());
  ASSERT_TRUE(state->position->opponent_can_castle_kingside());
  ASSERT_EQ(unsigned(0), state->half_moves);
  ASSERT_EQ(unsigned(1), state->full_moves);

  ASSERT_NO_THROW(state = parse_fen(
      "rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w - - 0 1"));

  ASSERT_EQ(white, state->turn);
  ASSERT_EQ(player_to_move, state->whites_view);
  ASSERT_TRUE(state->can_flip);
  ASSERT_FALSE(state->in_check);
  ASSERT_FALSE(state->is_check_mate);
  ASSERT_FALSE(state->is_stale_mate);
  ASSERT_FALSE(state->position->can_castle_queenside());
  ASSERT_FALSE(state->position->can_castle_kingside());
  ASSERT_FALSE(state->position->opponent_can_castle_queenside());
  ASSERT_FALSE(state->position->opponent_can_castle_kingside());
  ASSERT_EQ(unsigned(0), state->half_moves);
  ASSERT_EQ(unsigned(1), state->full_moves);

  ASSERT_NO_THROW(state = parse_fen(
        "1n2k3/ppppp1pp/2qrn3/r7/2b1Kp2/8/PPPPPPPP/RNBQ1BNR w - -"));

  ASSERT_EQ(white, state->turn);
  ASSERT_EQ(player_to_move, state->whites_view);
  ASSERT_TRUE(state->can_flip);
  ASSERT_TRUE(state->in_check);
  ASSERT_TRUE(state->is_check_mate);
  ASSERT_FALSE(state->is_stale_mate);
  ASSERT_FALSE(state->position->can_castle_queenside());
  ASSERT_FALSE(state->position->can_castle_kingside());
  ASSERT_FALSE(state->position->opponent_can_castle_queenside());
  ASSERT_FALSE(state->position->opponent_can_castle_kingside());
  ASSERT_EQ(unsigned(0), state->half_moves);
  ASSERT_EQ(unsigned(1), state->full_moves);

  ASSERT_NO_THROW(state = parse_fen(
        "1n2k3/ppppp1pp/3rn3/r7/2b1Kp2/7q/7P/8 w - -"));

  ASSERT_EQ(white, state->turn);
  ASSERT_EQ(player_to_move, state->whites_view);
  ASSERT_TRUE(state->can_flip);
  ASSERT_FALSE(state->in_check);
  ASSERT_FALSE(state->is_check_mate);
  ASSERT_TRUE(state->is_stale_mate);
  ASSERT_FALSE(state->position->can_castle_queenside());
  ASSERT_FALSE(state->position->can_castle_kingside());
  ASSERT_FALSE(state->position->opponent_can_castle_queenside());
  ASSERT_FALSE(state->position->opponent_can_castle_kingside());
  ASSERT_EQ(unsigned(0), state->half_moves);
  ASSERT_EQ(unsigned(1), state->full_moves);

  ASSERT_NO_THROW(state = parse_fen(
        "1n2k3/ppppp1np/3r4/r4p2/2b1K3/7q/PPPPPPPP/RNBQ1BNR w - -"));

  ASSERT_EQ(white, state->turn);
  ASSERT_EQ(player_to_move, state->whites_view);
  ASSERT_TRUE(state->can_flip);
  ASSERT_TRUE(state->in_check);
  ASSERT_FALSE(state->is_check_mate);
  ASSERT_FALSE(state->is_stale_mate);
  ASSERT_FALSE(state->position->can_castle_queenside());
  ASSERT_FALSE(state->position->can_castle_kingside());
  ASSERT_FALSE(state->position->opponent_can_castle_queenside());
  ASSERT_FALSE(state->position->opponent_can_castle_kingside());
  ASSERT_EQ(unsigned(0), state->half_moves);
  ASSERT_EQ(unsigned(1), state->full_moves);

  ASSERT_NO_THROW(state = parse_fen(
        "r2bk1nr/ppp1pppp/3p3b/8/Q6n/2P3q1/PP1PPPPP/RNB1KBNR b KQk -"));

  ASSERT_EQ(black, state->turn);
  ASSERT_EQ(player_opponent, state->whites_view);
  ASSERT_TRUE(state->can_flip);
  ASSERT_TRUE(state->in_check);
  ASSERT_FALSE(state->is_check_mate);
  ASSERT_FALSE(state->is_stale_mate);
  ASSERT_FALSE(state->position->can_castle_queenside());
  ASSERT_TRUE(state->position->can_castle_kingside());
  ASSERT_TRUE(state->position->opponent_can_castle_queenside());
  ASSERT_TRUE(state->position->opponent_can_castle_kingside());
  ASSERT_EQ(unsigned(0), state->half_moves);
  ASSERT_EQ(unsigned(1), state->full_moves);

  ASSERT_NO_THROW(state = parse_fen(
        "rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w - - 3 8"));

  ASSERT_EQ(unsigned(3), state->half_moves);
  ASSERT_EQ(unsigned(8), state->full_moves);

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
}

TEST(chess_game_state, to_fen)
{
  auto state = parse_fen(starting_fen);

  ASSERT_EQ("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            state->to_FEN());
  ASSERT_EQ("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -",
            state->to_FEN_no_move_counts());
  ASSERT_EQ("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1",
            state->flipped_to_FEN());
  ASSERT_EQ("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq -",
            state->flipped_to_FEN_no_move_counts());

  state = parse_fen(
      "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
  ASSERT_EQ("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
            state->to_FEN());
  ASSERT_EQ("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3",
            state->to_FEN_no_move_counts());

  state = parse_fen(
      "rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w KQkq b6 0 1");
  ASSERT_EQ("rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w KQkq b6 0 1",
            state->to_FEN());
  ASSERT_EQ("rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w KQkq b6",
            state->to_FEN_no_move_counts());

  state = parse_fen(
      "rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w - - 0 1");
  ASSERT_EQ("rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w - - 0 1",
            state->to_FEN());
  ASSERT_EQ("rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w - -",
            state->to_FEN_no_move_counts());
  ASSERT_EQ("rnbqkbnr/pppp1ppp/8/4p3/1P6/8/P1PPPPPP/RNBQKBNR b - - 0 1",
            state->flipped_to_FEN());
  ASSERT_EQ("rnbqkbnr/pppp1ppp/8/4p3/1P6/8/P1PPPPPP/RNBQKBNR b - -",
            state->flipped_to_FEN_no_move_counts());

  state = parse_fen(
        "rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w - - 3 8");

  ASSERT_EQ(state->to_FEN(),
      "rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR w - - 3 8");
}

TEST(chess_game_state, parse_move)
{
  std::unique_ptr<game_state> state = parse_fen(starting_fen);

  ASSERT_THROW(state->parse_move("klmkmm"), invalid_move_string);
  ASSERT_THROW(state->parse_move(""), invalid_move_string);
  ASSERT_THROW(state->parse_move("00"), invalid_move_string);
  ASSERT_THROW(state->parse_move("a1a1"), invalid_move_string);
  ASSERT_THROW(state->parse_move("g7g1"), invalid_move_string);
  ASSERT_THROW(state->parse_move("e1e2"), invalid_move_string);
  ASSERT_THROW(state->parse_move("d7d6"), invalid_move_string);

  move push(a2, a4, piece::pawn, move::pawn_double_push);

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

TEST(chess_game_state, print_move)
{
  move_notation coordinate = move_notation::coordinate;
  move_notation SAN = move_notation::SAN;
  move_notation FAN = move_notation::FAN;

  std::unique_ptr<game_state> state = parse_fen(starting_fen);

  move push(a2, a4, piece::pawn);

  ASSERT_EQ("a2a4", state->print_move(push, coordinate));
  ASSERT_EQ("a4", state->print_move(push, SAN));
  ASSERT_EQ("a4", state->print_move(push, FAN));

  move general(g1, f3, piece::knight);

  ASSERT_EQ("g1f3", state->print_move(general, coordinate));
  ASSERT_EQ("Nf3", state->print_move(general, SAN));

  state = parse_fen(
    "r3k2r/p1ppqpb1/bnP1pnp1/4N3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");

  general = move(f3, f5, piece::queen);

  ASSERT_EQ("f3f5", state->print_move(general, coordinate));
  ASSERT_EQ("Qf5", state->print_move(general, SAN));

  general = move(f3, f6, piece::queen, piece::knight);

  ASSERT_EQ("f3f6", state->print_move(general, coordinate));
  ASSERT_EQ("Qxf6", state->print_move(general, SAN));

  move capture(c6, d7, piece::pawn, piece::pawn);

  ASSERT_EQ("c6d7", state->print_move(capture, coordinate));
  ASSERT_EQ("cxd7+", state->print_move(capture, SAN));

  capture = move(e5, g6, piece::knight, piece::pawn);

  ASSERT_EQ("e5g6", state->print_move(capture, coordinate));
  ASSERT_EQ("Nxg6", state->print_move(capture, SAN));

  capture = move(e2, a6, piece::bishop, piece::bishop);

  ASSERT_EQ("e2a6", state->print_move(capture, coordinate));
  ASSERT_EQ("Bxa6", state->print_move(capture, SAN));

  ASSERT_EQ("e1g1", state->print_move(white_castle_kingside, coordinate));
  ASSERT_EQ("O-O", state->print_move(white_castle_kingside, SAN));
  ASSERT_EQ("O-O", state->print_move(white_castle_kingside, FAN));
  ASSERT_EQ("e1c1", state->print_move(white_castle_queenside, coordinate));
  ASSERT_EQ("O-O-O", state->print_move(white_castle_queenside, SAN));
  ASSERT_EQ("e8g8", state->print_move(black_castle_kingside, coordinate));
  ASSERT_EQ("O-O", state->print_move(black_castle_kingside, SAN));
  ASSERT_EQ("e8c8", state->print_move(black_castle_queenside, coordinate));
  ASSERT_EQ("O-O-O", state->print_move(black_castle_queenside, SAN));

  state = parse_fen(
    "r3k2r/p1ppqpb1/bnP1pnp1/4N3/Pp2P3/2N2Q2/1PPBBPpP/R3K2R b KQkq a3 0 1");

  ASSERT_EQ("bxa3e.p.", state->print_move(
    move(b4, a3, piece::pawn, piece::pawn, move::en_passant), SAN));
  ASSERT_EQ("g1=Q+", state->print_move(
    move(g2, g1, piece::queen, move::promotion), SAN));
  ASSERT_EQ("gxh1=N", state->print_move(
    move(g2, h1, piece::knight, piece::rook, move::promotion), SAN));
  ASSERT_EQ("gxh1=R+", state->print_move(
    move(g2, h1, piece::rook, piece::rook, move::promotion), SAN));

  state = parse_fen(
    "r1k4r/p1ppqpb1/bnP1pNp1/8/Pp2P3/2N2Q2/1PPBBPpP/R3K2R w - - 0 1");

  ASSERT_EQ("Ncd5", state->print_move(move(c3, d5, piece::knight), SAN));
  ASSERT_EQ("Nfd5", state->print_move(move(f6, d5, piece::knight), SAN));

  state = parse_fen(
    "r1k4r/p1Npqpb1/bnP1pNp1/8/Pp2P3/2N2Q2/1PPBBPpP/R3K2R w - - 0 1");

  ASSERT_EQ("N3d5", state->print_move(move(c3, d5, piece::knight), SAN));
  ASSERT_EQ("Na2", state->print_move(move(c3, a2, piece::knight), SAN));
  ASSERT_EQ("N7d5", state->print_move(move(c7, d5, piece::knight), SAN));
  ASSERT_EQ("Nce8", state->print_move(move(c7, e8, piece::knight), SAN));
  ASSERT_EQ("Nxa8", state->print_move(
    move(c7, a8, piece::knight, piece::rook), SAN));
  ASSERT_EQ("Nfd5", state->print_move(move(f6, d5, piece::knight), SAN));

  state = parse_fen(
    "r1k4r/p1Npqp2/bnP1pNp1/3b4/Pp2P3/2N2Q2/1PPBBPpP/R3K2R w - - 0 1");

  ASSERT_EQ("N3xd5", state->print_move(
    move(c3, d5, piece::knight, piece::bishop), SAN));
  ASSERT_EQ("Na2", state->print_move(
    move(c3, a2,   piece::knight), SAN));
  ASSERT_EQ("N7xd5", state->print_move(
    move(c7, d5, piece::knight, piece::bishop), SAN));
  ASSERT_EQ("Nce8", state->print_move(
    move(c7, e8,  piece::knight), SAN));
  ASSERT_EQ("Nxa8", state->print_move(
    move(c7, a8,  piece::knight, piece::rook), SAN));
  ASSERT_EQ("Nfxd5", state->print_move(
    move(f6, d5, piece::knight, piece::bishop), SAN));
}

TEST(chess_game_state, make_move)
{
  auto state = parse_fen(starting_fen);

  state = state->make_move(move(a2, a4, piece::pawn, move::pawn_double_push));

  ASSERT_EQ(
    "rnbqkbnr/pppppppp/8/8/P7/8/1PPPPPPP/RNBQKBNR b KQkq a3 0 1",
    state->to_FEN());
  ASSERT_EQ(unsigned(0), state->half_moves);
  ASSERT_EQ(unsigned(1), state->full_moves);
  ASSERT_EQ(black, state->turn);
  ASSERT_EQ(a3, state->en_passant_target_square);

  state = state->make_move(move(d7, d5, piece::pawn, move::pawn_double_push));

  ASSERT_EQ(
    "rnbqkbnr/ppp1pppp/8/3p4/P7/8/1PPPPPPP/RNBQKBNR w KQkq d6 0 2",
    state->to_FEN());
  ASSERT_EQ(unsigned(0), state->half_moves);
  ASSERT_EQ(unsigned(2), state->full_moves);
  ASSERT_EQ(white, state->turn);
  ASSERT_EQ(d6, state->en_passant_target_square);

  state = state->make_move(move(b1, a3, piece::knight));

  ASSERT_EQ(
    "rnbqkbnr/ppp1pppp/8/3p4/P7/N7/1PPPPPPP/R1BQKBNR b KQkq - 1 2",
    state->to_FEN());

  ASSERT_EQ(unsigned(1), state->half_moves);
  ASSERT_EQ(unsigned(2), state->full_moves);
  ASSERT_EQ(black, state->turn);
  ASSERT_FALSE(state->en_passant_target_square.is_set());

  state = state->make_move(move(c8, f5, piece::bishop));

  ASSERT_EQ(
    "rn1qkbnr/ppp1pppp/8/3p1b2/P7/N7/1PPPPPPP/R1BQKBNR w KQkq - 2 3",
    state->to_FEN());

  ASSERT_EQ(unsigned(2), state->half_moves);
  ASSERT_EQ(unsigned(3), state->full_moves);
  ASSERT_EQ(white, state->turn);
  ASSERT_FALSE(state->en_passant_target_square.is_set());
}

