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

FirstPersonCamera ca(0.0f, 0.0f, 4.0f);

using namespace std;

void moveEarth(float delta, float *rotation, float *revolution, float *srotation)
{
	*rotation += 15.0 * delta;
	*revolution += 3.0 * delta;
    *srotation += 1.0 * delta;

	if (*rotation > 360.0)
		*rotation -= 360;

	if (*revolution > 360.0)
		*revolution -= 360;

    if (*srotation > 360.0)
		*srotation -= 360;
}

int main(int argc, char **argv)
{
#ifndef _MSC_VER
    chdir(dirname(argv[0]));
#endif
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "SunEarth", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    ca.setWindowSize(window, screenWidth, screenHeight);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *_, int width, int height){ glViewport(0, 0, width, height); ca.setWindowSize(_, width, height); });
    glfwSetKeyCallback(window, [](GLFWwindow *_, int key, int scancode, int action, int mods){ ca.keyCallback(_, key, scancode, action, mods); });
    glfwSetCursorPosCallback(window, [](GLFWwindow *_,  double xpos, double ypos){ ca.cursorPosCallback(_, xpos, ypos); });
    glfwSetMouseButtonCallback(window, [](GLFWwindow *_, int button, int action, int mods){ ca.mouseButtonCallback(_, button, action, mods); });
    glfwSetScrollCallback(window, [](GLFWwindow *_,  double xpos, double ypos){ ca.scrollCallback(_, xpos, ypos); });
    stbi_set_flip_vertically_on_load(true);
    if (argc >= 3 && 0 == strcmp(argv[2], "1"))
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    ShaderProgram sphere;
    SphereArray sa(30, 60);
    sphere.addShaderFile("../shaders/sphere.vert", "../shaders/sphere.frag");
    sphere.genVertexArray();
        
    sphere.setVertexAttribute(0, 3, sa.vertices(), sa.verticeCount() * sizeof(float));
    // sphere.setVertexAttribute(1, 3, sa.vertices(), sa.verticeCount() * sizeof(float));
    sphere.setVertexAttribute(2, 2, sa.texCoords(), sa.texCoordCount() * sizeof(float));
    sphere.setIndexArray(sa.indices(), sa.indiceCount() * sizeof(GLuint));

    int width, height, nrChannels;
    unsigned char *data;
    string pictures[] = { "sun.jpg", "earth.jpg" };

    for (int i = 0; i < sizeof(pictures) / sizeof(string); ++i) {
        data = stbi_load(("../res/" + pictures[i]).c_str(), &width, &height, &nrChannels, 0);
        sphere.genImageData(data, width, height, nrChannels);
        free(data);
    }

    float revolution = 0.0f; // 地球公转
    float rotation = 0.0f;   // 地球自转
    float sunRotation = 0.0f;// 太阳自转

    glm::mat4 model, view, projection, modelEarth;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.08f, 0.08f, 0.08f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        ca.ProcessWindowEvent(glfwGetTime());
        moveEarth(ca.deltaTime(), &rotation, &revolution, &sunRotation);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sphere.use();
        view = ca.GetViewMatrix();
        projection = glm::perspective(glm::radians(ca.fovZoom()), ca.aspect(), 0.1f, 100.0f);
        // sun
        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(sunRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
        sphere.setInt1("diffuseSun", 0);
        sphere.setInt1("index", 0);
        sphere.setMat4("projection", &projection[0][0]);
        sphere.setMat4("view", &view[0][0]);
        sphere.setMat4("model", &model[0][0]);
        
        sphere.drawElementWithOneTex(sa.indiceCount(), 0);

        // earth
        modelEarth = glm::mat4(1.0f);
        modelEarth = glm::rotate(modelEarth, glm::radians(revolution), glm::vec3(0.0f, 1.0f, 0.0f)); // 平移之后再公转
        modelEarth = glm::translate(modelEarth, glm::vec3(0.0f, 0.0f, -3.0f));
        modelEarth = glm::rotate(modelEarth, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));// 先自转
        modelEarth = glm::rotate(modelEarth, glm::radians(23.43f), glm::vec3(0.0f, 0.0f, 1.0f)); // 黄赤交角。
        modelEarth = glm::rotate(modelEarth, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // 默认球坐标南北极是反的，转到正确位置
        modelEarth = glm::scale(modelEarth, glm::vec3(0.2f, 0.2f, 0.2f));

        sphere.setInt1("diffuseEarth", 1);
        sphere.setInt1("index", 1);
        sphere.setMat4("model", &modelEarth[0][0]);

        sphere.drawElementWithOneTex(sa.indiceCount(), 1);  

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

