project("mkclean" VERSION 0.9.0 LANGUAGES C)

add_subdirectory("regression")

# force output of the program in a specific folder with ReadMe/Changelog
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_BINARY_DIR}/pkg_bin")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/pkg_bin")

add_executable("mkclean" mkclean.c)
set_target_properties("mkclean" PROPERTIES 
  C_STANDARD 11
)
add_executable("mkWDclean" mkclean.c)
set_target_properties("mkWDclean" PROPERTIES 
  C_STANDARD 11
)

configure_file(mkclean_project.h.in mkclean_project.h)
target_include_directories("mkclean" PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories("mkWDclean" PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries("mkclean" PUBLIC "matroska2" "ebml2" "corec")
target_link_libraries("mkWDclean" PUBLIC "matroska2" "ebml2" "corec")

# Source packaging script
configure_file(pkg.sh.in pkg.sh)
configure_file(src.br.in src.br)
configure_file(CMakeLists.txt.in CMakeLists.txt)
configure_file(ReadMe.txt ${CMAKE_CURRENT_BINARY_DIR}/pkg_bin/ReadMe.txt)
configure_file(ChangeLog.txt ${CMAKE_CURRENT_BINARY_DIR}/pkg_bin/ChangeLog.txt)

include(GNUInstallDirs)

install(TARGETS mkclean RUNTIME)
install(TARGETS mkWDclean RUNTIME)
