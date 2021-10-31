#ifndef JS_ENGINE_BUILTINS_MACROS_H
#define JS_ENGINE_BUILTINS_MACROS_H
#include <assert.h>

#define EXPECT_MIN_ARGS(N)                                                     \
  {                                                                            \
    assert(args != 0);                                                         \
    if (args->size < N) {\
        printf("ERROR (%s): Expected a minimum of %d arguments.\n", __func__, N);\
    }\
  }

#define GET_ARG(N)                              \
  (JSAST*)args->size >= N ? args->items[N] : 0

#endif
