# add any test programs here

add_executable( test_protobuf ${CMAKE_CURRENT_SOURCE_DIR}/test_protobuf.cpp ${REVEAL_HEADERS} ${REVEAL_SOURCES} )
target_link_libraries( test_protobuf ${REVEAL_LIBRARIES} )

add_executable( test_zeromq ${CMAKE_CURRENT_SOURCE_DIR}/test_zeromq.cpp ${REVEAL_HEADERS} ${REVEAL_SOURCES} )
target_link_libraries( test_zeromq ${REVEAL_LIBRARIES} )

add_executable( test_reveal_server ${CMAKE_CURRENT_SOURCE_DIR}/test_reveal_server.cpp ${REVEAL_HEADERS} ${REVEAL_SOURCES} )
target_link_libraries( test_reveal_server ${REVEAL_LIBRARIES} )

add_executable( test_reveal_client ${CMAKE_CURRENT_SOURCE_DIR}/test_reveal_client.cpp ${REVEAL_HEADERS} ${REVEAL_SOURCES} )
target_link_libraries( test_reveal_client ${REVEAL_LIBRARIES} )


