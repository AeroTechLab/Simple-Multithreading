cmake_minimum_required( VERSION 3.10 )
project( Simple-MultiThreading )

set( CMAKE_C_STANDARD 99 )
set( CMAKE_C_STANDARD_REQUIRED ON )

set( LIBRARY_DIR CACHE PATH "Relative or absolute path to directory where built shared libraries will be placed" )

add_library( MultiThreading SHARED ${CMAKE_CURRENT_LIST_DIR}/threads.c ${CMAKE_CURRENT_LIST_DIR}/thread_locks.c ${CMAKE_CURRENT_LIST_DIR}/semaphores.c ${CMAKE_CURRENT_LIST_DIR}/thread_safe_lists.c ${CMAKE_CURRENT_LIST_DIR}/thread_safe_queues.c ${CMAKE_CURRENT_LIST_DIR}/thread_safe_maps.c )
set_target_properties( MultiThreading PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_DIR} )
target_include_directories( MultiThreading PUBLIC ${CMAKE_CURRENT_LIST_DIR} )
target_compile_definitions( MultiThreading PUBLIC -DDEBUG )
target_link_libraries( MultiThreading ${CMAKE_THREAD_LIBS_INIT} )
