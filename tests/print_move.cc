
#include <memory>

#include "tests.h"
#include "chess/game.h"
#include "chess/move.h"

static const char test_filename[] = "tests/print_move.cc";

namespace kator
{

[[noreturn]] void test_print_move()
{
  using namespace ::kator::chess;

  move_notation coordinate = move_notation::coordinate;
  move_notation SAN = move_notation::SAN;
  move_notation FAN = move_notation::FAN;

  std::unique_ptr<game_state> state = parse_fen(starting_fen);

  move push(a2, a4, pawn);

  ASSERT_EQ("a2a4", state->print_move(push, coordinate));
  ASSERT_EQ("a4", state->print_move(push, SAN));
  ASSERT_EQ("a4", state->print_move(push, FAN));

  move general(g1, f3, knight);

  ASSERT_EQ("g1f3", state->print_move(general, coordinate));
  ASSERT_EQ("Nf3", state->print_move(general, SAN));

  state = parse_fen(
      "r3k2r/p1ppqpb1/bnP1pnp1/4N3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");

  general = move(f3, f5, queen);

  ASSERT_EQ("f3f5", state->print_move(general, coordinate));
  ASSERT_EQ("Qf5", state->print_move(general, SAN));

  general = move(f3, f6, queen, knight);

  ASSERT_EQ("f3f6", state->print_move(general, coordinate));
  ASSERT_EQ("Qxf6", state->print_move(general, SAN));

  move capture(c6, d7, pawn, pawn);

  ASSERT_EQ("c6d7", state->print_move(capture, coordinate));
  ASSERT_EQ("cxd7+", state->print_move(capture, SAN));

  capture = move(e5, g6, knight, pawn);

  ASSERT_EQ("e5g6", state->print_move(capture, coordinate));
  ASSERT_EQ("Nxg6", state->print_move(capture, SAN));

  capture = move(e2, a6, bishop, bishop);

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

  ASSERT_EQ("bxa3e.p.",
      state->print_move(move(b4, a3, pawn, pawn, move::en_passant), SAN));
  ASSERT_EQ("g1=Q+",
      state->print_move(move(g2, g1, queen, move::promotion), SAN));
  ASSERT_EQ("gxh1=N",
      state->print_move(move(g2, h1, knight, rook, move::promotion), SAN));
  ASSERT_EQ("gxh1=R+",
      state->print_move(move(g2, h1, rook, rook, move::promotion), SAN));

  state = parse_fen(
      "r1k4r/p1ppqpb1/bnP1pNp1/8/Pp2P3/2N2Q2/1PPBBPpP/R3K2R w - - 0 1");

  ASSERT_EQ("Ncd5", state->print_move(move(c3, d5, knight), SAN));
  ASSERT_EQ("Nfd5", state->print_move(move(f6, d5, knight), SAN));

  state = parse_fen(
      "r1k4r/p1Npqpb1/bnP1pNp1/8/Pp2P3/2N2Q2/1PPBBPpP/R3K2R w - - 0 1");

  ASSERT_EQ("N3d5", state->print_move(move(c3, d5, knight), SAN));
  ASSERT_EQ("Na2", state->print_move(move(c3, a2, knight), SAN));
  ASSERT_EQ("N7d5", state->print_move(move(c7, d5, knight), SAN));
  ASSERT_EQ("Nce8", state->print_move(move(c7, e8, knight), SAN));
  ASSERT_EQ("Nxa8", state->print_move(move(c7, a8, knight, rook), SAN));
  ASSERT_EQ("Nfd5", state->print_move(move(f6, d5, knight), SAN));

  state = parse_fen(
      "r1k4r/p1Npqp2/bnP1pNp1/3b4/Pp2P3/2N2Q2/1PPBBPpP/R3K2R w - - 0 1");

  ASSERT_EQ("N3xd5", state->print_move(move(c3, d5, knight, bishop), SAN));
  ASSERT_EQ("Na2", state->print_move(move(c3, a2,   knight), SAN));
  ASSERT_EQ("N7xd5", state->print_move(move(c7, d5, knight, bishop), SAN));
  ASSERT_EQ("Nce8", state->print_move(move(c7, e8,  knight), SAN));
  ASSERT_EQ("Nxa8", state->print_move(move(c7, a8,  knight, rook), SAN));
  ASSERT_EQ("Nfxd5", state->print_move(move(f6, d5, knight, bishop), SAN));

  exit(EXIT_SUCCESS);
}

}

