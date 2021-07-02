# Global settings for Tests and Examples

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

macro(AddFlag MSVCFlag GCCFlag)
    if(MSVC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MSVCFlag}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCCFlag}")
    endif()
endmacro()

AddFlag("${CMAKE_C_FLAGS} /W3 /sdl-" "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Wfatal-errors -Wshadow -march=native -g")

if(${Optimization} STREQUAL 0)
    AddFlag(/Od -O0)
elseif(${Optimization} STREQUAL 1)
    AddFlag(/Ox -O1)
elseif(${Optimization} STREQUAL 2)
    AddFlag(/O1 -O2)
elseif(${Optimization} STREQUAL 3)
    AddFlag(/O2 -O3)
elseif(${Optimization} STREQUAL Debug)
    AddFlag("" -Og)
elseif(${Optimization} STREQUAL Fast)
    AddFlag("/O2 /Ot" -Ofast)
elseif(${Optimization} STREQUAL Size)
    AddFlag("/O1 /Os" -Os)
endif()

if(${LONG})
    add_compile_definitions(LONG)
endif()

if(${SANITIZE})
    AddFlag("/fsanitize=address" "-fsanitize=address -fsanitize=undefined")
endif()

if(${SRAND})
    add_compile_definitions(SRAND)
endif()

message("Using flag: ${CMAKE_C_FLAGS}")

include_directories(../ctl) # A dirty solution, but works for tests and examples
