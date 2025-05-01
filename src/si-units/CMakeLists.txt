build_lib(
  LIBNAME si-units
  SOURCE_FILES model/units-angle.cc model/units-attributes.cc
               model/units-energy.cc model/units-frequency.cc
  HEADER_FILES
    model/format-string.h
    model/si-units.h
    model/units-aliases.h
    model/units-angle.h
    model/units-attributes.h
    model/units-energy.h
    model/units-frequency.h
  LIBRARIES_TO_LINK ${libcore}
  TEST_SOURCES test/si-units-test-suite.cc
)
