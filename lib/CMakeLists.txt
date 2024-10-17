add_library(diff_branch_pack SHARED
    diff_branch_pack.c
)

target_link_libraries(diff_branch_pack
    cjson
    libcurl_static
    uthash
)

target_compile_options(diff_branch_pack PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_include_directories(diff_branch_pack
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)
