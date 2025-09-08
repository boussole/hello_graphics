cmake_minimum_required(VERSION 3.10)

# Set some basic project attributes
project (graphics VERSION 0.1)

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")

#add_subdirectory(hello_opengl)

add_subdirectory(interactive_computer_graphics)
