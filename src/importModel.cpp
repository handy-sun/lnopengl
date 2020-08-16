#include <iostream>
#include <cstdlib>
#ifndef _MSC_VER
#include <libgen.h>
#include <unistd.h>
#endif // _MSC_VER

#include "glad.h"
#include "glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "package/shaderprogram.h"
#include "package/firstpersoncamera.hpp"
#include "package/modelloader.h"

const int screenWidth = 800;
const int screenHeight = 600;

const float vt = sqrt(3.0) / 3.0f;
const float side = sqrt(3.0) / 6.0f;
FirstPersonCamera ca(0.0f, 1.5f, 4.0f);

using namespace std;

int main(int argc, char **argv) // argv[1]:(char *) 模型加载路径 argv[2](int) 是否用线框模式加载
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
    stbi_set_flip_vertically_on_load(true);

    float fourspace[] = {
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

    glm::vec3 pointLightPositions[] = { glm::vec3(0.7f, 1.6f, 2.7f), glm::vec3(-0.6f, 1.5f, -3.0f) };
    bool ok;
    ShaderProgram nano, light;
    ModelLoader loader;
    nano.addShaderFile("../shaders/load.vert", "../shaders/load.frag");
    loader.setShaderProgram(&nano);
    if (argc < 2) {
        ok = loader.loadModel("../res/nanosuit_reflection/nanosuit.obj");
    } else {
        ok = loader.loadModel(argv[1]);
    }

    if (!ok) {
        return -1;
    }

    light.addShaderFile("../shaders/lightcube.vert", "../shaders/lightcube.frag");
    light.setVertexAttribute(0, 3, fourspace, sizeof(fourspace)); // 用四面体模拟光源

    float dir = 0.1f, po = 0.25f, spot = 0.8f;
    glm::mat4 model = glm::mat4(1.0f), view, projection, lightMvp;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.08f, 0.08f, 0.08f, 1.0f);

    if (argc >= 3 && 0 == strcmp(argv[2], "1"))
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    while (!glfwWindowShouldClose(window))
    {
        ca.ProcessWindowEvent(glfwGetTime());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        nano.use();
        view = ca.GetViewMatrix();
        projection = glm::perspective(glm::radians(ca.fovZoom()), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

        nano.setMat4("projection", &projection[0][0]);
        nano.setMat4("view", &view[0][0]);
        nano.setMat4("model", &model[0][0]);

        nano.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        nano.setVec3("dirLight.ambient", dir, dir, dir);
        nano.setVec3("dirLight.diffuse", dir, dir, dir);
        nano.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        nano.setVec3("pointLights[0].position", glm::value_ptr(pointLightPositions[0]));
        nano.setVec3("pointLights[0].ambient", po, po, po);
        nano.setVec3("pointLights[0].diffuse", po, po, po);
        nano.setVec3("pointLights[0].specular", po, po, po);
        nano.setFloat("pointLights[0].constant", 1.0f);
        nano.setFloat("pointLights[0].linear", 0.09f);
        nano.setFloat("pointLights[0].quadratic", 0.032f);
        nano.setVec3("pointLights[1].position", glm::value_ptr(pointLightPositions[1]));
        nano.setVec3("pointLights[1].ambient", po, po, po);
        nano.setVec3("pointLights[1].diffuse", po, po, po);
        nano.setVec3("pointLights[1].specular", po, po, po);
        nano.setFloat("pointLights[1].constant", 1.0f);
        nano.setFloat("pointLights[1].linear", 0.09f);
        nano.setFloat("pointLights[1].quadratic", 0.032f);

        nano.setVec3("spotLight.position", ca.cameraPos());
        nano.setVec3("spotLight.direction", ca.cameraFront());
        nano.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        nano.setVec3("spotLight.diffuse", spot, spot, spot);
        nano.setVec3("spotLight.specular", spot, spot, spot);
        nano.setFloat("spotLight.constant", 1.0f);
        nano.setFloat("spotLight.linear", 0.09f);
        nano.setFloat("spotLight.quadratic", 0.032f);
        nano.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        nano.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
  
        nano.drawModels();

        for (int i = 0; i < sizeof(pointLightPositions) / sizeof(glm::vec3); i++) {

            model = glm::translate(glm::mat4(1.0f), pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightMvp = projection * view * model;
            light.use();
            light.setMat4("modViewProj", &lightMvp[0][0]);
            light.drawTrianglesArrays(1, 12);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

