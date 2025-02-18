cmake_minimum_required(VERSION 3.2)

project(lnopengl)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH "{CMAKE_SOURCE_DIR}/cmake")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_DEBUG_POSTFIX "_d")

set(CMAKE_CXX_STANDARD 11)

# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--subsystem,windows")
if(WIN32)
    set(CMAKE_CXX_FLAGS "-mwindows")
endif()

#find_package(OpenGL REQUIRED)
# if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    # set(EXE_NAME "${PROJECT_NAME}-d")
# else()
    # set(EXE_NAME ${PROJECT_NAME})
# endif()

#if(WIN32)
    #link_directories(GLFW)
#endif(WIN32)

include_directories(glad glfw/include/GLFW glm stb_image)
link_directories(bin)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
set(GLFW_LIBLIST glfw ${GLFW_LIBRARIES})


set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(third/assimp)

set(COMMON_SRC
    glad/glad.c
    src/package/shaderprogram.cpp
    src/package/modelloader.cpp
)

set(MAIN_LIST
    hello_window_clear
    two_triangle
    two_triangle2rect
    texture
    importModel
    sphere
    senior
    skybox
    geometryshader
    instancing
)

# use importModel
list(GET MAIN_LIST 4 SRC_FILE)

set(PROJECT_NAME ${SRC_FILE})

add_executable(${PROJECT_NAME} "src/${SRC_FILE}.cpp" ${COMMON_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} ${GLFW_LIBLIST}
    assimp
)

