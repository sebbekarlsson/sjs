#ifndef JS_ENGINE_MACROS_H
#define JS_ENGINE_MACROS_H
#include <stdlib.h>
#include <js/js_mem.h>

#define NEW(T)                                  \
  (T*)js_calloc(1, sizeof(T))

#endif
