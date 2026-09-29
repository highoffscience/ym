##
# @file    check_venv.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

if (NOT DEFINED ENV{VIRTUAL_ENV})
   message(FATAL_ERROR "Python virtual environment required to run this target")
endif()
