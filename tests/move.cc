
#include "tests.h"
#include "chess/move.h"

static const char test_filename[] = "tests/move.cc";

namespace kator
{

[[noreturn]] void test_move()
{
  using namespace ::kator::chess;

  {
    ASSERT_NO_THROW(move(a2, a4, pawn, move::pawn_double_push));

    move push(a2, a4, pawn, move::pawn_double_push);

    ASSERT_EQ(a2, push.from);
    ASSERT_EQ(a4, push.to);
    ASSERT_TRUE(push.is_double_pawn_push());
    ASSERT_TRUE(push.is_pawn_push());
    ASSERT_EQ(pawn, push.result());
    ASSERT_EQ(nonpiece, push.captured_piece);
    ASSERT_TRUE(push.is_special_move());
    ASSERT_FALSE(push.is_capture());
    ASSERT_FALSE(push.is_en_passant());
    ASSERT_FALSE(push.is_promotion());
    ASSERT_FALSE(push.is_castle_queenside());
    ASSERT_FALSE(push.is_castle_kingside());
    ASSERT_FALSE(push.is_reversible());
    ASSERT_TRUE(push.is_irreversible());

    ASSERT_NO_THROW(push.flip());

    ASSERT_EQ(a7, push.from);
    ASSERT_EQ(a5, push.to);
    ASSERT_TRUE(push.is_double_pawn_push());
    ASSERT_TRUE(push.is_pawn_push());
    ASSERT_EQ(pawn, push.result());
    ASSERT_EQ(nonpiece, push.captured_piece);
    ASSERT_TRUE(push.is_special_move());
    ASSERT_FALSE(push.is_capture());
    ASSERT_FALSE(push.is_en_passant());
    ASSERT_FALSE(push.is_promotion());
    ASSERT_FALSE(push.is_castle_queenside());
    ASSERT_FALSE(push.is_castle_kingside());
    ASSERT_FALSE(push.is_reversible());
    ASSERT_TRUE(push.is_irreversible());
  }

  {
    ASSERT_NO_THROW(move(g4, g5, pawn));

    move push = move(g4, g5, pawn);

    ASSERT_EQ(g4, push.from);
    ASSERT_EQ(g5, push.to);
    ASSERT_FALSE(push.is_double_pawn_push());
    ASSERT_TRUE(push.is_pawn_push());
    ASSERT_EQ(nonpiece, push.captured_piece);
    ASSERT_FALSE(push.is_capture());
    ASSERT_EQ(pawn, push.result());
    ASSERT_EQ(nonpiece, push.captured_piece);
    ASSERT_FALSE(push.is_capture());
    ASSERT_FALSE(push.is_special_move());
    ASSERT_FALSE(push.is_en_passant());
    ASSERT_FALSE(push.is_promotion());
    ASSERT_FALSE(push.is_castle_queenside());
    ASSERT_FALSE(push.is_castle_kingside());
    ASSERT_FALSE(push.is_reversible());
    ASSERT_TRUE(push.is_irreversible());

  }

  {
    ASSERT_NO_THROW(move(e4, d5, pawn, knight));

    move capture(e4, d5, pawn, knight);

    ASSERT_EQ(e4, capture.from);
    ASSERT_EQ(d5, capture.to);
    ASSERT_FALSE(capture.is_double_pawn_push());
    ASSERT_FALSE(capture.is_pawn_push());
    ASSERT_EQ(knight, capture.captured_piece);
    ASSERT_TRUE(capture.is_capture());
    ASSERT_FALSE(capture.is_castle_queenside());
    ASSERT_FALSE(capture.is_castle_kingside());
    ASSERT_FALSE(capture.is_special_move());
    ASSERT_FALSE(capture.is_en_passant());
    ASSERT_FALSE(capture.is_promotion());
    ASSERT_FALSE(capture.is_reversible());
    ASSERT_TRUE(capture.is_irreversible());

    ASSERT_NO_THROW(move(e4, f6, bishop, knight));
  }

  {
    move capture = move(e4, f6, bishop, knight);

    ASSERT_EQ(e4, capture.from);
    ASSERT_EQ(f6, capture.to);
    ASSERT_FALSE(capture.is_double_pawn_push());
    ASSERT_FALSE(capture.is_pawn_push());
    ASSERT_EQ(knight, capture.captured_piece);
    ASSERT_TRUE(capture.is_capture());
    ASSERT_FALSE(capture.is_castle_queenside());
    ASSERT_FALSE(capture.is_castle_kingside());
    ASSERT_FALSE(capture.is_en_passant());
    ASSERT_FALSE(capture.is_promotion());
    ASSERT_FALSE(capture.is_reversible());
    ASSERT_TRUE(capture.is_irreversible());

    ASSERT_NO_THROW(capture.flip());

    ASSERT_EQ(e5, capture.from);
    ASSERT_EQ(f3, capture.to);
    ASSERT_FALSE(capture.is_double_pawn_push());
    ASSERT_EQ(knight, capture.captured_piece);
    ASSERT_TRUE(capture.is_capture());
  }

  {
    ASSERT_TRUE(castle_kingside.is_castle_kingside());
    ASSERT_FALSE(castle_kingside.is_castle_queenside());
    ASSERT_FALSE(castle_kingside.is_capture());
    ASSERT_TRUE(castle_kingside.is_special_move());

    ASSERT_TRUE(castle_queenside.is_castle_queenside());
    ASSERT_FALSE(castle_queenside.is_castle_kingside());
    ASSERT_FALSE(castle_queenside.is_capture());
    ASSERT_TRUE(castle_queenside.is_special_move());
  }

  {
    move ep(e4, d3, pawn, pawn, move::en_passant);

    ASSERT_EQ(e4, ep.from);
    ASSERT_EQ(d3, ep.to);
    ASSERT_FALSE(ep.is_pawn_push());
    ASSERT_FALSE(ep.is_double_pawn_push());
    ASSERT_TRUE(ep.is_special_move());
    ASSERT_TRUE(ep.is_capture());
    ASSERT_TRUE(ep.is_en_passant());
    ASSERT_FALSE(ep.is_promotion());
    ASSERT_EQ(pawn, ep.captured_piece);
    ASSERT_TRUE(ep.is_irreversible());
    ASSERT_FALSE(ep.is_reversible());
  }

  {
    move general(f4, d7, bishop);

    ASSERT_EQ(f4, general.from);
    ASSERT_EQ(d7, general.to);
    ASSERT_FALSE(general.is_double_pawn_push());
    ASSERT_FALSE(general.is_pawn_push());
    ASSERT_EQ(bishop, general.result());
    ASSERT_EQ(nonpiece, general.captured_piece);
    ASSERT_FALSE(general.is_special_move());
    ASSERT_FALSE(general.is_capture());
    ASSERT_FALSE(general.is_en_passant());
    ASSERT_FALSE(general.is_promotion());
    ASSERT_FALSE(general.is_castle_queenside());
    ASSERT_FALSE(general.is_castle_kingside());
    ASSERT_TRUE(general.is_reversible());
    ASSERT_FALSE(general.is_irreversible());
  }

  {
    move promotion(c7, c8, knight, move::promotion);

    ASSERT_EQ(c7, promotion.from);
    ASSERT_EQ(c8, promotion.to);
    ASSERT_FALSE(promotion.is_double_pawn_push());
    ASSERT_TRUE(promotion.is_pawn_push());
    ASSERT_EQ(knight, promotion.result());
    ASSERT_EQ(nonpiece, promotion.captured_piece);
    ASSERT_TRUE(promotion.is_special_move());
    ASSERT_FALSE(promotion.is_capture());
    ASSERT_FALSE(promotion.is_en_passant());
    ASSERT_TRUE(promotion.is_promotion());
    ASSERT_FALSE(promotion.is_castle_queenside());
    ASSERT_FALSE(promotion.is_castle_kingside());
    ASSERT_FALSE(promotion.is_reversible());
    ASSERT_TRUE(promotion.is_irreversible());
  }

  exit(EXIT_SUCCESS);
}

}

