cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Server_tests )

add_executable( test-reveal-server ${CMAKE_CURRENT_SOURCE_DIR}/test_reveal_server.cpp ${REVEAL_SERVER_HEADERS} ${REVEAL_SERVER_SOURCES} )
add_dependencies( test-reveal-server ${REVEAL_CORE_LIBRARY} ${REVEAL_DB_LIBRARY} )
target_link_libraries( test-reveal-server LINK_PRIVATE ${REVEAL_SERVER_LIBRARIES} )

