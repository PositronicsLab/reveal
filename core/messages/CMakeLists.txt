cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Core_Messages )

find_package( Protobuf REQUIRED )

add_subdirectory( Reveal )

set( REVEAL_MESSAGES_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE )

