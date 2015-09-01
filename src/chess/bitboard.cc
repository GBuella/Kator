
#include "bitboard.h"

#include <array>
#include <cstring>
#include <vector>
#include <stdexcept>

namespace kator
{
  
namespace
{

typedef std::remove_const< std::remove_reference<
            decltype(*bitboard::magical::attacks)>::type
        >::type sliding_attack_t;

#include "magics.inc"

static const std::array<sq_index::direction, 8> king_directions({
  {north, north + east, north + west, west,
  south, south + east, south + west, east}
});

static const std::array<sq_index::direction, 8> knigth_directions({
  {north + north + west,
  north + north + east,
  north + west + west,
  north + east + east,
  south + south + west,
  south + south + east,
  south + west + west,
  south + east + east}
});

struct sliding_direction
{
  sq_index::direction delta;
  bitboard pre_mask_edge;
  bitboard attack_edge;
};

typedef std::array<sliding_direction, 4> sliding_directions;

static const sliding_directions rook_directions = {{
  { east,  bitboard(file_h), bitboard(file_a) },
  { west,  bitboard(file_a), bitboard(file_h) },
  { north, bitboard(rank_8), bitboard(rank_1) },
  { south, bitboard(rank_1), bitboard(rank_8) }
}};

static const sliding_directions bishop_directions = {{
  { east + north,
    bitboard(file_h) | bitboard(rank_8),
    bitboard(file_a) | bitboard(rank_1)
  },
  { west + north,
    bitboard(file_a) | bitboard(rank_8),
    bitboard(file_h) | bitboard(rank_1)
  },
  { east + south,
    bitboard(file_h) | bitboard(rank_1),
    bitboard(file_a) | bitboard(rank_8)
  },
  { west + south,
    bitboard(file_a) | bitboard(rank_1),
    bitboard(file_h) | bitboard(rank_8)
  }
}};

void generate_simple_table(std::array<bitboard, 64>& attack_patterns,
                           const std::array<sq_index::direction, 8>& directions)
{
  attack_patterns.fill(bitboard::empty());
  for (auto index : sq_index::range()) {
    for (auto direction : directions) {
      if (index.stays_in_board(direction)) {
        attack_patterns[index.offset()].set_bit(index + direction);
      }
    }
  }
}

bitboard generate_ray(sq_index src_index,
                      bitboard occupied,
                      sliding_direction direction)
{
  bitboard result = bitboard::empty();
  sq_index index = src_index + direction.delta;

  while (index.is_valid() and !direction.attack_edge.is_bit_set(index)) {
    result.set_bit(index);
    if (occupied.is_bit_set(index)) {
      return result;
    }
    index += direction.delta;
  }
  return result;
}

bitboard generate_move_pattern(sq_index src_index,
                               bitboard occupied,
                               const sliding_directions& directions)
{
    return generate_ray(src_index, occupied, directions[0])
         | generate_ray(src_index, occupied, directions[1])
         | generate_ray(src_index, occupied, directions[2])
         | generate_ray(src_index, occupied, directions[3]);
}

void generate_bishop_patterns(std::array<bitboard, 64>& attack_patterns)
{
  for (auto index : sq_index::range()) {
    attack_patterns[index.offset()] =
      generate_move_pattern(index, bitboard::empty(), bishop_directions);
  }
}

void generate_rook_patterns(std::array<bitboard, 64>& attack_patterns)
{
  for (auto index : sq_index::range()) {
    attack_patterns[index.offset()] =
      bitboard(index.file()) | bitboard(index.rank());
  }
}

void add_rays(std::array<bitboard, 64*64>& rays,
              sq_index source,
              sq_index::direction direction)
{
  bitboard ray = bitboard::empty();
  sq_index index = source;

  while (index.stays_in_board(direction)) {
    index += direction;

    rays[source.offset() * 64 + index.offset()] = ray;
    rays[index.offset() * 64 + source.offset()] = ray;
    ray.set_bit(index);
  }
}

void generate_ray_constants(std::array<bitboard, 64*64>& rays)
{
  rays.fill(bitboard::empty());
  for (auto index : sq_index::range()) {
    add_rays(rays, index, south);
    add_rays(rays, index, west);
    add_rays(rays, index, south + west);
    add_rays(rays, index, south + east);
  }
}

bitboard generate_pre_mask_ray(sq_index index,
                               const sliding_direction& direction)
{
  bitboard result = bitboard::empty();
  index += direction.delta;

  while (!direction.pre_mask_edge.is_bit_set(index)) {
    result.set_bit(index);
    index += direction.delta;
  }
  return result;
}

uint64_t generate_pre_mask(sq_index source,
                           const sliding_directions& directions)
{
  bitboard result = bitboard::empty();

  for (auto direction : directions) {
    if (not direction.pre_mask_edge.is_bit_set(source)) {
      result.merge(generate_pre_mask_ray(source, direction));
    }
  }
  return result.to_uint64_t();
}

#ifdef KATOR_USE_PEXT_BITBOARD
uint64_t generate_post_mask(sq_index source,
                           const sliding_directions& directions)
{
  bitboard result = bitboard::empty();

  for (auto direction : directions) {
    result.merge(generate_ray(source, bitboard::empty(), direction));
  }
  return result.to_uint64_t();
}
#endif

void generate_occupancies(sq_index source,
                          std::vector<bitboard>& occupancies,
                          bitboard mask)
{
  bitboard subset = bitboard::empty();

  do {
    occupancies.push_back(subset | bitboard(source));
    mask.generate_next_subset(subset);
  } while (!subset.is_empty());
}

void generate_sliding_attacks(sq_index index,
                              std::vector<sliding_attack_t>& sliding_attack_table,
                              const bitboard::magical& item,
                              const std::vector<bitboard>& occupancies,
                              const sliding_directions& directions)
{
  std::vector<sliding_attack_t> attacks;

#ifdef KATOR_USE_PEXT_BITBOARD
  attacks.assign(occupancies.size(), 0);
#else
  attacks.assign(1 << (64 - item.shift), bitboard::empty());
#endif
  size_t max_offset = 0;
  for (auto occupied : occupancies) {
    size_t offset = item.offset(occupied.to_uint64_t());

    if (offset > max_offset) {
      max_offset = offset;
    }
#   ifdef KATOR_USE_PEXT_BITBOARD
      bitboard temp = generate_move_pattern(index, occupied, directions);
      attacks[offset] =
        static_cast<sliding_attack_t>(temp.pext(bitboard(item.post_mask)));
#   else
      if (offset >= attacks.size() or attacks[offset].is_nonempty()) {
        throw std::logic_error("invalid magic multiplier");
      }
      attacks[offset] = generate_move_pattern(index, occupied, directions);
#   endif
  }
  sliding_attack_table.insert(sliding_attack_table.end(), attacks.cbegin(),
                    attacks.cbegin() + static_cast<ptrdiff_t>(max_offset) + 1);
}

void generate_magics(std::array<bitboard::magical, 64>& magics,
                     const std::array<uint64_t, 64> multipliers,
                     const sliding_directions& directions)
{
  std::array<size_t, 64> attack_offsets;
  auto sliding_attack_table = new std::vector<sliding_attack_t>();

  for (auto index : sq_index::range()) {
    std::vector<bitboard> occupancies;
    bitboard::magical& item = magics[index.offset()];

    item.pre_mask = generate_pre_mask(index, directions);
    generate_occupancies(index, occupancies, bitboard(item.pre_mask));
    attack_offsets[index.offset()] = sliding_attack_table->size();

#   ifdef KATOR_USE_PEXT_BITBOARD
      (void)multipliers;
      item.post_mask = generate_post_mask(index, directions);
#   else
      item.multiplier = multipliers[index.offset()];
      item.shift = static_cast<uint64_t>(64 - bitboard(item.pre_mask).popcnt());
#   endif

    generate_sliding_attacks(index, *sliding_attack_table,
                             item, occupancies, directions);
  }
  for (size_t i = 0; i < 64; ++i) {
    magics[i].attacks = sliding_attack_table->data() + attack_offsets[i];
  }
}

}

std::array<bitboard, 64> bitboard::knight_table {{}};
std::array<bitboard, 64> bitboard::king_table {{}};
std::array<bitboard, 64> bitboard::bishop_pattern_table {{}};
std::array<bitboard, 64> bitboard::rook_pattern_table {{}};
std::array<bitboard, 64 * 64> bitboard::rays {{}};

std::array<bitboard::magical, 64> bitboard::magical::rook {{}};
std::array<bitboard::magical, 64> bitboard::magical::bishop {{}};

static bool lookup_tables_generated = false;

void bitboard_lookup_tables_init()
{
  bitboard::magical::lookup_tables_init();
}

void bitboard::magical::lookup_tables_init()
{
  if (lookup_tables_generated) {
    return;
  }

  generate_simple_table(king_table, king_directions);
  generate_simple_table(knight_table, knigth_directions);
  generate_bishop_patterns(bishop_pattern_table);
  generate_rook_patterns(rook_pattern_table);
  generate_ray_constants(rays);
  generate_magics(rook, rook_magic_multipliers, rook_directions);
  generate_magics(bishop, bishop_magic_multipliers, bishop_directions);

  lookup_tables_generated = true;
}

} /* namespace kator */
