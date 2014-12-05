cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Server_tests )

add_executable( test-reveal-server ${CMAKE_CURRENT_SOURCE_DIR}/test_reveal_server.cpp ${REVEAL_SERVER_HEADERS} ${REVEAL_SERVER_SOURCES} )
add_dependencies( test-reveal-server ${REVEAL_CORE_LIBRARY} ${REVEAL_DB_LIBRARY} )
target_link_libraries( test-reveal-server LINK_PRIVATE ${REVEAL_SERVER_LIBRARIES} )

add_executable( test-create-test-users ${CMAKE_CURRENT_SOURCE_DIR}/test_create_test_users.cpp ${REVEAL_SERVER_HEADERS} ${REVEAL_SERVER_SOURCES} )
add_dependencies( test-create-test-users ${REVEAL_CORE_LIBRARY} ${REVEAL_DB_LIBRARY} )
target_link_libraries( test-create-test-users LINK_PRIVATE ${REVEAL_SERVER_LIBRARIES} )


