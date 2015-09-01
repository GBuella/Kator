
#ifndef KATOR_TESTS_H
#define KATOR_TESTS_H

namespace kator
{

class position;
class position_noz;
struct game_state;

unsigned long perft(const position&, unsigned depth);
unsigned long slow_perft(const position&, unsigned depth);
unsigned long perft(const game_state&, unsigned depth);
unsigned long slow_perft(const game_state&, unsigned depth);

} /* namespace kator */

#endif // !defined(KATOR_TESTS_H)

