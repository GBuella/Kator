
#ifndef KATOR_ENGINE_NODE_H
#define KATOR_ENGINE_NODE_H

#include "chess/position.h"
#include "chess/move_list.h"
#include "eval.h"

namespace kator
{
namespace engine
{

class node
{
public:

  // For creating a root node
  node(const position&);

private:

  position position;
  move_list moves;
  position_value alpha;
  position_value beta;
  std::array<move, 3> killers;

  node();

}; // class node

} // namespace kator::engine
} // namespace kator

#endif // !defined(KATOR_ENGINE_NODE_H)
