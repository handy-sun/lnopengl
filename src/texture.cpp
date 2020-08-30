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
#include "package/ShaderProgram.h"
#include "package/firstpersoncamera.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const unsigned int screenWidth = 800;
const unsigned int screenHeight = 600;

FirstPersonCamera ca(0.0f, 0.0f, 4.0f);
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
    ShaderProgram cube, light;

    std::string pictures[] =
    {
        "container2.png",
        "container2_specular.png"     
    };

    stbi_set_flip_vertically_on_load(true);

    for (int i = 0; i < sizeof(pictures) / sizeof(std::string); ++i)
    {
        std::string folders("../res/");
        folders.append(pictures[i].c_str());
        data = stbi_load(folders.c_str(), &width, &height, &nrChannels, 0);
        cube.genImageData(data, width, height, nrChannels);
        free(data);
    }

    cube.addShaderFile("../shaders/rectTexture.vert", "../shaders/rectTexture.frag"); // 世界空间
//    cube.genVertexArray();
//    cube.addShaderFile("../shaders/viewSpace.vert", "../shaders/viewSpace.frag"); // 观察空间
    cube.setVertexAttribute(0, 3, vertices, sizeof(vertices));
    cube.setVertexAttribute(1, 3, colors, sizeof(colors));
    cube.setVertexAttribute(2, 2, texCoords, sizeof(texCoords));
    cube.setVertexAttribute(3, 3, normals, sizeof(normals));
    cube.setIndexArray(indices, sizeof(indices));

    light.addShaderFile("../shaders/lightcube.vert", "../shaders/lightcube.frag");
//    light.genVertexArray();
//    light.setVertexAttribute(0, 3, vertices, sizeof(vertices));
//    light.setIndexArray(indices, sizeof(indices));
    light.setVertexAttribute(0, 3, fourspace, sizeof(fourspace)); // 用四面体模拟光源

    glm::mat4 view, projection, mvp, lightModel;
    glm::mat4 cubeModel(1.0f), model(1.0f);
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -7.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };
    char buf[256];

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    cube.use();
    cube.setInt1("sampDiffuse", 0);
    cube.setInt1("sampSpecular", 1);

    while (!glfwWindowShouldClose(window))
    {
        ca.ProcessWindowEvent(glfwGetTime());
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        view = ca.GetViewMatrix();
        projection = glm::perspective(glm::radians(ca.fovZoom()), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        if (ca.mouseButton() >= 0)
        {
            cubeModel = glm::rotate(cubeModel, glm::radians(ca.rotation()), ca.mouseButton() == 0 ?
                                        glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f));
        }
        
        cube.use();
        cube.setFloat("material.shininess", 32.0f);
        cube.setVec3("viewPos", ca.cameraPos());
        cube.setMat4("projection", &projection[0][0]);
        cube.setMat4("view", &view[0][0]);
//        cube.setVec3("lightDirection", ca.cameraFront());
//        cube.setVec3("lightColor", &lightColor[0]);
//        cube.setVec3("lightPos", ca.cameraPos());
        cube.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        cube.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        cube.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        cube.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        for (int i = 0; i < 4; ++i)
        {
            sprintf(buf, "pointLights[%d].position", i);
            cube.setVec3(buf, glm::value_ptr(pointLightPositions[i]));
            sprintf(buf, "pointLights[%d].ambient", i);
            cube.setVec3(buf, 0.05f, 0.05f, 0.05f);
            sprintf(buf, "pointLights[%d].diffuse", i);
            cube.setVec3(buf, 0.8f, 0.8f, 0.8f);
            sprintf(buf, "pointLights[%d].specular", i);
            cube.setVec3(buf, 1.0f, 1.0f, 1.0f);
            sprintf(buf, "pointLights[%d].constant", i);
            cube.setFloat(buf, 1.0f);
            sprintf(buf, "pointLights[%d].linear", i);
            cube.setFloat(buf, 0.09f);
            sprintf(buf, "pointLights[%d].quadratic", i);
            cube.setFloat(buf, 0.032f);
        }

//        cube.setVec3("spotLight.position", ca.cameraPos());
//        cube.setVec3("spotLight.direction", ca.cameraFront());
        cube.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        cube.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        cube.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        cube.setFloat("spotLight.constant", 1.0f);
        cube.setFloat("spotLight.linear", 0.09);
        cube.setFloat("spotLight.quadratic", 0.032);
        cube.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        cube.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        for (int i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i + 20.0f;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            cube.setMat4("model", &model[0][0]);

            cube.drawTrianglesElements(1, 36);
        }

        for (unsigned int i = 0; i < sizeof(pointLightPositions) / sizeof(glm::vec3); i++)
        {
            lightModel = glm::translate(glm::mat4(1.0f), pointLightPositions[i]);
            lightModel = glm::scale(lightModel, glm::vec3(0.2f));
            mvp = projection * view * lightModel;
            light.use();
            light.setMat4("modViewProj", &mvp[0][0]);
            light.drawTrianglesArrays(1, 12);
        }
        //glfwSwapInterval(1);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

