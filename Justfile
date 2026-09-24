##
# @file    Justfile
# @version 1.0.0
# @author  Forrest Jablonski
#

default_build_dir := "build"
default_target := "all"

build tar=default_target:
   @if [ ! -f "{{default_build_dir}}/CMakeCache.txt" ]; then \
      cmake -S . --preset {{tar}}; \
   fi
   cmake --build {{default_build_dir}} --target {{tar}}

open tar=default_target:
   @open {{default_build_dir}}/docs/html/index.html

clean dir=default_build_dir:
   @rm -rf "{{dir}}"
   @mkdir -p "{{dir}}"
