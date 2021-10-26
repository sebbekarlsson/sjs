#ifndef JS_ENGINE_IO_H
#define JS_ENGINE_IO_H
char *get_file_contents(const char *filepath);
char *get_working_directory();
unsigned int js_file_exists(const char *filepath);
unsigned int js_is_directory(const char *filepath);
unsigned int js_is_regular_file(const char *filepath);
#endif
