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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "package/shaderprogram.h"
#include "package/firstpersoncamera.hpp"
#include "package/spherearray.hpp"

const int screenWidth = 800;
const int screenHeight = 600;

const float vt = sqrt(3.0) / 3.0f;
const float side = sqrt(3.0) / 6.0f;
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
FirstPersonCamera ca(0.0f, 0.0f, 4.0f);

using namespace std;

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
    stbi_set_flip_vertically_on_load(true);
    if (argc >= 3 && 0 == strcmp(argv[2], "1"))
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("../res/earth.jpg", &width, &height, &nrChannels, 0);

    SphereArray sa(30, 60);

    ShaderProgram sphere, lightSphere;
    sphere.addShaderFile("../shaders/load.vert", "../shaders/load.frag");
    sphere.genVertexArray();

    sphere.setVertexAttribute(0, 3, sa.vertices(), sa.verticeCount() * sizeof(float));
    // sphere.setVertexAttribute(1, 3, sa.vertices(), sa.verticeCount() * sizeof(float));
    sphere.setVertexAttribute(2, 2, sa.texCoords(), sa.texCoordCount() * sizeof(float));
    sphere.setIndexArray(sa.indices(), sa.indiceCount() * sizeof(GLuint));

    sphere.genImageData(data, width, height, nrChannels);
    free(data);

    lightSphere.addShaderFile("../shaders/lightcube.vert", "../shaders/lightcube.frag");
    lightSphere.genVertexArray();
    // lightSphere.setVertexAttribute(0, 3, sa.vertices(), sa.verticeCount() * sizeof(float));
    // lightSphere.setIndexArray(sa.indices(), sa.indiceCount() * sizeof(GLuint));
    lightSphere.setVertexAttribute(0, 3, fourspace, sizeof(fourspace));

    float scale = 1.0f;
    float dir = 0.45f, po = 0.8f, spot = 0.95f;
    glm::vec3 pointLightPositions[] = { glm::vec3(0.7f, 0.9f, 1.5f), glm::vec3(-0.6f, 1.5f, -3.0f) };
    glm::mat4 model = glm::mat4(1.0f), view, projection, lightModel, lightMvp;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.08f, 0.08f, 0.08f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        ca.ProcessWindowEvent(glfwGetTime());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sphere.use();
        view = ca.GetViewMatrix();
        projection = glm::perspective(glm::radians(ca.fovZoom()), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

        model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        // model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        if (ca.mouseButton() == GLFW_MOUSE_BUTTON_LEFT) {
            model = glm::rotate(model, glm::radians(ca.rotation()), glm::vec3(1.0f, 0.0f, 0.0f));
        } else if (ca.mouseButton() == GLFW_MOUSE_BUTTON_RIGHT) {    
            model = glm::rotate(model, glm::radians(ca.rotation()), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        sphere.setInt1("texture_diffuse1", 0);

        sphere.setMat4("projection", &projection[0][0]);
        sphere.setMat4("view", &view[0][0]);
        sphere.setMat4("model", &model[0][0]);
        sphere.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        sphere.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        sphere.setVec3("dirLight.diffuse", dir, dir, dir);
        sphere.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        sphere.setVec3("pointLights[0].position", glm::value_ptr(pointLightPositions[0]));
        sphere.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        sphere.setVec3("pointLights[0].diffuse", po, po, po);
        sphere.setVec3("pointLights[0].specular", po, po, po);
        sphere.setFloat("pointLights[0].constant", 1.0f);
        sphere.setFloat("pointLights[0].linear", 0.09f);
        sphere.setFloat("pointLights[0].quadratic", 0.032f);
        sphere.setVec3("spotLight.position", ca.cameraPos());
        sphere.setVec3("spotLight.direction", ca.cameraFront());
        sphere.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        sphere.setVec3("spotLight.diffuse", spot, spot, spot);
        sphere.setVec3("spotLight.specular", spot, spot, spot);
        sphere.setFloat("spotLight.constant", 1.0f);
        sphere.setFloat("spotLight.linear", 0.09f);
        sphere.setFloat("spotLight.quadratic", 0.032f);
        sphere.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        sphere.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        sphere.drawTrianglesElements(1, sa.indiceCount());

        lightModel = glm::translate(glm::mat4(1.0f), pointLightPositions[0]);
        lightModel = glm::scale(lightModel, glm::vec3(0.2f));
        lightMvp = projection * view * lightModel;
        lightSphere.use();
        lightSphere.setMat4("modViewProj", &lightMvp[0][0]);
        lightSphere.drawTrianglesArrays(1, 12);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}





