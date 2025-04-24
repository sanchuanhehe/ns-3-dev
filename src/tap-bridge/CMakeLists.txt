if(NOT
   ${ENABLE_EMU}
)
  message(
    ${HIGHLIGHTED_STATUS}
    "tap-bridge requires emulation feature"
  )
  message(
    ${HIGHLIGHTED_STATUS}
    "Skipping src/tap-bridge"
  )
  list(
    REMOVE_ITEM
    libs_to_build
    tap-bridge
    PARENT_SCOPE
  )
  return()
endif()

add_definitions(
  -DTAP_CREATOR="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/src/tun-tap/ns${NS3_VER}-tap-creator${build_profile_suffix}"
)

build_lib(
  LIBNAME tap-bridge
  SOURCE_FILES helper/tap-bridge-helper.cc
               model/tap-bridge.cc
  HEADER_FILES helper/tap-bridge-helper.h
               model/tap-bridge.h
  LIBRARIES_TO_LINK ${libinternet}
                    ${libtun-tap}
)

add_dependencies(
  tap-bridge
  tap-creator
)
