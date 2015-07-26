
#ifndef KATOR_TESTS_H
#define KATOR_TESTS_H

namespace kator
{

namespace chess
{
class position;
struct game_state;
}

unsigned long perft(const chess::position&, unsigned depth);
unsigned long slow_perft(const chess::position&, unsigned depth);
unsigned long perft(const chess::game_state&, unsigned depth);
unsigned long slow_perft(const chess::game_state&, unsigned depth);

} /* namespace kator */

#endif // !defined(KATOR_TESTS_H)

