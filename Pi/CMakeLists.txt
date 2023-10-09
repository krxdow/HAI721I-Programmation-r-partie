project(Pi)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
set(CMAKE_C_STANDARD 11)

add_executable(p P.c fonctionsProcess.h fonctionsProcess.c)
target_link_libraries(p ${CURSES_LIBRARIES})