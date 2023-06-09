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
