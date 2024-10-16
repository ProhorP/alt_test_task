install(
    TARGETS
    cli_util
    RUNTIME
    COMPONENT diff_branch_pack
)
install(
    TARGETS
    diff_branch_pack
    LIBRARY
    COMPONENT diff_branch_pack
)

install(
    FILES ${CMAKE_SOURCE_DIR}/lib/diff_branch_pack.h
    DESTINATION include
    COMPONENT diff_branch_pack
)

# Определение компонентов и их содержимого (diff_branch_pack)
set(COMPONENT_diff_branch_pack diff_branch_pack)

set(CPACK_PACKAGE_HOMEPAGE_URL "https://example.com/")
set(CPACK_PACKAGE_VENDOR "Example, Inc <info@example.com>")

# Описания компонентов
set(CPACK_COMPONENT_diff_branch_pack_DESCRIPTION "diff branch pack")

set(CPACK_GENERATOR "RPM")
set(CPACK_GENERATOR_diff_branch_pack "RPM")

#установка информации в RPM
set(CPACK_RPM_PACKAGE_GROUP "Networking/Libraries")
set(CPACK_RPM_PACKAGE_VENDOR ${CPACK_PACKAGE_VENDOR})
set(CPACK_RPM_diff_branch_pack_PACKAGE_SUMMARY ${CPACK_COMPONENT_diff_branch_pack_DESCRIPTION})
# Включение опции для создания релокатабельных RPM-пакетов
set(CPACK_RPM_PACKAGE_RELOCATABLE ON)
# Разрешение установки компонентов пакета
set(CPACK_RPM_COMPONENT_INSTALL ON)
# Зависимости для компонента (RPM)
set(CPACK_RPM_diff_branch_pack_PACKAGE_REQUIRES "ssl, crypto")

# Указание доступных компонентов для установки
set(CPACK_COMPONENTS_ALL "diff_branch_pack")

include(CPack)
