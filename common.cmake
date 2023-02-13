##
# @file    common.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(RootDir     ${CMAKE_CURRENT_LIST_DIR})
set(CommonDir   ${RootDir}/common/)
set(FmtDir      ${RootDir}/fmt/)

set(CompileFlags
   -Werror
   -Wall
   -Wextra
   -Wno-format-security
   -O2)
