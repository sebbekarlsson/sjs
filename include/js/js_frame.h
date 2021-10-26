#ifndef JS_ENGINE_FRAME_H
#define JS_ENGINE_FRAME_H
#include <hashmap/map.h>

struct JS_EXECUTION_STRUCT;
map_T *setup_js_frame(struct JS_EXECUTION_STRUCT *execution);
void js_frame_free(map_T *frame);
#endif
