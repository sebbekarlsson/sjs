cmake_minimum_required(VERSION 3.20)


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


function (generate_files)
  FetchContent_Declare(
    gpp
    GIT_REPOSITORY https://github.com/sebbekarlsson/gpp.git
    )
  FetchContent_MakeAvailable(gpp)
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/context.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

  file(GLOB js_enums RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/src/enums ${CMAKE_CURRENT_SOURCE_DIR}/src/enums/*.gpp)
  foreach(gpp_file ${js_enums})
    message(${gpp_file})
    set(outfile ${CMAKE_CURRENT_BINARY_DIR}/enums/${gpp_file}.h)
    list(APPEND outfiles ${outfile})
    set(command_to_run mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/enums && gpp ${CMAKE_CURRENT_SOURCE_DIR}/src/enums/${gpp_file} > ${outfile})
    message(${command_to_run})
    add_custom_command(OUTPUT ${outfile}
      COMMAND ${command_to_run}
    )
endforeach()

file(GLOB js_builtins RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/src/builtins ${CMAKE_CURRENT_SOURCE_DIR}/src/builtins/*.gpp)
  foreach(gpp_file ${js_builtins})
    message(${gpp_file})
    set(outfile ${CMAKE_CURRENT_BINARY_DIR}/builtins/${gpp_file}.h)
    list(APPEND outfiles ${outfile})
    set(command_to_run mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/builtins && gpp ${CMAKE_CURRENT_SOURCE_DIR}/src/builtins/${gpp_file} > ${outfile})
    message(${command_to_run})
    add_custom_command(OUTPUT ${outfile}
      COMMAND ${command_to_run}
    )
endforeach()
add_custom_target(generate_all_files ALL DEPENDS ${outfiles})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/enums)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/builtins)
endfunction()


generate_files()

target_link_libraries(xjs PUBLIC hashmap_static)
target_link_libraries(xjs_e PUBLIC hashmap_static)
target_link_libraries(xjs_static PUBLIC hashmap_static)
