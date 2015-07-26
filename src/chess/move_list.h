
#ifndef KATOR_MOVE_LIST_H
#define KATOR_MOVE_LIST_H

#include "move.h"

namespace kator
{
namespace engine
{
class node;
}

namespace chess
{

class move_list
{

  move moves[219];
  size_t size;

public:

  class iterator
  {
    const move* p;

    iterator(const move *m) : p(m) {}

  public:

    move operator *() const noexcept;
    iterator& operator ++() noexcept;
    bool operator !=(const iterator& other) const noexcept;

    friend move_list;

  };

  const move* data() const noexcept
  {
    return moves;
  }

  move_list(const position&) noexcept;
  move_list(const position&, bitboard capture_targets) noexcept;
  move_list(const position&, player whites_view) noexcept;
  size_t count() const noexcept;
  iterator begin() const noexcept;
  iterator end() const noexcept;
  move first() const noexcept;
  bool contains(move) const noexcept;

  friend position;
  friend game_state;

private:

  move_list(): size(0) {}
  void flip() noexcept;

protected:

  move operator[] (size_t index) const noexcept;

  friend engine::node;
}; /* class move_list */

} /* namespace kator::chess */
} /* namespace kator */

#endif /* !defined(KATOR_MOVE_LIST_H) */
