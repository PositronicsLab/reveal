cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Client_Gazebo )

find_package( Pthread REQUIRED )
find_package( Boost REQUIRED )

add_definitions( -DMAKE_BIN="${CMAKE_MAKE_PROGRAM}")
add_definitions( -DCMAKE_BIN="${CMAKE_COMMAND}")

find_path( GZSERVER_BIN gzserver )
add_definitions( -DGZSERVER_BIN="${GZSERVER_PATH}/gzserver")

add_executable( gazebo-client ${CMAKE_CURRENT_SOURCE_DIR}/gazebo_client.cpp ${REVEAL_CLIENT_HEADERS} ${REVEAL_CLIENT_SOURCES} )
add_dependencies( gazebo-client ${REVEAL_CORE_LIBRARY} )
#target_link_libraries( gazebo-client LINK_PRIVATE ${REVEAL_CLIENT_LIBRARIES} ${PTHREAD_LIBRARIES} /usr/lib/libboost_system.so.1.46.1 /usr/lib/libboost_filesystem.so.1.46.1 ${BOOST_LIBRARIES} )
target_link_libraries( gazebo-client LINK_PRIVATE ${REVEAL_CLIENT_LIBRARIES} ${PTHREAD_LIBRARIES} )

