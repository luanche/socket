cmake_minimum_required(VERSION 3.0)

project(socket)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

aux_source_directory(./src/client CLIENT_SRC_LIST)
aux_source_directory(./src/server SERVER_SRC_LIST)

add_executable(client ${CLIENT_SRC_LIST})
add_executable(server ${SERVER_SRC_LIST})
