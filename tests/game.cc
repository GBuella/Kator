
#include "gtest.h"
#include "chess/move.h"
#include "chess/game.h"
#include "chess/game_state.h"

using namespace ::kator;

TEST(chess_game, game)
{
  auto game = ::kator::game::create();

  ASSERT_EQ(size_t(1), game->length());
  ASSERT_EQ(white, game->turn());
  ASSERT_TRUE(game->is_at_first_state());
  ASSERT_TRUE(game->is_at_last_state());
  game->advance(move(d2, d4, piece::pawn, move::pawn_double_push));
  ASSERT_THROW(game->advance(move(d2, d4, piece::pawn, move::pawn_double_push)),
    std::exception);
  ASSERT_EQ(size_t(2), game->length());
  ASSERT_EQ(black, game->turn());
  ASSERT_FALSE(game->is_at_first_state());
  ASSERT_TRUE(game->is_at_last_state());
  game->advance(move(d7, d5, piece::pawn, move::pawn_double_push));
  ASSERT_EQ(white, game->turn());
  ASSERT_THROW(game->advance(move(d2, d4, piece::pawn, move::pawn_double_push)),
      std::exception);
  ASSERT_EQ(size_t(3), game->length());
  game->advance(move(c1, g5, piece::bishop));
  ASSERT_TRUE(game->is_at_last_state());
  ASSERT_EQ(black, game->turn());
  ASSERT_EQ(size_t(4), game->length());
  game->advance();
  ASSERT_EQ(black, game->turn());
  ASSERT_EQ(size_t(4), game->length());
  ASSERT_FALSE(game->is_at_first_state());
  ASSERT_TRUE(game->is_at_last_state());
  game->advance(move(b8, a6, piece::knight));
  game->advance(move(d1, d3, piece::queen));
  game->advance(move(a8, b8, piece::rook));
  game->advance(move(d3, e3, piece::queen));
  game->advance(move(d8, d6, piece::queen));
  ASSERT_TRUE(game->is_at_last_state());
  ASSERT_EQ(
    "1rb1kbnr/ppp1pppp/n2q4/3p2B1/3P4/4Q3/PPP1PPPP/RN2KBNR w KQk - 6 5",
    game->current_state().to_FEN());
  game->advance(move(b1, c3, piece::knight));
  game->advance(move(d6, b6, piece::queen));
  game->advance(castle_queenside);
  ASSERT_EQ(size_t(12), game->length());
  ASSERT_TRUE(game->is_at_last_state());
  ASSERT_FALSE(game->is_at_first_state());
  ASSERT_EQ(black, game->turn());
  game->revert();
  ASSERT_FALSE(game->is_at_last_state());
  ASSERT_FALSE(game->is_at_first_state());
  ASSERT_EQ(size_t(12), game->length());
  ASSERT_EQ(white, game->turn());
  game->advance();
  ASSERT_EQ(size_t(12), game->length());
  ASSERT_TRUE(game->is_at_last_state());
  ASSERT_FALSE(game->is_at_first_state());
  ASSERT_EQ(black, game->turn());
  game->advance(move(g8, h6, piece::knight));
  ASSERT_EQ(white, game->turn());
  game->advance(move(g1, f3, piece::knight));
  game->advance(move(g7, g6, piece::pawn));
  game->advance(move(c3, d5, piece::knight, piece::pawn));
  game->advance(move(f8, g7, piece::bishop));
  game->advance(move(e3, e7, piece::queen, piece::pawn));
  ASSERT_TRUE(game->is_at_last_state());
  ASSERT_EQ(black, game->turn());
  ASSERT_TRUE(game->current_state().is_check_mate);
  ASSERT_TRUE(game->current_state().in_check);
  ASSERT_FALSE(game->current_state().is_stale_mate);
  ASSERT_FALSE(game->current_state().has_any_legal_moves);

  game->reset();
  ASSERT_EQ(size_t(1), game->length());
  ASSERT_EQ(white, game->turn());
  ASSERT_TRUE(game->is_at_first_state());
  ASSERT_TRUE(game->is_at_last_state());
}

