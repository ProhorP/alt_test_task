# include(FetchContent)

# FetchContent_Declare(
#     curl_fetch
#     URL https://git.altlinux.org/tasks/345770/build/100/x86_64/srpm/curl-8.7.1-alt2.src.rpm
#     # URL_HASH SHA256=${hash}
#     DOWNLOAD_NO_EXTRACT true
# )

# if(NOT curl_fetch_POPULATED)
#     FetchContent_Populate(curl_fetch)
# endif()

# set(CURL_RPM_PATH "${curl_fetch_SOURCE_DIR}/curl-8.7.1-alt2.src.rpm")

# message("!!!!!CURL_RPM_PATH = ${CURL_RPM_PATH}")

# # Определяем директорию для извлеченных файлов
# set(CURL_EXTRACT_DIR "${CMAKE_BINARY_DIR}/ext")

# message("!!!!!CURL_EXTRACT_DIR= CURL_EXTRACT_DIR ${CURL_EXTRACT_DIR}")

# # Создаем пользовательскую команду для извлечения
# add_custom_command(
#     OUTPUT ${CURL_EXTRACT_DIR}/CMakeLists.txt
#     COMMAND ${CMAKE_COMMAND} -E make_directory ${CURL_EXTRACT_DIR}
#     COMMAND rpm2cpio ${CURL_RPM_PATH} | cpio -id --no-absolute-filenames
#     COMMAND tar -xf curl-8.7.1.tar -C ${CURL_EXTRACT_DIR}
#     WORKING_DIRECTORY ${curl_fetch_SOURCE_DIR}
#     COMMENT "!!!!!Extracting curl from RPM and untarring..."
#     VERBATIM
# )

# # Создаем цель, которая будет зависеть от пользовательской команды
# add_custom_target(extract_curl ALL
#     DEPENDS ${CURL_EXTRACT_DIR}/CMakeLists.txt
# )

# #Установим флаг сборки статической библиотеки для curl
# set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)

# # Включаем поддержку Position Independent Code (PIC) для всех библиотек
# set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "Enable position-independent code (PIC) for static libraries" FORCE)

# # Добавляем подкаталог с исходниками curl, если он существует
# # if(EXISTS ${CURL_EXTRACT_DIR}/curl-8.7.1)
#     add_subdirectory(${CURL_EXTRACT_DIR}/curl-8.7.1 ${CMAKE_BINARY_DIR}/curl_build)
# # endif()

#################################################

# #Установим флаг сборки статической библиотеки для curl
# set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)

# # Включаем поддержку Position Independent Code (PIC) для всех библиотек
# set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "Enable position-independent code (PIC) for static libraries" FORCE)

# # Укажи путь к исходникам curl
# add_subdirectory(curl-8.7.1)

########################################

# include(FetchContent)

# FetchContent_Declare(
#     curl_fetch
#     URL https://git.altlinux.org/tasks/345770/build/100/x86_64/srpm/curl-8.7.1-alt2.src.rpm
#     DOWNLOAD_NO_EXTRACT true
# )

# if(NOT curl_fetch_POPULATED)
#     FetchContent_Populate(curl_fetch)
# endif()

# set(CURL_RPM_PATH "${curl_fetch_SOURCE_DIR}/curl-8.7.1-alt2.src.rpm")
# set(CURL_EXTRACT_DIR "${CMAKE_BINARY_DIR}/ext")

# # # Проверяем, был ли процесс извлечения выполнен ранее, используя флаг
# # if(NOT EXISTS "${CURL_EXTRACT_DIR}/curl-8.7.1")
# #     message("!!!!!CURL_RPM_PATH = ${CURL_RPM_PATH}")
# #     message("!!!!!CURL_EXTRACT_DIR = ${CURL_EXTRACT_DIR}")

#     # Создаем директорию для извлеченных файлов
#     file(MAKE_DIRECTORY ${CURL_EXTRACT_DIR})

#     # Выполняем команды по очереди
#     # 1. Конвертируем RPM в CPIO и сохраняем в файл
#     execute_process(
#         COMMAND rpm2cpio ${CURL_RPM_PATH} > ${CPIO_OUTPUT_FILE}
#         WORKING_DIRECTORY ${curl_fetch_SOURCE_DIR}
#     )

#     # # 2. Извлекаем CPIO архив
#     # execute_process(
#     #     COMMAND cpio -id --no-absolute-filenames < ${CPIO_OUTPUT_FILE}
#     #     WORKING_DIRECTORY ${curl_fetch_SOURCE_DIR}
#     # )

#     # # 3. Распаковываем tar
#     # execute_process(
#     #     COMMAND tar -xf curl-8.7.1.tar -C ${CURL_EXTRACT_DIR}
#     #     WORKING_DIRECTORY ${curl_fetch_SOURCE_DIR}
#     # )

# #     message("!!!!!Curl extracted successfully.")
# # else()
# #     message("!!!!!Curl already extracted.")
# # endif()

# # # Выполняем процесс один раз
# # if(NOT CACHED_CURL_EXTRACTED)
# #     extract_curl()
# #     set(CACHED_CURL_EXTRACTED TRUE CACHE INTERNAL "Curl has been extracted")
# # endif()

# # Устанавливаем флаг сборки статической библиотеки
# set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
# set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "Enable position-independent code (PIC) for static libraries" FORCE)

# # Теперь можно добавить подкаталог с исходниками curl
# if(EXISTS ${CURL_EXTRACT_DIR}/curl-8.7.1)
#     add_subdirectory(${CURL_EXTRACT_DIR}/curl-8.7.1 ${CMAKE_BINARY_DIR}/curl_build)
# endif()

###############################


set(EXTRACT_DIR "${CMAKE_BINARY_DIR}/ext")
set(CURL_EXTRACT_DIR "${EXTRACT_DIR}/curl-8.7.1")
set(CURL_EXTRACT_DIR_BUILD "${EXTRACT_DIR}/curl_build")
if(NOT EXISTS "${CURL_EXTRACT_DIR}")
    execute_process(
        COMMAND bash -c "wget https://git.altlinux.org/tasks/345770/build/100/x86_64/srpm/curl-8.7.1-alt2.src.rpm && rpm2cpio curl-8.7.1-alt2.src.rpm | cpio -id && tar -xf curl-8.7.1.tar"
        WORKING_DIRECTORY ${EXTRACT_DIR}
        RESULT_VARIABLE FF_MS_RES
    )
endif()

# Устанавливаем флаг сборки статической библиотеки
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "Enable position-independent code (PIC) for static libraries" FORCE)

# Теперь можно добавить подкаталог с исходниками curl
add_subdirectory(${CURL_EXTRACT_DIR} ${CURL_EXTRACT_DIR_BUILD})
