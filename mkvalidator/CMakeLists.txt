project("mkvalidator" VERSION 0.5.2)

add_executable("mkvalidator" mkvalidator.c)

configure_file(mkvalidator_project.h.in mkvalidator_project.h)
target_include_directories("mkvalidator" PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries("mkvalidator" PRIVATE "matroska2" "corec_parser")

# Source packaging script
configure_file(pkg.sh.in pkg.sh)
configure_file(src.br.in src.br)
configure_file(CMakeLists.txt.in CMakeLists.txt)
