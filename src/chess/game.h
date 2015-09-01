
#ifndef KATOR_CHESS_GAME_H
#define KATOR_CHESS_GAME_H

#include "chess.h"
#include "move_list.h"

#include <memory>

namespace kator
{

struct game_state;

class game
{
public:

  static std::unique_ptr<game> create();
  static std::unique_ptr<game> create(std::unique_ptr<game_state>);

  virtual void reset() = 0;
  virtual void reset(std::unique_ptr<game_state>) = 0;
  virtual void reset(std::string FEN) = 0;
  virtual void advance() = 0;
  virtual bool is_at_last_state() const noexcept = 0;
  virtual void revert() = 0;
  virtual bool is_at_first_state() const noexcept = 0;
  virtual void advance(move) = 0;
  virtual const game_state& current_state() const noexcept = 0;
  virtual size_t length() const noexcept = 0;
  virtual real_player turn() const noexcept = 0;
  virtual const move_list& legal_moves() const noexcept = 0;
  virtual ~game();

}; /* class game */

} /* namespace kator */

#endif /* !defined(KATOR_CHESS_GAME_H) */

