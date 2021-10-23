#ifndef JS_ENGINE_MACROS_H
#define JS_ENGINE_MACROS_H
#include <js/js_mem.h>
#include <stdlib.h>

#define NEW(T) (T *)js_calloc(1, sizeof(T))

#endif
