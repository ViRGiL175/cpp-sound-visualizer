cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -v")
set(PROJECT_NAME sound-visualizer)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${sound-visualizer_SOURCE_DIR}/out")

project(${PROJECT_NAME})

aux_source_directory(./src/ SOURCE_FILES)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

#[[If you havn't integrated SFML plese use find SFML library method from older commits, eg: 2333909 ]]
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio sfml-window sfml-system)
target_link_libraries(${PROJECT_NAME} opengl32 glu32 pthread glew32)