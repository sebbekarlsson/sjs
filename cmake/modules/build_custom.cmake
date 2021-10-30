include(FetchContent)


function(build_custom TARGETZ NAME CMD DEPENDS OUT)
  separate_arguments(EX UNIX_COMMAND PROGRAM SEPARATE_ARGS ${CMD})
  message(${CMD})
  foreach(target ${TARGETZ})
    add_custom_command(OUTPUT ${OUT}
        COMMAND ${EX}
      )
    add_custom_target(MAKE_${NAME}_${target} ALL DEPENDS ${OUT})
    add_dependencies(${target} MAKE_${NAME}_${target})
  endforeach()
endfunction()


function (generate_files)
  FetchContent_Declare(
    gpp
    GIT_REPOSITORY https://github.com/sebbekarlsson/gpp.git
    )
  FetchContent_MakeAvailable(gpp)
  file(COPY ${sjs_SOURCE_DIR}/context.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)


  file(GLOB js_builtins ${CMAKE_CURRENT_SOURCE_DIR}/src/builtins/*.gpp)
  file(GLOB js_enums ${CMAKE_CURRENT_SOURCE_DIR}/src/enums/*.gpp)
  list(APPEND custom_files ${js_builtins} ${js_enums})

  foreach (gpp_file ${custom_files})
    get_filename_component(barename ${gpp_file} NAME)
    set(outfile ${CMAKE_CURRENT_BINARY_DIR}/generated/${barename}.h)
    build_custom(
      "sjs;sjs_static;sjs_shared"
      ${barename}
      "mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/generated && gpp ${gpp_file} > ${outfile}"
      ${CMAKE_CURRENT_SOURCE_DIR}/src/builtins/${gpp_file}
      ${outfile}
    )
  endforeach()
  include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated)
  include_directories(${sjs_BINARY_DIR}/generated)
endfunction()
