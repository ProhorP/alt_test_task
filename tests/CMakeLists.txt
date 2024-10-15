
add_executable(test_01 test_01.c)
target_link_libraries(test_01 diff_branch_pack)
add_test(
        NAME
        test_01
        COMMAND
        $<TARGET_FILE:test_01>
)
