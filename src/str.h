
#ifndef KATOR_STR_H
#define KATOR_STR_H

#include <string>

namespace kator
{

struct game_state;

namespace str
{

bool has_only(const std::string& str, const char allowed[]) noexcept;
bool has_duplicates(const std::string&) noexcept;
bool contains(const std::string&, char) noexcept;
unsigned parse_uint(const std::string&, unsigned min, unsigned max);
void cleanup_move_string(std::string&);

extern const char u_alpha[];
extern const char u_beta[];
extern const char u_mu[];

std::string draw_game_state(const game_state&, bool use_figurines);

class unicode_table
{
public:
  unicode_table(size_t w): width(w) {}

  static const char upper_left[];
  static const char upper_right[];
  static const char lower_left[];
  static const char lower_right[];
  static const char horizontal[];
  static const char vertical[];
  static const char vertical_horizontal[];
  static const char horizontal_down[];
  static const char horizontal_up[];
  static const char vertical_right[];
  static const char vertical_left[];


  std::string upper_border() const noexcept;
  std::string lower_border() const noexcept;
  std::string row_separator() const noexcept;

private:
  size_t width;
};

} /* namespace kator::str */
} /* namespace kator */

#endif /* !defined(KATOR_STR_H) */
