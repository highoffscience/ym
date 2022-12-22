##
# @file    utcommon.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

include(../common.cmake)

set(UnitTestDir ${RootDir}/unittests/)

set(UTCommonSrcs
   ${UnitTestDir}/common/datashuttle.cpp
   ${UnitTestDir}/common/nameable.cpp
   ${UnitTestDir}/common/testcase.cpp
   ${UnitTestDir}/common/testsuitebase.cpp
   ${UnitTestDir}/common/utception.cpp)

set( CompileFlags
   ${CompileFlags}
   -DFMT_HEADER_ONLY)
