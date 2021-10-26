#ifndef JS_ENGINE_FRAME_H
#define JS_ENGINE_FRAME_H
#include <hashmap/map.h>
map_T *setup_js_frame();
void js_frame_free(map_T *frame);
#endif
