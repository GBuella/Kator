
#ifndef KATOR_CHESS_GAME_H
#define KATOR_CHESS_GAME_H

#include "chess.h"
#include "move_list.h"

#include <vector>
#include <string>
#include <istream>
#include <memory>

namespace kator
{
namespace chess
{

class game_state
{
public:

  const chess::position& position;
  const real_player turn;
  const player whites_view;
  const move_list moves;
  const bool has_any_legal_moves;
  const unsigned half_moves;
  const unsigned full_moves;
  const bool in_check;
  const bool is_check_mate;
  const bool is_stale_mate;

  static std::unique_ptr<game_state> parse_fen(std::string);
  static std::unique_ptr<game_state> parse_fen(std::istream&);
  std::unique_ptr<game_state> make_move(move) const;

  virtual std::string to_FEN() const noexcept = 0;
  virtual std::string to_FEN_no_move_counts() const noexcept = 0;
  virtual std::string flipped_to_FEN() const = 0;
  virtual std::string flipped_to_FEN_no_move_counts() const = 0;
  
  virtual move parse_move(const std::string&) const = 0;
  virtual std::string print_move(move, move_notation) const = 0;
  virtual bool can_flip() const noexcept = 0;
  virtual piece piece_at(const sq_index&) const noexcept = 0;
  virtual piece piece_at(const rank&, const file&) const noexcept = 0;

  virtual ~game_state() noexcept;

protected:

  game_state(const chess::position&, unsigned hm, unsigned fm, real_player);

}; /* class game_state */

class game
{
public:

  game();
  void reset(std::unique_ptr<game_state>);
  void reset(std::string fen);
  void reset(std::istream& fen);
  void advance();
  bool is_at_last_state() const noexcept;
  void revert();
  bool is_at_first_state() const noexcept;
  void advance(move);
  const game_state& current_state() const noexcept;
  size_t length() const noexcept;
  real_player turn() const noexcept;
  const move_list& legal_moves() const noexcept;
  ~game();

private:

  std::vector<std::unique_ptr<game_state>> states;
  size_t current_index;
}; // class game

} /* namespace kator::chess */
} /* namespace kator */

#endif /* !defined(KATOR_CHESS_GAME_H) */

