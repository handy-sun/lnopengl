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
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
// #include "assimp/Importer.hpp"
// #include "assimp/scene.h"
// #include "assimp/postprocess.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "package/shaderprogram.h"
#include "package/firstpersoncamera.hpp"
// #include "package/modelloader.h"

const unsigned int screenWidth = 800;
const unsigned int screenHeight = 600;

FirstPersonCamera ca(0.0f, 1.5f, 3.0f);

unsigned int loadCubemap(const char * faces[])
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (int i = 0; i < 6; ++i) {
        unsigned char *data = stbi_load(faces[i], &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
        }
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
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
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
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

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *, int width, int height){ glViewport(0, 0, width, height); });
    glfwSetKeyCallback(window, [](GLFWwindow *_, int key, int scancode, int action, int mods){ ca.keyCallback(_, key, scancode, action, mods); });
    glfwSetCursorPosCallback(window, [](GLFWwindow *_,  double xpos, double ypos){ ca.cursorPosCallback(_, xpos, ypos); });
    glfwSetMouseButtonCallback(window, [](GLFWwindow *_, int button, int action, int mods){ ca.mouseButtonCallback(_, button, action, mods); });
    glfwSetScrollCallback(window, [](GLFWwindow *_,  double xpos, double ypos){ ca.scrollCallback(_, xpos, ypos); });
    stbi_set_flip_vertically_on_load(true);

    float quadVertices[] = {
        -0.05f,  0.05f,
        0.05f, -0.05f, 
        -0.05f, -0.05f,
        -0.05f,  0.05f,
        0.05f, -0.05f,  
        0.05f,  0.05f,                 
    };  

    float quadColors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,  
        0.0f, 1.0f, 1.0f   
    };

    const char *vtCode = {
        "#version 330 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "layout (location = 2) in vec2 aOffset;\n"
        "\n"
        "out vec3 fColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    fColor = aColor;\n"
        "    gl_Position = vec4(aPos * gl_InstanceID / 100.0 + aOffset, 0.0, 1.0);\n"
        "}\n"
    };

    const char *frCode = {
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "in vec3 fColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(fColor, 1.0);\n"
        "}\n"
    };

    float tran[200];
    int i = 0;
    const float offset = 0.1f;
    for (int y = -10; y < 10; y += 2)
    {
        for (int x = -10; x < 10; x += 2)
        {
            tran[i++] = (float)x / 10.0f + offset;
            tran[i++] = (float)y / 10.0f + offset;
        }
    }

    ShaderProgram quad;
    quad.addShaderSourceCode(vtCode, GL_VERTEX_SHADER);
    quad.addShaderSourceCode(frCode, GL_FRAGMENT_SHADER);
    // quad.addShaderFile("../shaders/geometryshader.geom", GL_GEOMETRY_SHADER);
    quad.linkShaderProgram();
    GLuint vao = quad.genVertexArray();
    quad.setVertexAttribute(0, 2, quadVertices, sizeof(quadVertices));
    quad.setVertexAttribute(1, 3, quadColors, sizeof(quadColors));
    quad.setVertexAttribute(2, 2, (float *)&tran[0], sizeof(tran));
    glVertexAttribDivisor(2, 1);

    glm::mat4 model, view, projection;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        ca.ProcessWindowEvent(glfwGetTime());
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        quad.use();
        glBindVertexArray(vao);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

