#include <iostream>
#ifndef _MSC_VER
#include <libgen.h>
#include <unistd.h>
#elif defined _DEBUG
#include <vld.h>
#endif // _MSC_VER
#include <cstdlib>
#include <map>

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

FirstPersonCamera ca(0.0f, 0.0f, 3.0f);

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

    float vertices[] = {
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

    unsigned int indices[] = {
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

    float texCoords[] = {
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

    float normals[] = {
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

    float floorVertex[] = {
         5.0f, -0.5f,  5.0f,
        -5.0f, -0.5f,  5.0f,
        -5.0f, -0.5f, -5.0f,
         5.0f, -0.5f,  5.0f,
        -5.0f, -0.5f, -5.0f,
         5.0f, -0.5f, -5.0f,
    };

    float floorTexCoords[] = {
        2.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 2.0f,
        2.0f, 0.0f,
        0.0f, 2.0f,
        2.0f, 2.0f	
    };

    float tranVertices[] = {
        0.0f,  0.5f,  0.0f,
        0.0f, -0.5f,  0.0f,
        1.0f, -0.5f,  0.0f,
        1.0f,  0.5f,  0.0f,
    };

    float tranTex[] = {
        0.0f,  0.0f,
        0.0f,  1.0f,
        1.0f,  1.0f,
        1.0f,  0.0f
    };

    unsigned int tranIndices[] = { 0, 1, 2, 0, 2, 3 };

    float quadVertices[] = { 
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
    };

    float quadTex[] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f
    };

    const char *vtCode = {
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoords;\n"
        "\n"
        "out vec2 TexCoords;\n"
        "\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    TexCoords = aTexCoords;\n"
        "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "}\n"
    };

    const char *frCode = {
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "in vec2 TexCoords;\n"
        "\n"
        "uniform sampler2D sampDiffuse;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor = texture(sampDiffuse, TexCoords);\n"
        // "    float depth = 20.0 / (100.1 - (gl_FragCoord.z * 2.0 - 1.0) * 99.9);\n"
        // "    FragColor = vec4(vec3(depth / 100.0), 1.0);\n"
        "}\n"
    };

    const char *frSingleColorCode = {
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(0.04, 0.28, 0.26, 1.0);\n"
        "}\n"
    };

    int width, height, nrChannels;
    unsigned char *data = NULL;
    ShaderProgram cube, floor, singleColor, tranwindow, quad;

    cube.addShaderSourceCode(vtCode, frCode);
    cube.genVertexArray();
    cube.setVertexAttribute(0, 3, vertices, sizeof(vertices));
    cube.setVertexAttribute(1, 2, texCoords, sizeof(texCoords));
    cube.setIndexArray(indices, sizeof(indices));
    data = stbi_load("../res/container.jpg", &width, &height, &nrChannels, 0);
    cube.genImageData(data, width, height, nrChannels);

    tranwindow.addShaderSourceCode(vtCode, frCode);
    tranwindow.genVertexArray();
    tranwindow.setVertexAttribute(0, 3, tranVertices, sizeof(tranVertices));
    tranwindow.setVertexAttribute(1, 2, tranTex, sizeof(tranTex));
    tranwindow.setIndexArray(tranIndices, sizeof(tranIndices));
    data = stbi_load("../res/blending_transparent_window.png", &width, &height, &nrChannels, 0);
    tranwindow.genImageData(data, width, height, nrChannels);
    free(data);

    floor.addShaderSourceCode(vtCode, frCode);
    floor.genVertexArray();
    floor.setVertexAttribute(0, 3, floorVertex, sizeof(floorVertex));
    floor.setVertexAttribute(1, 2, floorTexCoords, sizeof(floorTexCoords));
    data = stbi_load("../res/brickwall.jpg", &width, &height, &nrChannels, 0);
    floor.genImageData(data, width, height, nrChannels);
    free(data);

    singleColor.addShaderSourceCode(vtCode, frSingleColorCode);
    singleColor.genVertexArray();
    singleColor.setVertexAttribute(0, 3, vertices, sizeof(vertices));
    singleColor.setIndexArray(indices, sizeof(indices));

    quad.addShaderFile("../shaders/framebuffer.vert", "../shaders/framebuffer.frag");
    quad.genVertexArray();
    quad.setVertexAttribute(0, 3, quadVertices, sizeof(quadVertices));
    quad.setVertexAttribute(1, 2, quadTex, sizeof(quadTex));
    quad.use();
    quad.setInt1("screenTexture", 0);
    unsigned int fbo = quad.genFrameBuffer();
    quad.genRenderBuffer(screenWidth, screenHeight);

    float scale = 1.05f;
    glm::mat4 view, projection, model(1.0f);
    std::map<float, glm::vec3> mapSorted;

    glm::vec3 cubePositions[] = {
        glm::vec3(-1.0f, 0.0f, -1.0f),
        glm::vec3(2.0f, 0.0f, 0.0f),
    };

    glm::vec3 windows[] = {
        glm::vec3(-1.5f, 0.0f, -0.48f),
        glm::vec3( 1.5f, 0.0f, 0.51f),
        glm::vec3( 0.0f, 0.0f, 0.7f),
        glm::vec3(-0.3f, 0.0f, -2.3f),
        glm::vec3( 0.5f, 0.0f, -0.6f)
    };

    for (int i = 0; i < sizeof(windows) / sizeof(glm::vec3); i++) {
        float distance = glm::length(ca.position() - windows[i]);
        mapSorted[distance] = windows[i];
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    cube.setInt1("sampDiffuse", 0);
    floor.setInt1("sampDiffuse", 0);
    tranwindow.setInt1("sampDiffuse", 0);
    
    while (!glfwWindowShouldClose(window)) {
        ca.ProcessWindowEvent(glfwGetTime());
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT /* | GL_STENCIL_BUFFER_BIT */);
        
        view = ca.GetViewMatrix();
        projection = glm::perspective(glm::radians(ca.fovZoom()), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        
        glStencilFunc(GL_ALWAYS, 1, 0xFF); 
        glStencilMask(0xFF); 
        cube.use();
        cube.setMat4("projection", &projection[0][0]);
        cube.setMat4("view", &view[0][0]);

        for (int i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); i++) {
            model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
            cube.setMat4("model", &model[0][0]);
            cube.drawTrianglesElements(1, 36);
        }

        model = glm::mat4(1.0f);
        glStencilMask(0x00); // 记得保证我们在绘制地板的时候不会更新模板缓冲
        floor.use();
        floor.setMat4("projection", &projection[0][0]);
        floor.setMat4("view", &view[0][0]);
        floor.setMat4("model", &model[0][0]);
        floor.drawTrianglesArrays(1, 6);
#if 1
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00); 
        glDisable(GL_DEPTH_TEST);

        singleColor.use();
        singleColor.setMat4("projection", &projection[0][0]);
        singleColor.setMat4("view", &view[0][0]);
        for (int i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); i++) {
            model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
            model = glm::scale(model, glm::vec3(scale, scale, scale));
            singleColor.setMat4("model", &model[0][0]);
            singleColor.drawTrianglesElements(1, 36);
        }
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);

        tranwindow.use();
        tranwindow.setMat4("projection", &projection[0][0]);
        tranwindow.setMat4("view", &view[0][0]);

        for (auto it = mapSorted.crbegin(); it != mapSorted.crend(); ++it) {
            model = glm::translate(glm::mat4(1.0f), it->second);              
            tranwindow.setMat4("model", &model[0][0]);
            tranwindow.drawTrianglesElements(1, 6);
        }
#endif
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); 
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        quad.use();
        quad.drawTrianglesArrays(1, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

