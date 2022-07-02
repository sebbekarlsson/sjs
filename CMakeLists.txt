cmake_minimum_required(VERSION 3.20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
include(build_custom)
include(sjs_deps)

project(sjs)


#add_subdirectory(tests)

file(GLOB PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB sjs_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.c ${CMAKE_CURRENT_SOURCE_DIR}/src/**/**/*.c)


add_compile_options(-g -Wall)

add_executable(sjs ${sjs_sources})


list(FILTER sjs_sources EXCLUDE REGEX "main.c")

add_library(sjs_shared SHARED ${sjs_sources})
add_library(sjs_static STATIC ${sjs_sources})

target_compile_options(sjs_shared PUBLIC -fPIC)
target_compile_options(sjs_static PUBLIC -fPIC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(
  sjs_shared
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_include_directories(
  sjs_static
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_include_directories(
  sjs
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)


include(FetchContent)
FetchContent_Declare(
  hashmap_static
  GIT_REPOSITORY https://github.com/sebbekarlsson/hashmap.git
  GIT_TAG master
)
FetchContent_MakeAvailable(hashmap_static)

    FetchContent_Declare(
    hdr
    GIT_REPOSITORY https://github.com/sebbekarlsson/hdr.git
    )
  FetchContent_MakeAvailable(hdr)



sjs_init_deps(sjs GPP_BIN_PATH)
sjs_init_deps(sjs_shared GPP_BIN_PATH)
sjs_init_deps(sjs_static GPP_BIN_PATH)


generate_files()



target_link_libraries(sjs PUBLIC hashmap_static m)
target_link_libraries(sjs_shared PUBLIC hashmap_static m)
target_link_libraries(sjs_static PUBLIC hashmap_static m)
