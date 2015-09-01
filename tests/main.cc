
#include "gtest.h"

#include "chess/bitboard.h"
#include "chess/chess.h"
#include "engine/eval.h"

std::ostream& operator<< (std::ostream& stream, ::kator::move move)
{
  return stream << move.from << move.to;
}

int main(int argc, char** argv)
{
  ::kator::bitboard_lookup_tables_init();
  ::kator::engine::position_value::default_initialize_lookup_tables();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
