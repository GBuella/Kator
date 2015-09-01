
#ifndef KATOR_CHESS_GAME_STATE_H
#define KATOR_CHESS_GAME_STATE_H

#include "chess.h"
#include "move_list.h"
#include "position.h"

#include <string>
#include <istream>
#include <memory>

namespace kator
{

struct game_state
{

  const std::unique_ptr<const ::kator::position> position;
  const real_player turn;
  const position_player whites_view;
  const move_list moves;
  const bool has_any_legal_moves;
  const unsigned half_moves;
  const unsigned full_moves;
  const bool in_check;
  const bool is_check_mate;
  const bool is_stale_mate;
  const bool can_flip;
  const sq_index en_passant_target_square;

  game_state(const game_state&);

  static std::unique_ptr<game_state> parse_fen(std::string);
  static std::unique_ptr<game_state> parse_fen(std::istream&);

  std::unique_ptr<game_state> make_move(move) const;

  std::string to_FEN() const noexcept;
  std::string to_FEN_no_move_counts() const noexcept;
  std::string flipped_to_FEN() const;
  std::string flipped_to_FEN_no_move_counts() const;
  
  move parse_move(const std::string&) const;
  std::string print_move(move, move_notation) const;
  real_piece piece_at(const sq_index&) const noexcept;
  real_piece piece_at(const rank&, const file&) const noexcept;

private:

  game_state(const ::kator::position&,
             unsigned half_moves, unsigned full_moves,
             real_player, sq_index ep_target);
  game_state(std::unique_ptr<const ::kator::position>,
             unsigned half_moves, unsigned full_moves,
             real_player, sq_index ep_target);

}; /* class game_state */

} /* namespace kator */

#endif /* !defined(KATOR_CHESS_GAME_STATE_H) */
