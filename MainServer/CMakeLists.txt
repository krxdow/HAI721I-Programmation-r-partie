project(MainServer)

set(CMAKE_C_STANDARD 11)

add_library(project1_lib Pconfig.c)
target_include_directories(project1_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(pconfig Pconfig.c)  # Nom de l'exécutable modifié ici
target_link_libraries(pconfig project1_lib)  # Nom de la cible de liaison modifié ici

