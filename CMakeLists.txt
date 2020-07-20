cmake_minimum_required(VERSION 3.2)

project(LearnOpenGL)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH "{CMAKE_SOURCE_DIR}/cmake")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_DEBUG_POSTFIX "_d")

#find_package(OpenGL REQUIRED)
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(EXE_NAME "${PROJECT_NAME}_D")
else()
    set(EXE_NAME ${PROJECT_NAME})
endif()

include_directories(glad glew GLFW glm include)
link_directories(glew GLFW)

set(COMMON_SRC
    src/package/GLManager.h
    src/package/GLManager.cpp
    glad/glad.c
)

set(MAIN_LIST
    hello_window_clear
    two_triangle
    two_triangle2rect
    texture
)

list(GET MAIN_LIST 3 SRC_FILE)
#add_definitions(-DGLEW_STATIC)

add_executable(${PROJECT_NAME} "src/${SRC_FILE}.cpp" ${COMMON_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${EXE_NAME})

target_link_libraries(${PROJECT_NAME} glfw3
#    glew32 opengl32 glu32
    )

#ADD_CUSTOM_TARGET(debug ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Debug ${PROJECT_SOURCE_DIR})
#ADD_CUSTOM_TARGET(release ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Release ${PROJECT_SOURCE_DIR})
