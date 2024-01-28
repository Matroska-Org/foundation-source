project("mkvalidator2" VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# force output of the program in a specific folder with ReadMe/Changelog
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_BINARY_DIR}/pkg_bin")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/pkg_bin")

find_package(Matroska 2.0.0 REQUIRED)

add_executable("mkvalidator2" mkvalidator.cpp)

configure_file(mkvalidator2_project.h.in mkvalidator2_project.h)
target_include_directories("mkvalidator2" PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries("mkvalidator2" PUBLIC EBML::ebml)
target_link_libraries("mkvalidator2" PUBLIC Matroska::matroska)
# target_link_libraries("mkvalidator2" PUBLIC "matroska2" "ebml2" "corec")

# Source packaging script
configure_file(pkg.sh.in pkg.sh)
configure_file(src.br.in src.br)
configure_file(CMakeLists.txt.in CMakeLists.txt)
configure_file(ReadMe.txt ${CMAKE_CURRENT_BINARY_DIR}/pkg_bin/ReadMe.txt)
configure_file(ChangeLog.txt ${CMAKE_CURRENT_BINARY_DIR}/pkg_bin/ChangeLog.txt)

include(GNUInstallDirs)

install(TARGETS mkvalidator RUNTIME)
