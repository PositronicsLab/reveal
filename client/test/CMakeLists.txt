cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Client_tests )

add_executable( test-reveal-client ${CMAKE_CURRENT_SOURCE_DIR}/test_reveal_client.cpp ${REVEAL_CLIENT_HEADERS} ${REVEAL_CLIENT_SOURCES} )
add_dependencies( test-reveal-client ${REVEAL_CORE_LIBRARY} )
target_link_libraries( test-reveal-client LINK_PRIVATE ${REVEAL_CLIENT_LIBRARIES} )

#add_executable( test-bad-client ${CMAKE_CURRENT_SOURCE_DIR}/test_client_bad_queries.cpp ${REVEAL_CLIENT_HEADERS} ${REVEAL_CLIENT_SOURCES} )
#add_dependencies( test-bad-client ${REVEAL_CORE_LIBRARY} )
#target_link_libraries( test-bad-client LINK_PRIVATE ${REVEAL_CLIENT_LIBRARIES} )

