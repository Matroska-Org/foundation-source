cmake_minimum_required(VERSION 3.14)

project("foundation sources")

option(CONFIG_MATROSKA2 "Build libmatroska2 and tools" ON)
option(DEV_MODE "Developer mode with extra compilation checks" OFF)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.11)

include(CheckCCompilerFlag)
include(CMakeParseArguments)
function(add_c_flag_if_supported)
  foreach(flagname ${ARGN})
    string(REPLACE "-" "_" TEST_RESULT ${flagname})
    string(REPLACE "=" "_" TEST_RESULT ${TEST_RESULT})
    check_c_compiler_flag(${flagname} HAVE_FLAG_${TEST_RESULT})
    # message(STATUS "supported ${flagname}: ${HAVE_FLAG_${TEST_RESULT}}")

    if (HAVE_FLAG_${TEST_RESULT})
      add_compile_options(${flagname})
    endif()
  endforeach()
endfunction()

if(DEV_MODE)
  message(STATUS "Using developer mode for ${CMAKE_C_COMPILER_ID}")
  set(CMAKE_C_EXTENSIONS OFF)
  add_c_flag_if_supported(-Wno-error=unused-command-line-argument
                          -Wall -Wextra -Wpedantic -Wfatal-errors -fstack-protector-strong
                          -Wno-self-assign
                          -Wno-unused-parameter
                          -Werror=missing-field-initializers
                          -Werror=excess-initializers
                          -Werror=strict-aliasing
                          -Werror=array-bounds
                          -Werror=format
                          -Werror=incompatible-pointer-types
                          -Werror=restrict
                          -Werror=int-conversion
                          -Werror=implicit-int
                          -Werror=return-mismatch
                          -Werror=declaration-missing-parameter-type
                          -Werror=switch
                          -Werror=return-type
                          # MSVC
                          -W4
                          -we4013 # undefined; assuming extern returning int
                          -we4005 # macro redefinition
                          )
endif()

add_subdirectory("corec")
add_subdirectory("libebml2")
if(CONFIG_MATROSKA2)
    add_subdirectory("libmatroska2")
    add_subdirectory("mkvalidator")
    add_subdirectory("mkclean")
    add_subdirectory("mkparts")
endif(CONFIG_MATROSKA2)
