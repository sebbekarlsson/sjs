
function (sjs_init_deps TARGET OUT_VALUE)
  set(GPP_ZIP_PATH ${CMAKE_CURRENT_BINARY_DIR}/gpp.zip)
  set(GPP_BIN_PATH ${CMAKE_CURRENT_BINARY_DIR}/gpp_e)
  add_custom_command(
    OUTPUT ${GPP_BIN_PATH}
    COMMAND wget https://github.com/sebbekarlsson/gpp/archive/refs/heads/master.zip -O ${GPP_ZIP_PATH} && unzip ${GPP_ZIP_PATH} && cd gpp-master && mkdir -p build && cd build && ${CMAKE_COMMAND} ../ && make && cp ./gpp_e ${CMAKE_CURRENT_BINARY_DIR}
    )


  if (NOT TARGET gpp_bin)
  add_custom_target(
    gpp_bin ALL
    COMMAND echo "gpp_bin"
    DEPENDS ${GPP_BIN_PATH}
    )
endif()


set(${OUT_VALUE} ${GPP_BIN_PATH})

add_dependencies(${TARGET} gpp_bin)
endfunction()
