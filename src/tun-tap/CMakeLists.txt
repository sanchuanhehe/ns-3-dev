set(source_files
    helper/creator-utils.cc
    helper/encode-decode.cc
)

set(header_files
    helper/creator-utils.h
    helper/encode-decode.h
)

build_lib(
  LIBNAME tun-tap
  SOURCE_FILES ${source_files}
  HEADER_FILES ${header_files}
)

build_exec(
  EXECNAME tap-creator
  SOURCE_FILES helper/tap-creator.cc
  LIBRARIES_TO_LINK ${libnetwork}
                    ${libtun-tap}
  EXECUTABLE_DIRECTORY_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/src/tun-tap/
  INSTALL_DIRECTORY_PATH ${CMAKE_INSTALL_LIBEXECDIR}/ns3
  STANDALONE
)
