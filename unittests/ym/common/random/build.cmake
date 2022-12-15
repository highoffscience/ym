##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

set(SUTDir         ${RootDir}/ym/common/)
set(SUTUnitTestDir ${UnitTestDir}/ym/common/random/)

set(TargetName random)

# add_library(random SHARED)

# target_sources(random PRIVATE
#    ${SUTDir}/random.cpp
#    ${UnitTestDir}/common/testcase.cpp
#    ${UnitTestDir}/common/datashuttle.cpp
#    ${UnitTestDir}/common/testsuitebase.cpp
#    ${UnitTestDir}/common/nameable.cpp
#    ${SUTUnitTestDir}/testsuite.cpp)

# # TODO investigate FetchContent command
# #target_link_libraries(random ${FMTDir}/build/libfmt.a)

# set_target_properties(random PROPERTIES VERSION ${PROJECT_VERSION})
# set_target_properties(random PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/ym/common)

# target_compile_options(random PRIVATE -Werror -Wall -Wextra -O2 -DFMT_HEADER_ONLY)

# target_include_directories(random PRIVATE ${FMTDir}/include/)
# target_include_directories(random PRIVATE ${CommonDir})
# target_include_directories(random PRIVATE ${UnitTestDir}/common/)
# target_include_directories(random PRIVATE ${SUTDir})
# target_include_directories(random PRIVATE ${SUTUnitTestDir})
