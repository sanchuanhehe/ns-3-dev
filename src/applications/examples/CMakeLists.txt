set(base_examples
    three-gpp-http-example
    traffic-generator-example
)
foreach(
  example
  ${base_examples}
)
  build_lib_example(
    NAME ${example}
    SOURCE_FILES ${example}.cc
    LIBRARIES_TO_LINK
      ${libapplications}
      ${libpoint-to-point}
      ${libinternet}
      ${libnetwork}
  )
endforeach()
