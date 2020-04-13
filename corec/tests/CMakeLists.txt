add_executable("string_test" string_test.c)
target_link_libraries("string_test" PUBLIC "corec_str" "corec_charconvert")

add_executable("file_test" file_test.c)
target_link_libraries("file_test" PUBLIC "corec_file")

add_executable("node_test" node_test.c)
target_link_libraries("node_test" PUBLIC "corec_node")

# add_executable("parser_test" parser_test.c)
# target_link_libraries("parser_test" PUBLIC "corec_parser")
