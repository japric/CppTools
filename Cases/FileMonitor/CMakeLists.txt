SET(LIB_NAME "file_monitor")
SET(BIN_NAME ${LIB_NAME}_main)
include_directories(include)

add_library(${LIB_NAME} SHARED  src/FileMonitor.cpp)
add_executable(${BIN_NAME} src/main.cpp)
target_link_libraries(${BIN_NAME} ${LIB_NAME})