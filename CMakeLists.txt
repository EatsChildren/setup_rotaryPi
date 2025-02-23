cmake_minimum_required(VERSION 3.10)
project(template_project)

#############       SETUP DEFAULT BUILD TYPE       #############
##  YOU CAN SPECIFY with call:                                ##
##  cmake -DCMAKE_BUILD_TYPE=<type>                           ##
##  <type> options: Release, Debug, RelWithDebInfo            ##
################################################################
# Set a default build type if none was specified
set(default_build_type "RelWithDebInfo")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}  CMAKE_CONFIGURATION_TYPES: ${CMAKE_CONFIGURATION_TYPES}")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
else()
  message(STATUS "Selected build type is: '${CMAKE_BUILD_TYPE}'.")
endif()

###########################################################
#############       SPECIFY C++ VERSION       #############
###########################################################
include(CheckCXXCompilerFlag)
set(CMAKE_CXX_STANDARD 20)
#list(APPEND CMAKE_CXX_FLAGS "-std=c++2a")
list(APPEND CMAKE_CXX_FLAGS "-std=c++2a -w -fcompare-debug-second") # to supress eigen warnings due to gcc issue
list(REMOVE_DUPLICATES CMAKE_CXX_FLAGS)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggdb3")


####################################################################
## Collect all C/CPP/CC Files for Compiling and define INCLUDES   ##
####################################################################
file(GLOB_RECURSE PROJECT_SRC_FILES RELATIVE ${PROJECT_SOURCE_DIR} "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.c" )
list(REMOVE_ITEM PROJECT_SRC_FILES  "src/main_blink_led.c"
"src/main_count_pulses.c"
"src/main_clear_pins.c"
"src/main_check_hook.c"  )
SET(PROJECT_INCLUDE_DIRS "include")
file(GLOB_RECURSE TMP LIST_DIRECTORIES true RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include/* )
FOREACH(child ${TMP})
    IF(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${child})
        LIST(APPEND PROJECT_INCLUDE_DIRS ${child})
    ELSE()
        LIST(APPEND PROJECT_HEADERS ${PROJECT_SOURCE_DIR}/${child})
    ENDIF()
ENDFOREACH()
UNSET(TMP)

########################################################
## Find System-wide Packages (like Eigen, PCL, etc..) ##
########################################################
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules")

# find_package(m3r_code_base QUIET)
# if( m3r_code_base_FOUND )
#         list(APPEND PROJECT_LIBS ${M3R_CODE_BASE_LIBS})
#         list(APPEND PROJECT_INCLUDE_DIRS ${M3R_CODE_BASE_INCLUDE_DIRS})
# else()
#         MESSAGE( FATAL_ERROR "<${PROJECT_NAME}> Could not find_package m3r_code_base.  Have you installed it properly?")
# endif()

######## Find Threads Library ########
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
list(APPEND PROJECT_LIBS Threads::Threads)

# ## EIGEN3 Library
# find_package(Eigen3 REQUIRED)
# if(NOT EIGEN3_FOUND)
#     MESSAGE(WARNING "<${PROJECT_NAME}> CMake DID NOT FIND the package {Eigen3}. Try >> sudo apt install libeigen3")
# endif()
# list(APPEND PROJECT_INCLUDE_DIRS ${EIGEN3_INCLUDE_DIRS} )


# ### YAML used for smoother output and other logging ###
# find_package(yaml-cpp REQUIRED)
# if(NOT YAML_CPP_FOUND)
#     MESSAGE("-- WARNING!! CMake DID NOT FIND the package {yaml-cpp}. Try >> sudo apt install libyaml-cpp")
# endif(NOT  YAML_CPP_FOUND)
# list(APPEND PROJECT_INCLUDE_DIRS ${YAML_CPP_INCLUDE_DIR} )
# list(APPEND PROJECT_LIBS ${YAML_CPP_LIB})


#########################
##    Setup Build      ##
#########################
include_directories( ${PROJECT_INCLUDE_DIRS} )

##########################
## Make Project Library ##
##########################
if( PROJECT_SRC_FILES )
        add_library(${PROJECT_NAME} SHARED ${PROJECT_SRC_FILES})
        target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBS})
        export(PACKAGE ${PROJECT_NAME})
        list(APPEND PROJECT_LIBS ${PROJECT_NAME})
endif()

##############################
## Make Project Executibles ##
##############################
add_executable(run_blink src/main_blink_led.c)
add_executable(run_count src/main_count_pulses.c)
add_executable(run_clear src/main_clear_pins.c)
add_executable(run_hook src/main_check_hook.c)

target_link_libraries(run_blink ${PROJECT_LIBS})
target_link_libraries(run_count ${PROJECT_LIBS})
target_link_libraries(run_clear ${PROJECT_LIBS})
target_link_libraries(run_hook ${PROJECT_LIBS})



################
## Add TESTS  ##
################
# file(GLOB_RECURSE TEST_SRC_FILES "${PROJECT_SOURCE_DIR}/test/*.cpp" "${PROJECT_SOURCE_DIR}/test/*.c" )
# find_package(GTest REQUIRED)
# enable_testing()

# add_executable( run_all_tests
#   ${TEST_SRC_FILES}
# )

# target_link_libraries(
#     run_all_tests
#     ${PROJECT_LIBS}
#     gtest
#     gtest_main
# )

# include(GoogleTest)
# gtest_discover_tests(run_all_tests)

