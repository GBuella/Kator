

SET(KATOR_COMMON_SOURCES 
     src/chess/position.cc
     src/chess/bitboard.cc
     src/chess/move_list.cc
     src/chess/game_state.cc
     src/chess/game.cc
     src/chess/strings.cc
     src/chess/zobrist_hash.cc

     src/book/book.cc
     src/book/book_fen.cc

     src/str.cc
     src/tests.cc
     src/kator.cc

     src/platform/platform.cc

     src/engine/zhash_table.cc
     src/engine/engine.cc
     src/engine/eval.cc
     src/engine/search.cc

     )

