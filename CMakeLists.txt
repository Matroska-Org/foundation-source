cmake_minimum_required(VERSION 3.14)

project("foundation sources")

option(CONFIG_MATROSKA2 "Build libmatroska2 and tools" ON)

add_subdirectory("corec")
add_subdirectory("libebml2")
if(CONFIG_MATROSKA2)
    add_subdirectory("libmatroska2")
    add_subdirectory("mkvalidator")
    add_subdirectory("mkvalidator2")
    add_subdirectory("mkclean")
    add_subdirectory("mkclean2")
    add_subdirectory("mkparts")
endif(CONFIG_MATROSKA2)
