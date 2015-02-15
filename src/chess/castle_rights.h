
#ifndef KATOR_CHESS_CASTLE_RIGHTS_H
#define KATOR_CHESS_CASTLE_RIGHTS_H

#include "chess.h"
#include "move.h"

#include <array>
#include <exception>

namespace kator
{
namespace chess
{

class castle_rights
{
public:

  enum class side {
    queenside = 0,
    kingside = 1,
    opponent_queenside = 2,
    opponent_kingside = 3,
  };

  bool can_castle_queenside() const;
  bool opponent_can_castle_queenside() const;
  bool can_castle_kingside() const;
  bool opponent_can_castle_kingside() const;
  bool opponent_can_castle_any() const;
  bool can_castle_any() const;

  castle_rights(std::istream& FEN_stream);
  castle_rights(std::string FEN_string);
  std::string generate_castle_FEN(real_player point_of_view) const;

protected:

  bool clears_right_queenside(move) const;
  bool clears_right_opponent_queenside(move) const;
  bool clears_right_kingside(move) const;
  bool clears_right_opponent_kingside(move) const;

  void clear_castle_right(side);

  enum class leave { uninitialized };
  castle_rights(leave) {}
  castle_rights(const castle_rights&, player point_of_view);

  void flip();
  castle_rights castle_flipped() const noexcept;


private:

  std::array<bool, 4> rights;
  castle_rights() {}

};

inline bool castle_rights::can_castle_queenside() const
{
  return rights[static_cast<unsigned>(side::queenside)];
}

inline bool castle_rights::opponent_can_castle_queenside() const
{
  return rights[static_cast<unsigned>(side::opponent_queenside)];
}

inline bool castle_rights::can_castle_kingside() const
{
  return rights[static_cast<unsigned>(side::kingside)];
}

inline bool castle_rights::opponent_can_castle_kingside() const
{
  return rights[static_cast<unsigned>(side::opponent_kingside)];
}

inline bool castle_rights::opponent_can_castle_any() const
{
  return opponent_can_castle_queenside() or opponent_can_castle_kingside();
}

inline bool castle_rights::can_castle_any() const
{
  return can_castle_queenside() or can_castle_kingside();
}

inline void castle_rights::flip()
{
  std::swap(rights[0], rights[2]);
  std::swap(rights[1], rights[3]);
}

inline castle_rights castle_rights::castle_flipped() const noexcept
{
  castle_rights result;

  result.rights[0] = rights[2];
  result.rights[1] = rights[3];
  result.rights[2] = rights[0];
  result.rights[3] = rights[1];
  return result;
}

inline
castle_rights::castle_rights(const castle_rights& other, player point_of_view):
  castle_rights(other)
{
  if (point_of_view == player::opponent) {
    flip();
  }
}

inline bool castle_rights::clears_right_queenside(move move) const
{
  return can_castle_queenside() and (move.to == a1);
}

inline bool castle_rights::clears_right_opponent_queenside(move move) const
{
  return opponent_can_castle_queenside() 
         and (move.from == e8 or move.from == a8);
}

inline bool castle_rights::clears_right_kingside(move move) const
{
  return can_castle_kingside() and (move.to == h1);
}

inline bool castle_rights::clears_right_opponent_kingside(move move) const
{
  return opponent_can_castle_kingside()
         and (move.from == e8 or move.from == h8);
}

inline void castle_rights::clear_castle_right(side side)
{
  rights[static_cast<unsigned>(side)] = false;
}

static inline castle_rights::side opponent_of(castle_rights::side side)
{
  return static_cast<castle_rights::side>(static_cast<unsigned>(side) ^ 1);
}

} /* namespace kator::chess */
} /* namespace kator */

#endif /* !define(KATOR_CHESS_CASTLE_RIGHTS_H) */
