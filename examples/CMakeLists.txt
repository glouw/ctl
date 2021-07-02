cmake_minimum_required(VERSION 3.10)

project(ctlExamples)

# Set up the global common compiler settings
include(../common.cmake)

# Add examples
add_custom_target(examples)
function(AddExample sourceFileNoExtension)
    add_executable(${sourceFileNoExtension} "${sourceFileNoExtension}.c")
    add_dependencies(examples ${sourceFileNoExtension})
endfunction()

AddExample(astar)
AddExample(postfix)
AddExample(json)
AddExample(snow)
AddExample(6502)
