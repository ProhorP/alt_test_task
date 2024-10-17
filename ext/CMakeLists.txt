set(EXTRACT_DIR "${CMAKE_BINARY_DIR}/ext")
#curl################################################3

set(CURL_Version "8.7.1")
set(CURL_EXTRACT_DIR "${EXTRACT_DIR}/curl-${CURL_Version}")
if(NOT EXISTS "${CURL_EXTRACT_DIR}")
    execute_process(
        COMMAND bash -c "wget https://git.altlinux.org/tasks/345770/build/100/x86_64/srpm/curl-${CURL_Version}-alt2.src.rpm && rpm2cpio curl-${CURL_Version}-alt2.src.rpm | cpio -id && tar -xf curl-${CURL_Version}.tar"
        WORKING_DIRECTORY ${EXTRACT_DIR}
    )
endif()

# Устанавливаем флаг сборки статической библиотеки
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "Enable position-independent code (PIC) for static libraries" FORCE)

# Теперь можно добавить подкаталог с исходниками curl
# add_subdirectory(${CURL_EXTRACT_DIR} ${CURL_EXTRACT_DIR})
add_subdirectory(${CURL_EXTRACT_DIR} ${CURL_EXTRACT_DIR})



#cjson##################################

set(cJSON_Version "1.7.16")
set(cJSON_EXTRACT_DIR "${EXTRACT_DIR}/cjson-${cJSON_Version}")
if(NOT EXISTS "${cJSON_EXTRACT_DIR}")
    execute_process(
        COMMAND bash -c "wget https://git.altlinux.org/tasks/345106/build/40/x86_64/srpm/cjson-${cJSON_Version}-alt0.1.src.rpm && rpm2cpio cjson-${cJSON_Version}-alt0.1.src.rpm | cpio -id && tar -xf cjson-${cJSON_Version}.tar"
        WORKING_DIRECTORY ${EXTRACT_DIR}
    )
endif()

add_library(cjson STATIC "${EXTRACT_DIR}/cjson-${cJSON_Version}/cJSON.c")

target_include_directories(cjson
    PUBLIC
    ${cJSON_EXTRACT_DIR}
)


#uthash##################################

set(uthash_Version "2.3.0")
set(uthash_EXTRACT_DIR "${EXTRACT_DIR}/libuthash-devel-${uthash_Version}/src")
if(NOT EXISTS "${uthash_EXTRACT_DIR}")
    execute_process(
        COMMAND bash -c "wget https://git.altlinux.org/tasks/273064/build/100/x86_64/srpm/libuthash-devel-${uthash_Version}-alt1.src.rpm && rpm2cpio libuthash-devel-${uthash_Version}-alt1.src.rpm | cpio -id && tar -xf libuthash-devel-${uthash_Version}.tar"
        WORKING_DIRECTORY ${EXTRACT_DIR}
    )
endif()

add_library(uthash INTERFACE)

target_include_directories(uthash
    INTERFACE
    ${uthash_EXTRACT_DIR}
)
