# For Debug Build :: -DCMAKE_BUILD_TYPE=Debug

project( Reveal_scenarios )

cmake_minimum_required(VERSION 2.8.7)

#SET(CMAKE_CXX_COMPILER clang)
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -g")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra -g ")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -Wfatal-errors -g")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra -Wfatal-errors -g ")

# make available any custom .cmakes
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake_modules ${CMAKE_MODULE_PATH} )

#SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
#SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

add_subdirectory( pendulum )
add_subdirectory( grasp )
#add_subdirectory( weaselball )

