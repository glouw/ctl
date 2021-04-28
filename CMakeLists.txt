cmake_minimum_required(VERSION 3.10)

project(ctl
    VERSION 3.0
    HOMEPAGE_URL "https://github.com/glouw/ctl"
    DESCRIPTION "CTL is a fast compiling, type safe, header only, template-like library for ISO C99/C11."
)

# Globals
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(LONG OFF)
option(SANITIZE OFF)
option(SRAND ON)

set(OptimizationLevel Debug 0 1 2 3 Fast Size)
set(Optimization Debug CACHE STRING "Level of optimization")
set_property(CACHE Optimization PROPERTY STRINGS ${OptimizationLevel})

if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Wfatal-errors -Wshadow -march=native -g")
endif()

macro(AddFlag MSVCFlag GCCFlag)
    if(MSVC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MSVCFlag}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCCFlag}")
    endif()
endmacro()

if(Optimization STREQUAL 0)
    AddFlag(/Od -O0)
elseif(Optimization STREQUAL 1)
    AddFlag(/Ox -O1)
elseif(Optimization STREQUAL 2)
    AddFlag(/O1 -O2)
elseif(Opmitization STREQUAL 3)
    AddFlag(/O2 -O3)
endif()

include_directories(ctl)

function(AddExecutableInDir dir target sourceFileNoExtension)
    find_file(${sourceFileNoExtension}.c ${sourceFileNoExtension}.c ${CMAKE_SOURCE_DIR}/${dir})
    if(${sourceFileNoExtension}.c)
        message("Found ${sourceFileNoExtension}.c")
        add_executable(${sourceFileNoExtension} "${dir}/${sourceFileNoExtension}.c")
    else()
        message("Found ${sourceFileNoExtension}.cc")
        add_executable(${sourceFileNoExtension} "${dir}/${sourceFileNoExtension}.cc")
    endif()
    add_dependencies(${target} ${sourceFileNoExtension})
endfunction()



# TESTS
set(testFileDir tests/func)
function(AddTest sourceFileNoExtension)
    AddExecutableInDir(${testFileDir} tests ${sourceFileNoExtension})
endfunction()

add_custom_target(tests)
AddTest(test_c11)
AddTest(test_container_composing)
AddTest(test_deq)
AddTest(test_lst)
AddTest(test_str)
AddTest(test_pqu)
AddTest(test_que)
AddTest(test_set)
AddTest(test_ust)
AddTest(test_stk)
AddTest(test_vec_capacity)
AddTest(test_vec)

# Examples
set(exampleFileDir examples)
function(AddExample sourceFileNoExtension)
    AddExecutableInDir(${exampleFileDir} examples ${sourceFileNoExtension})
endfunction()

add_custom_target(examples)
AddExample(astar)
AddExample(postfix)
AddExample(json)
AddExample(snow)
AddExample(6502)

# Package to a library
add_library(ctl INTERFACE)
set(ctlIncludeDir ${CMAKE_SOURCE_DIR}/ctl)
target_include_directories(ctl INTERFACE ${ctlIncludeDir})

# Install
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(ctlDir ctl-${PROJECT_VERSION})
message(${ctlDir})
message(${CMAKE_INSTALL_FULL_INCLUDEDIR})

install(
    TARGETS ctl
    EXPORT ctlTarget
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
    FILES ${PROJECT_BINARY_DIR}/${versionFile}
    DESTINATION ${CMAKE_INSTALL_PREFIX}
)

install(
    DIRECTORY ${ctlIncludeDir}
    DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}
)

# Uninstall
file(
    WRITE Uninstall.cmake
    "message(\"Removing ${CMAKE_INSTALL_PREFIX}\")
    file(REMOVE_RECURSE \"${CMAKE_INSTALL_PREFIX}\")"
)
add_custom_target(
    Uninstall
    COMMAND cmake -P Uninstall.cmake
)
