cmake_minimum_required(VERSION 3.1.2)

project("foundation sources")

option(CONFIG_MATROSKA2 "Build libmatroska2 and tools" ON)

add_subdirectory("corec")
add_subdirectory("libebml2")
if(CONFIG_MATROSKA2)
    add_subdirectory("mkvalidator")
    add_subdirectory("mkclean")
    add_subdirectory("mkparts")
    add_subdirectory("libmatroska2")
endif(CONFIG_MATROSKA2)
