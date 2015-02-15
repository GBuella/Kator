
include(CTest)

enable_testing()

add_test("FEN" kator --test_fen)
add_test("move" kator --test_move)
add_test("print_move" kator --test_print_move)
add_test("parse_move" kator --test_parse_move)
add_test("move_counters" kator --test_move_counters)

add_test("move_generator_perftsuite_starting_position"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_000 )
add_test("move_generator_perftsuite_001"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_001 )
add_test("move_generator_perftsuite_002"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_002 )
add_test("move_generator_perftsuite_003"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_003 )
add_test("move_generator_perftsuite_004"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_004 )
add_test("move_generator_perftsuite_005"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_005 )
add_test("move_generator_perftsuite_006"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_006 )
add_test("move_generator_perftsuite_007"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_007 )
add_test("move_generator_perftsuite_008"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_008 )
add_test("move_generator_perftsuite_009"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_009 )
add_test("move_generator_perftsuite_010"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_010 )
add_test("move_generator_perftsuite_011"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_011 )
add_test("move_generator_perftsuite_012"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_012 )
add_test("move_generator_perftsuite_013"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_013 )
add_test("move_generator_perftsuite_014"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_014 )
add_test("move_generator_perftsuite_015"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_015 )
add_test("move_generator_perftsuite_016"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_016 )
add_test("move_generator_perftsuite_017"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_017 )
add_test("move_generator_perftsuite_018"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_018 )
add_test("move_generator_perftsuite_019"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_019 )
add_test("move_generator_perftsuite_020"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_020 )
add_test("move_generator_perftsuite_021"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_021 )
add_test("move_generator_perftsuite_022"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_022 )
add_test("move_generator_perftsuite_023"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_023 )
add_test("move_generator_perftsuite_024"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_024 )
add_test("move_generator_perftsuite_025"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_025 )
add_test("move_generator_perftsuite_026"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_026 )
add_test("move_generator_perftsuite_027"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_027 )
add_test("move_generator_perftsuite_028"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_028 )
add_test("move_generator_perftsuite_029"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_029 )
add_test("move_generator_perftsuite_030"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_030 )
add_test("move_generator_perftsuite_031"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_031 )
add_test("move_generator_perftsuite_032"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_032 )
add_test("move_generator_perftsuite_033"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_033 )
add_test("move_generator_perftsuite_034"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_034 )
add_test("move_generator_perftsuite_035"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_035 )
add_test("move_generator_perftsuite_036"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_036 )
add_test("move_generator_perftsuite_037"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_037 )
add_test("move_generator_perftsuite_038"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_038 )
add_test("move_generator_perftsuite_039"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_039 )
add_test("move_generator_perftsuite_040"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_040 )
add_test("move_generator_perftsuite_041"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_041 )
add_test("move_generator_perftsuite_042"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_042 )
add_test("move_generator_perftsuite_043"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_043 )
add_test("move_generator_perftsuite_044"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_044 )
add_test("move_generator_perftsuite_045"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_045 )
add_test("move_generator_perftsuite_046"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_046 )
add_test("move_generator_perftsuite_047"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_047 )
add_test("move_generator_perftsuite_048"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_048 )
add_test("move_generator_perftsuite_049"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_049 )
add_test("move_generator_perftsuite_050"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_050 )
add_test("move_generator_perftsuite_051"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_051 )
add_test("move_generator_perftsuite_052"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_052 )
add_test("move_generator_perftsuite_053"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_053 )
add_test("move_generator_perftsuite_054"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_054 )
add_test("move_generator_perftsuite_055"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_055 )
add_test("move_generator_perftsuite_056"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_056 )
add_test("move_generator_perftsuite_057"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_057 )
add_test("move_generator_perftsuite_058"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_058 )
add_test("move_generator_perftsuite_059"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_059 )
add_test("move_generator_perftsuite_060"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_060 )
add_test("move_generator_perftsuite_061"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_061 )
add_test("move_generator_perftsuite_062"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_062 )
add_test("move_generator_perftsuite_063"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_063 )
add_test("move_generator_perftsuite_064"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_064 )
add_test("move_generator_perftsuite_065"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_065 )
add_test("move_generator_perftsuite_066"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_066 )
add_test("move_generator_perftsuite_067"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_067 )
add_test("move_generator_perftsuite_068"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_068 )
add_test("move_generator_perftsuite_069"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_069 )
add_test("move_generator_perftsuite_070"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_070 )
add_test("move_generator_perftsuite_071"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_071 )
add_test("move_generator_perftsuite_072"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_072 )
add_test("move_generator_perftsuite_073"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_073 )
add_test("move_generator_perftsuite_074"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_074 )
add_test("move_generator_perftsuite_075"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_075 )
add_test("move_generator_perftsuite_076"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_076 )
add_test("move_generator_perftsuite_077"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_077 )
add_test("move_generator_perftsuite_078"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_078 )
add_test("move_generator_perftsuite_079"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_079 )
add_test("move_generator_perftsuite_080"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_080 )
add_test("move_generator_perftsuite_081"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_081 )
add_test("move_generator_perftsuite_082"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_082 )
add_test("move_generator_perftsuite_083"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_083 )
add_test("move_generator_perftsuite_084"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_084 )
add_test("move_generator_perftsuite_085"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_085 )
add_test("move_generator_perftsuite_086"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_086 )
add_test("move_generator_perftsuite_087"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_087 )
add_test("move_generator_perftsuite_088"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_088 )
add_test("move_generator_perftsuite_089"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_089 )
add_test("move_generator_perftsuite_090"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_090 )
add_test("move_generator_perftsuite_091"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_091 )
add_test("move_generator_perftsuite_092"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_092 )
add_test("move_generator_perftsuite_093"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_093 )
add_test("move_generator_perftsuite_094"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_094 )
add_test("move_generator_perftsuite_095"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_095 )
add_test("move_generator_perftsuite_096"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_096 )
add_test("move_generator_perftsuite_097"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_097 )
add_test("move_generator_perftsuite_098"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_098 )
add_test("move_generator_perftsuite_099"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_099 )
add_test("move_generator_perftsuite_100"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_100 )
add_test("move_generator_perftsuite_101"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_101 )
add_test("move_generator_perftsuite_102"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_102 )
add_test("move_generator_perftsuite_103"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_103 )
add_test("move_generator_perftsuite_104"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_104 )
add_test("move_generator_perftsuite_105"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_105 )
add_test("move_generator_perftsuite_106"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_106 )
add_test("move_generator_perftsuite_107"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_107 )
add_test("move_generator_perftsuite_108"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_108 )
add_test("move_generator_perftsuite_109"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_109 )
add_test("move_generator_perftsuite_110"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_110 )
add_test("move_generator_perftsuite_111"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_111 )
add_test("move_generator_perftsuite_112"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_112 )
add_test("move_generator_perftsuite_113"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_113 )
add_test("move_generator_perftsuite_114"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_114 )
add_test("move_generator_perftsuite_115"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_115 )
add_test("move_generator_perftsuite_116"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_116 )
add_test("move_generator_perftsuite_117"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_117 )
add_test("move_generator_perftsuite_118"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_118 )
add_test("move_generator_perftsuite_119"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_119 )
add_test("move_generator_perftsuite_120"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_120 )
add_test("move_generator_perftsuite_121"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_121 )
add_test("move_generator_perftsuite_122"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_122 )
add_test("move_generator_perftsuite_123"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_123 )
add_test("move_generator_perftsuite_124"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_124 )
add_test("move_generator_perftsuite_125"
         kator --test_file ${CMAKE_SOURCE_DIR}/tests/perftsuite/perft_125 )

