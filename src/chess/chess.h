
#ifndef KATOR_CHESS_H
#define KATOR_CHESS_H

#include <exception>
#include <memory>
#include <ostream>

namespace kator
{

class bitboard;
class position;
struct move;
class move_list;
struct game_state;
class game;
class sq_index;

enum class move_notation
{
  coordinate,
  SAN,
  FAN
};

enum class piece : unsigned
{
  pawn =            2,
  rook =            4,
  king =            6,
  bishop =          8,
  knight =          10,
  queen =           12   //  bishop | rook == 4 | 8
};

enum square : unsigned
{
  nonpiece =        0,
  pawn =            2,
  opponent_pawn =   3,
  rook =            4,
  opponent_rook =   5,
  king =            6,
  opponent_king =   7,
  bishop =          8,
  opponent_bishop = 9,
  knight =          10,
  opponent_knight = 11,
  queen =           12,   //  bishop | rook == 4 | 8
  opponent_queen =  13
};

constexpr unsigned offset(piece piece)
{
  return static_cast<unsigned>(piece);
}

class all_piece_types
{
public:
  constexpr all_piece_types()
  {
  }

  class iterator
  {
    int value;

    constexpr iterator(int start): value(start) {}

    public:

    bool operator != (const iterator& other) const
    {
      return other.value != value;
    }

    bool operator == (const iterator& other) const
    {
      return other.value == value;
    }

    piece operator* () const
    {
      return static_cast<piece>(value);
    }

    const iterator& operator++ ()
    {
      value += 2;
      return *this;
    }

    friend class all_piece_types;
  };

  constexpr iterator begin() const
  {
    return iterator(2);
  }

  constexpr iterator end() const
  {
    return iterator(14);
  }

}; /* class all_piece_types */

constexpr size_t piece_array_size = 14;

enum class real_player
{
  white, black
};

constexpr real_player white = real_player::white;
constexpr real_player black = real_player::black;

static inline real_player opponent_of(real_player player)
{
  return (player == real_player::white) ? black : white;
}

enum class position_player
{
  to_move = 0,
  opponent = 1 
};

constexpr unsigned offset(position_player player)
{
  return static_cast<unsigned>(player);
}

constexpr position_player player_to_move = position_player::to_move; 
constexpr position_player player_opponent = position_player::opponent; 

constexpr square
make_square(piece piece, position_player player = player_to_move)
{
  return static_cast<square>(
           static_cast<int>(piece) | static_cast<int>(player));
}

constexpr position_player get_player(square square)
{
  return static_cast<position_player>(square & 1);
}

constexpr piece get_piece_type(square square)
{
  return static_cast<piece>(square & ~1);
}

constexpr position_player opponent_of(position_player player)
{
  return static_cast<position_player>(offset(player) ^ 1);
}

constexpr square opponent_of(square square)
{
  return static_cast<kator::square>(square ^ 1);
}

struct real_piece
{
public:
  piece type;
  real_player player;

  explicit real_piece(char);

  explicit constexpr
  real_piece(piece ctor_type, real_player player_to_move):
    type(ctor_type),
    player(player_to_move)
  {
  }

  explicit constexpr
  real_piece(square ctor_square, position_player whites_view):
    type(get_piece_type(ctor_square)),
    player((whites_view == get_player(ctor_square)) ? white : black)
  {
  }

  static bool is_valid_char(char);
  char to_char() const;
  std::string to_str() const;
  std::string to_figurine() const;
  std::string to_str(bool use_figurines) const;

  constexpr unsigned offset() const
  {
    return static_cast<unsigned>(type) + static_cast<unsigned>(player);
  }

}; /* class real_piece */

constexpr real_piece white_pawn = real_piece(piece::pawn, white);
constexpr real_piece black_pawn = real_piece(piece::pawn, black);
constexpr real_piece white_rook = real_piece(piece::rook, white);
constexpr real_piece black_rook = real_piece(piece::rook, black);
constexpr real_piece white_king = real_piece(piece::king, white);
constexpr real_piece black_king = real_piece(piece::king, black);
constexpr real_piece white_bishop = real_piece(piece::bishop, white);
constexpr real_piece black_bishop = real_piece(piece::bishop, black);
constexpr real_piece white_knight = real_piece(piece::knight, white);
constexpr real_piece black_knight = real_piece(piece::knight, black);
constexpr real_piece white_queen = real_piece(piece::queen, white);
constexpr real_piece black_queen = real_piece(piece::queen, black);

struct real_square
{
  bool is_empty;
  real_piece piece;

  real_square():
    is_empty(true),
    piece(white_pawn)
  {
  }
};

extern const char starting_fen[];

class invalid_fen: public std::exception
{
public:
  virtual const char* what() const noexcept;
};

class invalid_castle_rights: public std::exception
{
public:
  virtual const char* what() const noexcept;
};

class invalid_en_passant_index: public std::exception
{
public:
  virtual const char* what() const noexcept;
};

class invalid_king_positions: public std::exception
{
public:
  virtual const char* what() const noexcept;
};

class invalid_move_string
{
public:
  invalid_move_string(const char*);
  invalid_move_string(const std::string&);
  const char* what() const noexcept;
private:
  std::string move_error_string;
};

std::unique_ptr<game_state> parse_fen(std::string);
std::unique_ptr<game_state> parse_fen(std::istream&);

std::ostream& operator<< (std::ostream&, real_player);

void bitboard_lookup_tables_init();

void cleanup_move_string(std::string&);

std::ostream& operator<< (std::ostream&, sq_index);
std::ostream& operator<< (std::ostream&, real_piece);

} /* namespace kator */

#endif /* !defined(KATOR_CHESS_H) */

