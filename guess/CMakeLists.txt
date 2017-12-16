cmake_minimum_required(VERSION 3.9)
project(guess)

set(CMAKE_CXX_STANDARD 11)

#add_executable(server
#        io.c
#        io.h
#        Makefile
#        numbers.c
#        numbers.h
#        server.c)


add_executable(client
        client.c
        io.c
        io.h)