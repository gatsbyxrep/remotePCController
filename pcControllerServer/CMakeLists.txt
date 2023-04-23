cmake_minimum_required(VERSION 3.5)

project(pcControllerServer LANGUAGES C)

set(HEADER_FILES misc/datastructures/conqueue.h
    misc/datastructures/node.h
    socket/socket.h
    parsers/rawdataparser.h
    websocket/websocket.h
    misc/SHA1/sha1.h
    service/pcController.h
    misc/datastructures/stack.h
    misc/logger/logger.h
    service/codogram/codogramids.h
    service/codogram/codogrambase.h
    service/codogram/mousemovecodogram.h)

set(SOURCE_FILES main.c
    socket/socket.c
    websocket/websocket.c
    misc/SHA1/sha1.c
    misc/datastructures/stack.c
    misc/datastructures/conqueue.c
    misc/logger/logger.c
    service/codogram/codogrambase.c)

add_executable(pcControllerServer ${SOURCE_FILES} ${HEADER_FILES})

install(TARGETS pcControllerServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
if(WIN32)
  target_link_libraries(pcControllerServer wsock32 ws2_32)
endif()
