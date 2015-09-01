
#include "move_list.h"
#include "position.h"

namespace kator
{

namespace
{

class move_generator
{
private:

  move* pmove;
  bitboard nonpinned;
  bool ep_special_pin;
  bitboard dest_mask;
  const kator::position& position;

public:

  move_generator(const kator::position& ctor_position, move* pm):
    pmove(pm),
    nonpinned(bitboard::universe()),
    dest_mask(compl ctor_position.map_of(player_to_move)),
    position(ctor_position)
  { }

  move_generator(const kator::position& ctor_position,
                 move* pm,
                 bitboard victims):
    pmove(pm),
    nonpinned(bitboard::universe()),
    dest_mask(victims),
    position(ctor_position)
  { }

  const move* current_pointer() const
  {
    return pmove;
  }

private:

  bitboard occupied() const
  {
    return position.occupied();
  }

  template<typename type>
  bitboard map(type item) const
  {
    return position.map_of(item);
  }

  template<typename type, typename... types>
  bitboard map(type arg, types... tail) const
  {
    return union_of(map(arg), map(tail...));
  }

  piece piece_at(sq_index index)
  {
    return position.piece_at(index);
  }

  void add_simple_move(sq_index from, sq_index to, piece result)
  {
    *pmove++ = move(from, to, result);
  }

  void add_simple_move(sq_index from, sq_index to,
                       piece result, piece captured)
  {
    *pmove++ = move(from, to, result, captured, move::general);
  }

  void add_promotions(sq_index from, sq_index to)
  {
    *pmove++ = move(from, to, piece::queen, move::promotion);
    *pmove++ = move(from, to, piece::knight, move::promotion);
    *pmove++ = move(from, to, piece::bishop, move::promotion);
    *pmove++ = move(from, to, piece::rook, move::promotion);
  }

  void add_promotions(sq_index from, sq_index to, piece captured)
  {
    *pmove++ = move(from, to, piece::queen, captured, move::promotion);
    *pmove++ = move(from, to, piece::knight, captured, move::promotion);
    *pmove++ = move(from, to, piece::bishop, captured, move::promotion);
    *pmove++ = move(from, to, piece::rook, captured, move::promotion);
  }

  void add_pawn_push(sq_index from, sq_index to)
  {
    if (to.rank() == rank_8) {
      add_promotions(from, to);
    }
    else {
      add_simple_move(from, to, piece::pawn);
    }
  }

  void add_pawn_push_strict(sq_index from, sq_index to)
  {
    add_simple_move(from, to, piece::pawn);
  }

  void add_pawn_push_strict(sq_index from)
  {
    add_simple_move(from, north_of(from), piece::pawn);
  }

  void add_pawn_double_push(sq_index from, sq_index to)
  {
    *pmove++ = move(from, to, piece::pawn, move::pawn_double_push);
  }

  void add_pawn_double_push(sq_index from)
  {
    add_pawn_double_push(from, north_of(north_of(from)));
  }

  void add_pawn_capture(sq_index from, sq_index to)
  {
    piece captured = piece_at(to);

    if (to.rank() == rank_8) {
      add_promotions(from, to, captured);
    }
    else {
      add_simple_move(from, to, piece::pawn, captured);
    }
  }

  void add_en_passant(sq_index from)
  {
    *pmove++ = move(from, north_of(position.ep_index()),
                    piece::pawn, piece::pawn,
                    move::en_passant);
  }

  void add_general_move(sq_index from, sq_index to, piece result)
  {
    add_simple_move(from, to, result, piece_at(to));
  }

  void add_general_moves(sq_index from, bitboard to_map, piece result)
  {
    for (auto to : to_map) {
      add_general_move(from, to, result);
    }
  }

  void add_general_moves(sq_index from, bitboard to_map)
  {
    piece result = piece_at(from);

    for (auto to : to_map) {
      add_general_move(from, to, result);
    }
  }

  void add_castle_kingside()
  {
    *pmove++ = castle_kingside;
  }

  void add_castle_queenside()
  {
    *pmove++ = castle_queenside;
  }

  bool is_pin(bitboard ray) const
  {
    return are_disjoint(ray, map(player_opponent)) and
           intersection_of(ray, map(player_to_move)).is_singular();
  }

  void handle_pawn_pinned_by_bishop(bitboard ray,
                                          sq_index pinned_i,
                                          sq_index pinner_i)
  {
    if (pinner_i == north_of(left_of(pinned_i)) or
        pinner_i == north_of(right_of(pinned_i)))
    {
      add_pawn_capture(pinned_i, pinner_i);
    }
    else if (position.has_en_passant_square()) {
      sq_index epi = position.ep_index();

      if (right_of(pinned_i) == epi or left_of(pinned_i) == epi) {
        if (ray.is_bit_set(north_of(epi))) {
          add_en_passant(pinned_i);
        }
      }
    }
  }

  void handle_pawn_pinned_by_rook(bitboard ray, sq_index pinned_i)
  {
    if (ray.is_bit_set(north_of(pinned_i))) {
      add_pawn_push_strict(pinned_i);
      if (pinned_i.rank() == rank_2) {
        if (ray.is_bit_set(north_of(north_of(pinned_i)))) {
          add_pawn_double_push(pinned_i);
        }
      }
    }
  }

  template<bool is_bishop>
  void handle_pawn_pinned_by(bitboard ray, sq_index pinned_i, sq_index pinner_i)
  {
    if (is_bishop) {
      handle_pawn_pinned_by_bishop(ray, pinned_i, pinner_i);
    }
    else {
      handle_pawn_pinned_by_rook(ray, pinned_i);
    }
  }


  template<bool is_bishop>
  static bitboard sliders(const kator::position& position)
  {
    if (is_bishop) {
      return position.bishop_queen_map();
    }
    else {
      return position.rook_queen_map();
    }
  }

  template<bool is_bishop>
  void handle_pinned_by(bitboard ray, sq_index pinner_i)
  {
    nonpinned = intersection_of(nonpinned, compl ray);
    if (position.in_check()) {
      return;
    }
    sq_index pinned_i =
      intersection_of(ray, map(player_to_move)).lsb_index();

    if (not are_disjoint(sliders<is_bishop>(position), ray)) {
      ray.set_bit(pinner_i);
      ray.reset_bit(pinned_i);
      add_general_moves(pinned_i, ray);
    }
    else if (not are_disjoint(map(pawn), ray)) {
      handle_pawn_pinned_by<is_bishop>(ray, pinned_i, pinner_i);
    }
  }

  void treat_en_passant_indirect_pin(bitboard ray)
  {
    if (position.has_en_passant_square()) {
      if (intersection_of(ray, occupied()) == bitboard(position.ep_index())) {
        ep_special_pin = true;
      }
    }
  }

  void handle_bishop_pins()
  {
    sq_index king_i = position.king_index();
    bitboard potential_pinners = position.opponent_bishop_queen_map();

    potential_pinners.filter_by(bitboard::bishop_pattern(king_i));
    for (auto pinner_i : potential_pinners) {
      bitboard ray = bitboard::ray_between(king_i, pinner_i);

      if (is_pin(ray)) {
        handle_pinned_by<true>(ray, pinner_i);
      }
      else {
        treat_en_passant_indirect_pin(ray);
      }
    }
  }

  void treat_en_passant_rank_pin(bitboard ray)
  {
    if (ep_special_pin or !position.has_en_passant_square()) {
      return;
    }
    if (ray.is_bit_non_set(position.ep_index())) {
      return;
    }
    if (intersection_of(ray, occupied(), bitboard(rank_5)).popcnt() == 2) {
      ep_special_pin = true;
    }
  }

  void handle_rook_pins()
  {
    sq_index king_i = position.king_index();
    bitboard potential_pinners = position.opponent_rook_queen_map();

    potential_pinners.filter_by(bitboard::rook_pattern(king_i));
    for (auto pinner_i : potential_pinners) {
      bitboard ray = bitboard::ray_between(king_i, pinner_i);

      if (is_pin(ray)) {
        handle_pinned_by<false>(ray, pinner_i);
      }
      else {
        treat_en_passant_rank_pin(ray);
      }
    }
  }

  void gen_castle_kingside()
  {
    if (position.can_castle_kingside()
        and position.is_any_unoccupied(f1, g1)
        and not position.is_any_attacked_by(player_opponent, f1, g1))
    {
      add_castle_kingside();
    }
  }

  void gen_castle_queenside()
  {
    if (position.can_castle_queenside()
        and position.is_any_unoccupied(b1, c1, d1)
        and not position.is_any_attacked_by(player_opponent, c1, d1))
    {
      add_castle_queenside();
    }
  }

  bool can_en_passant_at_all()
  {
    return position.has_en_passant_square()
           and not ep_special_pin
           and dest_mask.is_bit_set(north_of(position.ep_index()));
  }

  bool can_en_passant_from_left()
  {
    return position.ep_index().file() != file_a
           and position.map_of(pawn).is_bit_set(left_of(position.ep_index()))
           and nonpinned.is_bit_set(left_of(position.ep_index()));
  }

  bool can_en_passant_from_right()
  {
    return position.ep_index().file() != file_h
           and position.map_of(pawn).is_bit_set(right_of(position.ep_index()))
           and nonpinned.is_bit_set(right_of(position.ep_index()));
  }

  void gen_en_passant()
  {
    if (can_en_passant_at_all()) {
      if (can_en_passant_from_left()) {
        add_en_passant(left_of(position.ep_index()));
      }
      if (can_en_passant_from_right()) {
        add_en_passant(right_of(position.ep_index()));
      }
    }
  }

  void gen_knight_moves()
  {
    for (auto from : intersection_of(map(knight), nonpinned)) {
      bitboard to_map = bitboard::knight_attacks(from);

      to_map.filter_by(dest_mask);
      add_general_moves(from, to_map, piece::knight);
    }
  }

  void gen_pawn_pushes()
  {
    bitboard pawns = intersection_of(map(pawn), nonpinned);
    bitboard pushes = intersection_of(north_of(pawns), compl occupied());

    for (auto push : intersection_of(pushes, dest_mask)) {
      add_pawn_push(south_of(push), push);
    }
    pushes = north_of(pushes);
    pushes.filter_by(bitboard(rank_4), compl occupied());
    for (auto push : intersection_of(pushes, dest_mask)) {
      add_pawn_double_push(south_of(south_of(push)), push);
    }
  }

  void gen_pawn_captures()
  {
    bitboard pawns = intersection_of(map(pawn), nonpinned);
    bitboard attacks = bitboard::pawn_attacks_left(pawns);

    attacks = intersection_of(attacks, map(player_opponent), dest_mask);
    for (auto to : attacks) {
      add_pawn_capture(right_of(south_of(to)), to);
    }
    attacks = bitboard::pawn_attacks_right(pawns);
    attacks = intersection_of(attacks, map(player_opponent), dest_mask);
    for (auto to : intersection_of(attacks, map(player_opponent))) {
      add_pawn_capture(left_of(south_of(to)), to);
    }
  }

  void gen_sliding_moves(const magic_array& magics, bitboard pieces)
  {
    for (auto from : intersection_of(pieces, nonpinned)) {
      bitboard to_map = bitboard::sliding_attacks(magics, occupied(), from);

      add_general_moves(from, intersection_of(to_map, dest_mask));
    }
  }

  void gen_king_moves()
  {
    bitboard to_map = intersection_of(
                        bitboard::king_attacks(position.king_index()),
                        compl map(player_to_move),
                        compl position.attacks_of(player_opponent));

    add_general_moves(position.king_index(), to_map, piece::king);
  }

public:

  void run()
  {
    if (not position.has_multiple_checkers()) {
      ep_special_pin = false;

      handle_bishop_pins();
      handle_rook_pins();
      if (position.in_check()) {
        dest_mask.filter_by(position.king_attack_map());
      }
      else {
        gen_castle_kingside();
        gen_castle_queenside();
      }
      gen_en_passant();
      gen_knight_moves();
      gen_pawn_pushes();
      gen_pawn_captures();
      gen_sliding_moves(bitboard::magical::rook, map(rook, queen));
      gen_sliding_moves(bitboard::magical::bishop, map(bishop, queen));
    }
    gen_king_moves();
  }

  void run_captures()
  {
    assert(not position.in_check());

    ep_special_pin = false;
    handle_bishop_pins();
    handle_rook_pins();

    if (not are_disjoint(dest_mask, map(opponent_pawn))) {
      gen_en_passant();
    }
    gen_knight_moves();
    gen_pawn_captures();
    if (intersection_of(position.attacks_of(rook), dest_mask).is_nonempty()) {
      gen_sliding_moves(bitboard::magical::rook, map(rook));
    }
    if (intersection_of(position.attacks_of(queen), dest_mask).is_nonempty()) {
      gen_sliding_moves(bitboard::magical::rook, map(rook));
      gen_sliding_moves(bitboard::magical::bishop, map(queen));
    }
    if (intersection_of(position.attacks_of(bishop), dest_mask).is_nonempty()) {
      gen_sliding_moves(bitboard::magical::bishop, map(bishop));
    }
    gen_king_moves();
  }

}; /* inline class move_generator */

} /* anonym namespace */

move_list::move_list(const position& position)
{
  move_generator generator(position, moves);

  generator.run();
  size = static_cast<size_t>(generator.current_pointer() - moves);
}

move_list::move_list(const position& position, bitboard victims)
{
  move_generator generator(position, moves, victims);

  generator.run();
  size = static_cast<size_t>(generator.current_pointer() - moves);
}

size_t move_list::count() const noexcept
{
  return size;
}

move move_list::operator[] (size_t index) const noexcept
{
  return moves[index];
}

move move_list::iterator::operator *() const noexcept
{
  return *p;
}

move_list::iterator& move_list::iterator::operator ++() noexcept
{
  ++p;
  return *this;
}

bool move_list::iterator::operator !=(
          const move_list::iterator& other) const noexcept
{
  return p != other.p;
}

move_list::iterator move_list::begin() const noexcept
{
  return iterator(&(moves[0]));
}

move_list::iterator move_list::end() const noexcept
{
  return iterator(moves + size);
}

void move_list::flip() noexcept
{
  for (size_t i = 0; i < size; ++i) {
    moves[i].flip();
  }
}

move_list::move_list(const position& position, real_player player_to_move):
  move_list(position)
{
  if (player_to_move == black) {
    flip();
  }
}

move move_list::first() const noexcept
{
  return moves[0];
}

bool move_list::contains(move move) const noexcept
{
  for (auto item : *this) {
    if (item == move) {
      return true;
    }
  }
  return false;
}

} /* namespace kator */

