#  Copyright © 2016 Fatih Gazimzyanov. Contacts: virgil7g@gmail.com
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#          http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and

cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -v")
set(PROJECT_NAME sound-visualizer)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${sound-visualizer_SOURCE_DIR}/out")

project(${PROJECT_NAME})

aux_source_directory(./src/ SOURCE_FILES)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

#[[If you haven't integrated SFML please use SFML finding method from older commits]]
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio sfml-window sfml-system)
target_link_libraries(${PROJECT_NAME} opengl32 glu32 pthread glew32)
