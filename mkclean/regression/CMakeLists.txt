project("mkcleanreg" VERSION 0.1.0)

add_executable("mkcleanreg" mkcleanreg.c)

configure_file(mkcleanreg_project.h.in mkcleanreg_project.h)
target_include_directories("mkcleanreg" PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries("mkcleanreg" PRIVATE "corec")
