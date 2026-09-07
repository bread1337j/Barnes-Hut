cmake_minimum_required(VERSION 3.15)
project(my_project) 

find_package(raylib 3.0 REQUIRED)

set(CMAKE_CXX_STANDARD 26) 


add_executable(${PROJECT_NAME} 
	src/main.cpp
	src/quadtree.cpp
	src/object.cpp
) 



#target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(${PROJECT_NAME} raylib)
target_link_libraries(${PROJECT_NAME} m)
