##
# @file    targets.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

##
# @brief Links target to fmt library.
#
macro(link-to-fmt Target)

   find_library(YM_FMT_LIB
      NAMES fmt
      PATHS ${YM_ExtLibsDir}/build/fmt)
   if (YM_FMT_LIB)
      target_link_libraries(${Target} INTERFACE ${YM_FMT_LIB})
      target_include_directories(${Target} INTERFACE ${YM_ExtLibsDir}/fmt/include)
   else()
      message(FATAL_ERROR "fmt lib not found. Must build external libraries first.")
   endif()

endmacro()
