#include <iostream>
#ifndef _MSC_VER
#include <libgen.h>
#include <unistd.h>
#elif defined _DEBUG
#include <vld.h>
#endif // _MSC_VER
#include <cstdlib>

#include "glad.h"
#include "glfw3.h"
#include "package/GLManager.h"
#include "package/firstpersoncamera.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const unsigned int screenWidth = 600;
const unsigned int screenHeight = 600;
glm::mat4 cubeModel(1.0f);

FirstPersonCamera ca(0.7f, 0.8f, 4.0f);
const float vt = sqrt(3.0) / 3.0f;
const float side = sqrt(3.0) / 6.0f;

int main(int argc, char **argv)
{
#ifndef _MSC_VER
    chdir(dirname(argv[0]));
#endif
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    ca.setWindowSize(window, screenWidth, screenHeight);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *, int width, int height){ glViewport(0, 0, width, height); });
    glfwSetKeyCallback(window, [](GLFWwindow *_, int key, int scancode, int action, int mods){ ca.keyCallback(_, key, scancode, action, mods); });
    glfwSetCursorPosCallback(window, [](GLFWwindow *_,  double xpos, double ypos){ ca.cursorPosCallback(_, xpos, ypos); });
    glfwSetMouseButtonCallback(window, [](GLFWwindow *_, int button, int action, int mods){ ca.mouseButtonCallback(_, button, action, mods); });
    glfwSetScrollCallback(window, [](GLFWwindow *_,  double xpos, double ypos){ ca.scrollCallback(_, xpos, ypos); });

    float vertices[] =
    {
        0.5f, 0.5f, 0.5f, // front
        0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,

        -0.5f, 0.5f, -0.5f, // back
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,

        -0.5f, 0.5f, 0.5f, // left
        -0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,

        0.5f, 0.5f, -0.5f, // right
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        0.5f, 0.5f, -0.5f, // top
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,

        -0.5f, -0.5f, -0.5f, // bottom
        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, -0.5f,
    };

    float fourspace[] =
    {
        0.0f, -side, vt,
        -0.5f, -side, -side,
        0.5f, -side, -side,

        0.0f, -side, vt,
        -0.5f, -side, -side,
        0.0f, vt, 0.0f,

        -0.5f, -side, -side,
        0.5f, -side, -side,
        0.0f, vt, 0.0f,

        0.0f, -side, vt,
        0.5f, -side, -side,
        0.0f, vt, 0.0f,
    };

    unsigned int indices[] =
    {
        0, 1, 3,
        1, 2, 3,
        4, 5, 7,
        5, 6, 7,
        8, 9, 11,
        9, 10, 11,
        12, 13, 15,
        13, 14, 15,
        16, 17, 19,
        17, 18, 19,
        20, 21, 23,
        21, 22, 23
    };

    float texCoords[] =
    {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,

        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,

        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,

        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,

        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,

        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
    };

    float colors[] =
    {
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,

        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,

        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f,

        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,

        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
    };

    float normals[] =
    {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,

        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,

        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,

        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
    };

    for (int i = 0; i < sizeof(colors) / sizeof(float); i += 3)
    {
        colors[i] = 1.0f;
        colors[i + 1] = 0.50f;
        colors[i + 2] = 0.31f;
    }
    int width, height, nrChannels;
    unsigned char *data = NULL;
    GLManager cube, light;

    std::string pictures[] =
    {
        "llvm.png",
        "QQ2017.png",
        "block.png",
        "webq.png",
        "awesomeface.png",
        "compass_plate.png"
    };

    stbi_set_flip_vertically_on_load(true);
    for (int i = 0; i < 6; ++i)
    {
        std::string folders("../res/");
        folders.append(pictures[i].c_str());
        data = stbi_load(folders.c_str(), &width, &height, &nrChannels, 0);
        cube.genImageData(data, width, height, i, nrChannels);
        free(data);
        data = NULL;
    }

    cube.readShaderFile("../shaders/rectTexture.vert", "../shaders/rectTexture.frag"); // 世界空间
//    cube.readShaderFile("../shaders/viewSpace.vert", "../shaders/viewSpace.frag"); // 观察空间
    cube.setVertexArray(0, 3, vertices, sizeof(vertices));
    cube.setVertexArray(1, 3, colors, sizeof(colors));
    cube.setVertexArray(2, 2, texCoords, sizeof(texCoords));
    cube.setVertexArray(3, 3, normals, sizeof(normals));
    cube.setIndexArray(indices, sizeof(indices));

    light.readShaderFile("../shaders/lightcube.vert", "../shaders/lightcube.frag");
//    light.setVertexArray(0, 3, vertices, sizeof(vertices));
//    light.setIndexArray(indices, sizeof(indices));
    light.setVertexArray(0, 3, fourspace, sizeof(fourspace)); // 用四面体模拟光源

    glm::mat4 view, projection, mvp, lightModel;
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        ca.ProcessWindowEvent(glfwGetTime());

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        view = ca.GetViewMatrix();
        projection = glm::perspective(glm::radians(ca.fovZoom()), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        if (ca.mouseButton() >= 0)
        {
            cubeModel = glm::rotate(cubeModel, glm::radians(ca.rotation()), ca.mouseButton() == 0 ?
                                        glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f));
        }
        // 光源随时间移动
        //lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        //lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;
        //lightPos.z = cos(glfwGetTime() / 2.0f) * 1.0f;

        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);
        
        cube.use();
        cube.setMat4("projection", &projection[0][0]);
        cube.setMat4("view", &view[0][0]);
        cube.setMat4("model", &cubeModel[0][0]);

        cube.setVec3("lightColor", &lightColor[0]);
        cube.setVec3("lightPos", &lightPos[0]);
        cube.setVec3("viewPos", ca.cameraPos());
        cube.paintTriangles(1, 36);

        //lightModel = glm::rotate(lightModel, (float)glm::radians(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
        lightModel = glm::translate(glm::mat4(1.0f), lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.3f));
        mvp = projection * view * lightModel;

        light.use();
        light.setMat4("modViewProj", &mvp[0][0]);
        light.paintTriangles(1, 12);

        //glfwSwapInterval(1);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

