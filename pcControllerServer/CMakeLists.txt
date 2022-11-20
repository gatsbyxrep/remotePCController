cmake_minimum_required(VERSION 3.5)

project(pcControllerServer LANGUAGES C)

set(HEADER_FILES socket/socket.h parsers/rawdataparser.h service/commands/commands.h websocket/websocket.h misc/SHA1/sha1.h service/pcController.h)
set(SOURCE_FILES main.c socket/socket.c websocket/websocket.c misc/SHA1/sha1.c)

add_executable(pcControllerServer ${SOURCE_FILES} ${HEADER_FILES})

install(TARGETS pcControllerServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
if(WIN32)
  target_link_libraries(pcControllerServer wsock32 ws2_32)
endif()
