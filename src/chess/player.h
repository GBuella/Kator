
#ifndef KATOR_CHESS_PLAYER_H
#define KATOR_CHESS_PLAYER_H

namespace kator
{
namespace chess
{

enum class player
{
  to_move = 0,
  opponent = 1 
};

enum class real_player
{
  white, black
};

static inline player opponent_of(player arg)
{
  return static_cast<player>(static_cast<int>(arg) ^ 1);
}

static inline real_player opponent_of(real_player arg)
{
  return (arg == real_player::white) ? real_player::black : real_player::white;
}

static constexpr real_player white = real_player::white;
static constexpr real_player black = real_player::black;

} /* namespace kator::chess */
} /* namespace kator */

#endif /* !defined(KATOR_CHESS_PLAYER_H) */
