#include <js/builtins/console/console.h>
#include <js/js_frame.h>
#include <stdint.h>
#include <stdio.h>

map_T *setup_js_frame() {
  map_T *frame = NEW_MAP();

  JSAST *console = init_js_builtin_console();
  map_set(frame, "console", console);

  return frame;
}
