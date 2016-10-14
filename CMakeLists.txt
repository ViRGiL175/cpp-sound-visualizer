cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -v")
set(PROJECT_NAME sound-visualizer)
set(INSTALL_PATH ./out)
set(AQUILA_DIRECTORY "D:/Code/Libs/aquila-lib")

project(${PROJECT_NAME})

aux_source_directory(./src/ SOURCE_FILES)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

find_library(AQUILA_LIB libAquila.a ${AQUILA_DIRECTORY}/lib)
include_directories(${AQUILA_DIRECTORY}/include/aquila)
target_link_libraries(${PROJECT_NAME} ${AQUILA_LIB})

find_library(OOURA_LIB libOoura_fft.a ${AQUILA_DIRECTORY}/lib)
target_link_libraries(${PROJECT_NAME} ${OOURA_LIB})

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio sfml-window sfml-system)
target_link_libraries(${PROJECT_NAME} opengl32 glu32 pthread glew32)