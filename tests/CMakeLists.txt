cmake_minimum_required(VERSION 3.10)

project(ctlTest)

# Set up the global common compiler settings
include(../common.cmake)

# Add tests
add_custom_target(tests)
set(testFileDir func)

function(AddTest sourceFileNoExtension)
    find_file(${sourceFileNoExtension}.c ${sourceFileNoExtension}.c ${testFileDir})
    if(${sourceFileNoExtension}.c)
        message("Found ${sourceFileNoExtension}.c")
        add_executable(${sourceFileNoExtension} "${testFileDir}/${sourceFileNoExtension}.c")
    else()
        message("Found ${sourceFileNoExtension}.cc")
        add_executable(${sourceFileNoExtension} "${testFileDir}/${sourceFileNoExtension}.cc")
    endif()
    add_dependencies(tests ${sourceFileNoExtension})
endfunction()

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
