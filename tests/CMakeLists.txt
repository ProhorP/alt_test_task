
add_executable(test_download_json test_download_json.c)
target_link_libraries(test_download_json diff_branch_pack)
add_test(
        NAME
        test_download_json
        COMMAND
        $<TARGET_FILE:test_download_json>
)

add_executable(test_diff test_diff.c)
target_link_libraries(test_diff diff_branch_pack)
add_test(
        NAME
        test_diff
        COMMAND
        $<TARGET_FILE:test_diff>
)


add_executable(test_print_json test_print_json.c)
target_link_libraries(test_print_json diff_branch_pack)
add_test(
        NAME
        test_print_json
        COMMAND
        $<TARGET_FILE:test_print_json>
)
