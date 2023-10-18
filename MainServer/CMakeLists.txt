project(MainServer)

set(CMAKE_C_STANDARD 11)

add_executable(pconfig Pconfig.c)
target_include_directories(pconfig PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
