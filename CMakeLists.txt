cmake_minimum_required(VERSION 3.15)
project(my_project LANGUAGES CXX CUDA) 

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(raylib 3.0 REQUIRED)

set(CMAKE_CXX_STANDARD 26) 
set(CMAKE_BUILD_TYPE Release)
add_executable(${PROJECT_NAME} 
	src/main.cpp
	src/quadtree.cpp
	src/object.cpp
	src/multiprocessing.cu
) 


set_target_properties(${PROJECT_NAME} PROPERTIES
      CUDA_SEPARABLE_COMPILATION ON
      CUDA_ARCHITECTURES 89
)

#target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(${PROJECT_NAME} raylib)
target_link_libraries(${PROJECT_NAME} m)
