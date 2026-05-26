
file(GLOB_RECURSE SRC_UI ${CMAKE_SOURCE_DIR} "main/*.c")

idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "." "include"
    REQUIRES dht20
)
