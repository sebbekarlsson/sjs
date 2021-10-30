cmake_minimum_required(VERSION 3.20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${sjs_SOURCE_DIR}/cmake/modules")
include(build_custom)

project(sjs_test)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/sourcefiles DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

file(GLOB test_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_executable(test ${test_sources})


add_dependencies(test sjs_static)


target_link_libraries(test PRIVATE sjs_static)

generate_files()
