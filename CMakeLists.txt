cmake_minimum_required(VERSION 3.10)
project(diff_branch_pack LANGUAGES C VERSION 1.0.0)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")

add_subdirectory(ext)
add_subdirectory(lib)

add_executable(cli_util
    main.c
)

target_link_libraries(cli_util
    diff_branch_pack
)

target_compile_options(cli_util PRIVATE -Wall -Wextra -Wpedantic -Werror)

enable_testing()
add_subdirectory(tests)

add_subdirectory(pack)


