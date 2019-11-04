cmake_minimum_required(VERSION 3.16.0)

project(maketest)

ADD_LIBRARY(LibsModule 
    snake.c
)

target_link_libraries(LibsModule -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf)

add_executable(snake snake.c)

target_link_libraries(snake LibsModule)

install(TARGETS snake DESTINATION bin)

set(DCMAKE_SH="CMAKE_SH-NOTFOUND")