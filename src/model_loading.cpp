#include <iostream>
#ifndef _MSC_VER
#include <libgen.h>
#include <unistd.h>
#endif
#include "glad.h"
#include "glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "package/shader.h"
#include "package/model.h"
#include "package/firstpersoncamera.hpp"

const int screenWidth = 800;
const int screenHeight = 600;

FirstPersonCamera ca(0.0f, 1.5f, 4.0f);

int main(int argc, char **argv)
{
#ifndef _MSC_VER
    chdir(dirname(argv[0]));
#endif
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    ca.setWindowSize(window, screenWidth, screenHeight);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *, int width, int height){ glViewport(0, 0, width, height); });
    glfwSetKeyCallback(window, [](GLFWwindow *_, int key, int scancode, int action, int mods){ ca.keyCallback(_, key, scancode, action, mods); });
    glfwSetCursorPosCallback(window, [](GLFWwindow *_,  double xpos, double ypos){ ca.cursorPosCallback(_, xpos, ypos); });
    glfwSetMouseButtonCallback(window, [](GLFWwindow *_, int button, int action, int mods){ ca.mouseButtonCallback(_, button, action, mods); });
    glfwSetScrollCallback(window, [](GLFWwindow *_,  double xpos, double ypos){ ca.scrollCallback(_, xpos, ypos); });

    stbi_set_flip_vertically_on_load(true);

    const char *vtc = 
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aNormal;\n"
        "layout (location = 2) in vec2 aTexCoords;\n"
        ""
        "out vec2 TexCoords;\n"
        ""
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        ""
        "void main()\n"
        "{\n"
        "    TexCoords = aTexCoords;\n"
        "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "}\n\0"
    ;

    const char *frc = 
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        ""
        "in vec2 TexCoords;\n"
        ""
        "uniform sampler2D texture_diffuse1;\n"
        ""
        "void main()\n"
        "{\n"
        "    FragColor = texture(texture_diffuse1, TexCoords);\n"
        "}\n\0"
    ;
    Shader ourShader(vtc, frc, true);

    //Model ourModel("../res/X/dwarf.x");
    Model ourModel("../res/nanosuit_reflection/nanosuit.obj");

    glm::mat4 model = glm::mat4(1.0f), view, projection;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    while (!glfwWindowShouldClose(window))
    {
        ca.ProcessWindowEvent(glfwGetTime());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        view = ca.GetViewMatrix();
        projection = glm::perspective(glm::radians(ca.fovZoom()), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

