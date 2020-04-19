# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.10.2)

# Project's name
project(isp)

# Build for C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#packages
find_package(OpenCV REQUIRED)

#add all cpp files
#file(GLOB SOURCES "src/*.cpp")
#file(GLOB SOURCES "scripts/*.cpp")

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}"
                    "${PROJECT_SOURCE_DIR}/src"
                    "${PROJECT_SOURCE_DIR}/script"
                    ${OpenCV_INCLUDE_DIRS})
              
#source code
add_subdirectory(src)

#executables
add_executable(try "${PROJECT_SOURCE_DIR}/scripts/test.cpp")
add_executable(demo "${PROJECT_SOURCE_DIR}/scripts/demosaicing.cpp")
add_executable(padd "${PROJECT_SOURCE_DIR}/scripts/padding.cpp")
add_executable(hist "${PROJECT_SOURCE_DIR}/scripts/histogramEqualization.cpp")
add_executable(low "${PROJECT_SOURCE_DIR}/scripts/lowPassFilter.cpp")
add_executable(pyr "${PROJECT_SOURCE_DIR}/scripts/pyramid.cpp")

#lib linking
set(LIBS ${OpenCV_LIBS} src)
target_link_libraries(try ${LIBS})
target_link_libraries(demo ${LIBS})
target_link_libraries(padd ${LIBS})
target_link_libraries(hist ${LIBS})
target_link_libraries(low ${LIBS})
target_link_libraries(pyr ${LIBS})
