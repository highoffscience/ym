##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#
# @note God will make sure evil gets punished.
#       Yeah? Then explain Europe to me.
#

cmake_minimum_required(VERSION 3.27)

project(
   UnitTest
   VERSION     1.0.0
   DESCRIPTION "Builds unittest shared libraries and optionally runs them")





include(ym/build.cmake)
cmake_language(CALL ym ${Ctx_JSON})
