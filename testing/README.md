Install instructions for unittest directory
(cppyy doesn't work with python3.11, continue with 3.10)

sudo apt install python3.13
sudo apt install python3.13-venv
sudo apt install python3.13-dev

sudo apt install libstdc++-15-dev // for std::stacktrace

can experiment with "$jupyter lab <name-of-file>" after activating venv

$ python -m venv venv/
$ . ./venv/bin/activate
$ python -m pip install -r requirements.txt
$ mkdir build/ | cov_build/
$ cd build/ | cov_build/
$ cmake .. [-DCOV_ENABLED=True|False]
$ cmake --build . [--target <suite-name>[-run]]

eg...
$ cmake --build . --target ym.common.ymdefs[-run]

To clean, eg...
$ cmake --build . --target clean

Produced libraries are placed in customlibs/
The "_run" option will build the testsuite(s) and then run it
If running for coverage it will optionally produce coverage files in profiles/

To add a new unittest suite:
Use setup_ut_scaffold.py
Edit generated files as appropriate
Edit CMakeLists.txt

If the suite requires extra care to test you can add a custom build.cmake to the test suite directory

// -----------------------------------------------------------------------------

Unittests require to be run in the venv
$ . ./venv/bin/activate

To run a unittest manually, eg...
$ python -m unittest ym.common.ymdefs.testsuite

To run a specific test, eg...
$ python -m unittest ym.common.ymdefs.testsuite.TestSuite.test_InteractiveInspection

Test suites can also be run in the directory, eg...
$ python testsuite.py

// -----------------------------------------------------------------------------

<https://clang.llvm.org/docs/SourceBasedCodeCoverage.html>
<https://llvm.org/docs/CommandGuide/llvm-cov.html>

To prepare testing for coverage do the following:
$ cd cov_build/
$ cmake .. -DCOV_ENABLED=True
$ cmake --build . --target clean

The scripts use llvm-cov show, but could also use llvm-cov report...
$ llvm-cov report <desired-obj-file> -instr-profile=<testsuite>.profdata

// -----------------------------------------------------------------------------

                  a/                         a/
                 .  .                       .  .
               .      .                   .      .
              b/        c/               b/        c/
             .         .  .             .         .  .
          b0..bn     .      .        b0..bn     .      .
                  c0..cn     d/              c0..cn    d/
                             .                         .
                             e/                        e/
                             .                         .
                          e0..en                    e0..en

Example source directory structure is above. a/ - e/ are directories, a0..an are files.
There are 3 types of directories build-wise:
   1) File-Level directories
   2) Library    directories
   3) Container  directories

Type 1:
   Each source file will have it's own directory for unittesting. These directories
      are called File-Level directories. They may contain their own build.cmake
      script if building the unittest is non-trivial.

Type 2:
   Every source file will have a directory it lives in. At the unittest level every
      source directory that contains a source file will be a library directory. This
      means all direct source files (not nested) will be packaged into a shared object.
      This increases modularity and build times. Each unittest file in the directory
      will link to this shared object.
   These directories contain a build.cmake that build the shared object as well as
      defines the unittest targets for each of the source files, assuming that source
      file's File-Level unittest directory doesn't contain it's own build.cmake.

Type 3:
   These directories don't contain any files, just other directories. Nested shared
      objects will link to the parent shared object.

There are several types of libraries.
   1) Framework               Libraries (eg ut.common)
   2) Interface-For-Shared    Libraries (eg ym-interface)
   3) Interface-For-Testsuite Libraries
   4) Shared                  Libraries (eg ym.common)

Type 1:
   These libraries are for unittest framework operation. They are not instrumented and
   are not part of any testsuite, they only provide operations the testsuite needs to
   do it's job.

Type 2:
   These are pure abstract libraries meant to be linked into Shared (type 4) libraries.

Type 3:
   These are pure abstract libraries meant to be linked into testsuite executables.

Type 4:
   These libraries only contain the source files under test. Each directory/module will
   package all the source files within it into one shared library.

TODO code in testing/ym/common/

function(intgbuild-ym.common Ctx_JSON)

   set(BaseBuild ym.common)
   set(TargetAll ${BaseBuild}-unittests)
   set(TargetRun ${BaseBuild}-run)
   set(TargetInt ${BaseBuild}-interface)

   string(REPLACE "." "/" BaseBuildDir ${BaseBuild})

   add_custom_target(${TargetAll})
   add_custom_target(${TargetRun})
   add_library(${TargetInt} INTERFACE)

   target_link_libraries(${TargetInt} INTERFACE ym-interface)

   include(${YM_ProjRootDir}/${BaseBuildDir}/build.cmake)
   cmake_language(CALL srcbuild-${BaseBuild} ${Ctx_JSON})
   target_link_libraries(${TargetInt} INTERFACE ${BaseBuild})
   target_link_libraries(${BaseBuild} PRIVATE ${TargetInt})
   set_target_properties(${BaseBuild} PROPERTIES VERSION ${PROJECT_VERSION})
   set_target_properties(${BaseBuild} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${YM_CustomLibsDir})

   set(SubBuilds argparser datalogger fileio logger memio rng textlogger timer verbogroup ymassert ymdefs ymutils)
   foreach(SubBuild ${SubBuilds})

      set(SubBaseBuild ${BaseBuild}.${SubBuild})
      set(SubTarget    ${BaseBuild}.${SubBuild}-unittests)
      set(SubTargetRun ${BaseBuild}.${SubBuild}-run)

      set(SubBuildDir ${YM_UnitTestDir}/${BaseBuildDir}/${SubBuild})

      if(EXISTS  ${SubBuildDir}/build.cmake)
         include(${SubBuildDir}/build.cmake)
         cmake_language(CALL unitbuild-${SubBaseBuild} Ctx_JSON)
      else()
         add_library(${SubTarget} SHARED)
         target_sources(${SubTarget} PRIVATE
            ${SubBuildDir}/testsuite.cpp)
         target_link_libraries(${SubTarget} PRIVATE ${TargetInt})
         set_target_properties(${SubTarget} PROPERTIES VERSION ${PROJECT_VERSION})
         set_target_properties(${SubTarget} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${YM_CustomLibsDir})
      endif()

      add_custom_target(${SubTargetRun} DEPENDS ${SubTarget})
      add_dependencies(${SubTargetRun} check-venv)
      add_dependencies(${TargetAll} ${SubTarget})
      add_dependencies(${TargetRun} ${SubTargetRun})

      add_custom_command(TARGET ${SubTargetRun}
         POST_BUILD
         WORKING_DIRECTORY ${YM_UnitTestDir}
         COMMAND ${CMAKE_COMMAND} -E env
            PYTHONPATH=$ENV{PYTHONPATH}
            ${YM_Python} run_unittest.py
               --unittestdir=${YM_UnitTestDir}
               --projrootdir=${YM_ProjRootDir}
               --builddir=${CMAKE_BINARY_DIR}
               --suitename=${SubBaseBuild}
               --libraryname=lib${BaseBuild}.so
               --covenabled=${YM_CovEnabled})

   endforeach()

   if (YM_CovEnabled)
      add_custom_command(TARGET ${TargetRun}
         POST_BUILD
         WORKING_DIRECTORY ${YM_UnitTestDir}
         COMMAND ${YM_Python} merge_cov_profiles.py --binarydir=${CMAKE_BINARY_DIR} --libraryname=lib${BaseBuild}.so)
   endif()

endfunction()
