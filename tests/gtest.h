
#ifdef __STRICT_ANSI__
/* Treatment for "'fileno' was not declared in this scope" error in cygwin */
#  undef __STRICT_ANSI__
#  include "gtest/gtest.h"
#  define __STRICT_ANSI__
#else
#  include "gtest/gtest.h"
#endif
