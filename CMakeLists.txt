cmake_minimum_required(VERSION 3.10)

project(
    ctl
    VERSION 3.0
    HOMEPAGE_URL "https://github.com/glouw/ctl"
    DESCRIPTION "CTL is a fast compiling, type safe, header only, template-like library for ISO C99/C11."
)

    
# Package to a library
add_library(ctl INTERFACE)
set(ctlIncludeDir ${PROJECT_SOURCE_DIR}/ctl)
target_include_directories(ctl INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ctl>)

# Install
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)


install(
    TARGETS ctl
    EXPORT ctlTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}
)

set(versionFile ctlConfigVersion.cmake)
write_basic_package_version_file(
    ${versionFile}
    VERSION ${CMAKE_PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    ARCH_INDEPENDENT
)
install(
    EXPORT ctlTargets
    FILE ctlConfig.cmake
    DESTINATION ${CMAKE_INSTALL_PREFIX}
)
install(
    FILES ${PROJECT_BINARY_DIR}/${versionFile}
    DESTINATION ${CMAKE_INSTALL_PREFIX}
)

install(
    DIRECTORY ${ctlIncludeDir}
    DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}
)

# Uninstall
add_custom_target(
    Uninstall
    COMMAND cmake -E rm -rf ${CMAKE_INSTALL_PREFIX}/ctl 
    ${CMAKE_INSTALL_PREFIX}/ctlConfig.cmake 
    ${CMAKE_INSTALL_PREFIX}/${versionFile}
)
