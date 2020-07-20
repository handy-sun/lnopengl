#include <iostream>
#include "package/GLManager.h"
#include "glad.h"
#include "glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

void framebuffer_size_callback(GLFWwindow *, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(int argc, char **argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(600, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    float vertices[] =
    {
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f
    };
    unsigned int indices[] =
    {
        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };
    float texCoords[] =
    {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    };
    for (int i = 0; i < 8; ++i)
    {
        texCoords[i] *= 2.0f;
    }

    GLManager gm;

    int width, height, nrChannels;
    unsigned char *data;
    stbi_set_flip_vertically_on_load(true);

    data = stbi_load("../shaders/container.jpg", &width, &height, &nrChannels, 0);
    gm.genImageData(data, width, height, 0);
    data = stbi_load("../shaders/llvm.png", &width, &height, &nrChannels, 0);
    gm.genImageData(data, width, height, 1);

    gm.readShaderFile("../shaders/rectTexture.vert", "../shaders/rectTexture.frag");
    gm.setVertexArray(vertices, sizeof(vertices), 0, 3);
    gm.genIndexArray(indices, sizeof(indices));
    gm.setTextureArray(texCoords, sizeof(texCoords), 1, 2);

//    glm::mat4 trans(1.0f);
//    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
//    trans = glm::rotate(trans, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//    trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float scaleFactor = sin(glfwGetTime()), ro = cos(glfwGetTime());

        glm::mat4 trans(1.0f);
        trans = glm::translate(trans, glm::vec3(scaleFactor, ro, 0.0f));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        trans = glm::scale(trans, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
        GLuint trLoc = glGetUniformLocation(gm.programId(), "transform");

        glUniformMatrix4fv(trLoc, 1, GL_FALSE, &trans[0][0]);

        gm.paintTriangles(0, sizeof(indices) / sizeof(GLuint));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    stbi_image_free(data);
    return 0;
}
