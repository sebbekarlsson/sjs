cmake_minimum_required(VERSION 3.20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
include(build_custom)

project(xjs)

file(GLOB PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB xjs_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.c ${CMAKE_CURRENT_SOURCE_DIR}/src/**/**/*.c)


add_compile_options(-g -Wall)

add_executable(xjs_e ${xjs_sources})


list(FILTER xjs_ources EXCLUDE REGEX "main.c")

add_library(xjs SHARED ${xjs_sources})
add_library(xjs_static STATIC ${xjs_sources})

target_compile_options(xjs PUBLIC -fPIC)
target_compile_options(xjs_static PUBLIC -fPIC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(
  xjs
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_include_directories(
  xjs_static
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_include_directories(
  xjs_e
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)


include(FetchContent)
FetchContent_Declare(
  hashmap_static
  URL /home/ianertson/workspace/hashmap
  # GIT_REPOSITORY https://github.com/sebbekarlsson/hashmap.git
  # GIT_TAG master
)
FetchContent_MakeAvailable(hashmap_static)


generate_files()

target_link_libraries(xjs PUBLIC hashmap_static)
target_link_libraries(xjs_e PUBLIC hashmap_static)
target_link_libraries(xjs_static PUBLIC hashmap_static)
