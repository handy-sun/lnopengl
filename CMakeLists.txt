cmake_minimum_required(VERSION 3.2)

project(LearnOpenGL)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH "{CMAKE_SOURCE_DIR}/cmake")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_DEBUG_POSTFIX "_d")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_STANDARD 11)

#find_package(OpenGL REQUIRED)
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(EXE_NAME "${PROJECT_NAME}_D")
else()
    set(EXE_NAME ${PROJECT_NAME})
endif()

#if(WIN32)
    #link_directories(GLFW)
#endif(WIN32)

include_directories(glad glfw/include/GLFW glm stb_image assimp)
link_directories(assimp)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glfw)
set(GLFW_LIBLIST glfw ${GLFW_LIBRARIES})

set(COMMON_SRC
    glad/glad.c
    src/package/shaderprogram.h
    src/package/shaderprogram.cpp
    src/package/firstpersoncamera.hpp
    # src/package/model.h
    # src/package/mesh.h
    # src/package/shader.h
    src/package/modelloader.h
    src/package/modelloader.cpp
    src/package/common.h
)

set(MAIN_LIST
    hello_window_clear
    two_triangle
    two_triangle2rect
    texture
    importModel
    model_loading
    sphere
)

list(GET MAIN_LIST 6 SRC_FILE)
#add_definitions(-DGLEW_STATIC)

add_executable(${PROJECT_NAME} "src/${SRC_FILE}.cpp" ${COMMON_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${EXE_NAME})

target_link_libraries(${PROJECT_NAME} ${GLFW_LIBLIST} 
    assimp.dll
)

#ADD_CUSTOM_TARGET(debug ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Debug ${PROJECT_SOURCE_DIR})
#ADD_CUSTOM_TARGET(release ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Release ${PROJECT_SOURCE_DIR})
