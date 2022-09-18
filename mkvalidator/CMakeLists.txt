project("mkvalidator" VERSION 0.6.0 LANGUAGES C)

# force output of the program in a specific folder with ReadMe/Changelog
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_BINARY_DIR}/pkg_bin")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/pkg_bin")

add_executable("mkvalidator" mkvalidator.c)
set_target_properties("mkvalidator" PROPERTIES 
  C_STANDARD 11
)

configure_file(mkvalidator_project.h.in mkvalidator_project.h)
target_include_directories("mkvalidator" PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries("mkvalidator" PUBLIC "matroska2" "ebml2" "corec")

# Source packaging script
configure_file(pkg.sh.in pkg.sh)
configure_file(src.br.in src.br)
configure_file(CMakeLists.txt.in CMakeLists.txt)
configure_file(ReadMe.txt ${CMAKE_CURRENT_BINARY_DIR}/pkg_bin/ReadMe.txt)
configure_file(ChangeLog.txt ${CMAKE_CURRENT_BINARY_DIR}/pkg_bin/ChangeLog.txt)

include(GNUInstallDirs)

install(TARGETS mkvalidator RUNTIME)
