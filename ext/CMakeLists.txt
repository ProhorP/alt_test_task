set(EXTRACT_DIR "${CMAKE_BINARY_DIR}/ext")
#curl################################################3

set(CURL_EXTRACT_DIR "${EXTRACT_DIR}/curl-8.7.1")
set(CURL_EXTRACT_DIR_BUILD "${EXTRACT_DIR}/curl_build")
if(NOT EXISTS "${CURL_EXTRACT_DIR}")
    execute_process(
        COMMAND bash -c "wget https://git.altlinux.org/tasks/345770/build/100/x86_64/srpm/curl-8.7.1-alt2.src.rpm && rpm2cpio curl-8.7.1-alt2.src.rpm | cpio -id && tar -xf curl-8.7.1.tar"
        WORKING_DIRECTORY ${EXTRACT_DIR}
        # RESULT_VARIABLE FF_MS_RES
    )
endif()

# Устанавливаем флаг сборки статической библиотеки
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "Enable position-independent code (PIC) for static libraries" FORCE)

# Теперь можно добавить подкаталог с исходниками curl
add_subdirectory(${CURL_EXTRACT_DIR} ${CURL_EXTRACT_DIR_BUILD})


#cjson##################################

set(cJSON_EXTRACT_DIR "${EXTRACT_DIR}/cjson-1.7.16")
set(cJSON_EXTRACT_DIR_BUILD "${EXTRACT_DIR}")
if(NOT EXISTS "${cJSON_EXTRACT_DIR}")
    execute_process(
        COMMAND bash -c "wget https://git.altlinux.org/tasks/345106/build/40/x86_64/srpm/cjson-1.7.16-alt0.1.src.rpm && rpm2cpio cjson-1.7.16-alt0.1.src.rpm | cpio -id && tar -xf cjson-1.7.16.tar"
        WORKING_DIRECTORY ${EXTRACT_DIR}
        # RESULT_VARIABLE FF_MS_RES
    )
endif()

add_library(cjson STATIC "${EXTRACT_DIR}/cjson-1.7.16/cJSON.c")

target_include_directories(cjson
        PUBLIC
        ${cJSON_EXTRACT_DIR}
)
