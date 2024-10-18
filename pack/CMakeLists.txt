install(
    TARGETS
    cli_util
    RUNTIME
    COMPONENT alt
)
install(
    TARGETS
    diff_branch_pack
    LIBRARY
    COMPONENT alt
)

install(
    FILES ${CMAKE_SOURCE_DIR}/lib/diff_branch_pack.h
    DESTINATION "include"
    COMPONENT alt
)

# Определение компонентов и их содержимого (alt)
set(COMPONENT_alt alt)

set(CPACK_PACKAGE_HOMEPAGE_URL "https://example.com/")
set(CPACK_PACKAGE_VENDOR "Example, Inc <info@example.com>")

# Описания компонентов
set(CPACK_COMPONENT_alt_DESCRIPTION "diff branch pack")

set(CPACK_GENERATOR "RPM")
set(CPACK_GENERATOR_alt "RPM")

#установка информации в RPM
set(CPACK_RPM_PACKAGE_GROUP "Networking/Libraries")
set(CPACK_RPM_PACKAGE_VENDOR ${CPACK_PACKAGE_VENDOR})
set(CPACK_RPM_alt_PACKAGE_SUMMARY ${CPACK_COMPONENT_alt_DESCRIPTION})
# Включение опции для создания релокатабельных RPM-пакетов
set(CPACK_RPM_PACKAGE_RELOCATABLE ON)
# Разрешение установки компонентов пакета
set(CPACK_RPM_COMPONENT_INSTALL ON)
# Зависимости для компонента (RPM)
# set(CPACK_RPM_alt_PACKAGE_REQUIRES "systemd")

# Указание доступных компонентов для установки
set(CPACK_COMPONENTS_ALL "alt")

include(CPack)
