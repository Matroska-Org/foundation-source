project("mkparts" VERSION 0.1.0)

add_executable("mkparts" mkparts.c)

configure_file(mkparts_project.h.in mkparts_project.h)
target_include_directories("mkparts" PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries("mkparts" PUBLIC "matroska2" "ebml2" "corec")

# Source packaging script
configure_file(pkg.sh.in pkg.sh)
configure_file(src.br.in src.br)
configure_file(CMakeLists.txt.in CMakeLists.txt)
