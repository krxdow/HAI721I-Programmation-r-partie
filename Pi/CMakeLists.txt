project(Pi)

set(CMAKE_C_STANDARD 11)

add_library(project2_lib P.c)  # Changer le nom de la bibliothèque en project2_lib
target_include_directories(project2_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(p P.c)  # Changer le nom de l'exécutable en p
target_link_libraries(p project2_lib)  # Lier à la bibliothèque project2_lib
