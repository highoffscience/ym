##
# @file    common.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(RootDir     ${CMAKE_CURRENT_LIST_DIR})
set(CommonDir   ${RootDir}/common/)
set(UnitTestDir ${RootDir}/unittests/)
set(FmtDir      ${RootDir}/fmt/)

set(UTCommonSrcs
   ${UnitTestDir}/common/datashuttle.cpp
   ${UnitTestDir}/common/nameable.cpp
   ${UnitTestDir}/common/testcase.cpp
   ${UnitTestDir}/common/testsuitebase.cpp)

set(CompileFlags
   -Werror
   -Wall
   -Wextra
   -O2
   -DFMT_HEADER_ONLY)
