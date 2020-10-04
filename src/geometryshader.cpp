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
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "package/shaderprogram.h"
#include "package/firstpersoncamera.hpp"
#include "package/modelloader.h"

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

    const char *vtCode = {
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 2) in vec2 aTexCoords;\n"
        "\n"
        "out vec2 TexCoords;\n"
        "\n"
        "uniform mat4 projection;\n"
        "uniform mat4 view;\n"
        "uniform mat4 model;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    TexCoords = aTexCoords;\n"
        "    gl_Position = projection * view * model * vec4(aPos, 1.0); \n"
        "}\n"
    };

    const char *frCode = {
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "in vec2 TexCoords;\n"
        "\n"
        "uniform sampler2D texture_diffuse1;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor = texture(texture_diffuse1, TexCoords);\n"
        "}\n"
    };

////////////////////////////////////////////////////////
    const char *vtNor = {
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aNormal;\n"
        "\n"
        "out VS_OUT {\n"
        "    vec3 normal;\n"
        "} vs_out;\n"
        "\n"
        "uniform mat4 view;\n"
        "uniform mat4 model;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    mat3 normalMatrix = mat3(transpose(inverse(view * model)));\n"
        "    vs_out.normal = vec3(vec4(normalMatrix * aNormal, 0.0));\n"
        "    gl_Position = view * model * vec4(aPos, 1.0); \n"
        "}\n"
    };

    const char *frNor = {
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0, 1.0, 0.0, 1.0);\n"
        "}\n"
    };

    const char *geoNor = {
        "#version 330 core\n"
        "layout (triangles) in;\n"
        "layout (line_strip, max_vertices = 6) out;\n"
        "\n"
        "in VS_OUT {\n"
        "    vec3 normal;\n"
        "} gs_in[];\n"
        "\n"
        "const float MAGNITUDE = 0.008;\n"
        "\n"
        "uniform mat4 projection;\n"
        "\n"
        "void GenerateLine(int index)\n"
        "{\n"
        "    gl_Position = projection * gl_in[index].gl_Position;\n"
        "    EmitVertex();\n"
        "    gl_Position = projection * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);\n"
        "    EmitVertex();\n"
        "    EndPrimitive();\n"
        "}\n"
        "\n"
        "void main()\n"
        "{\n"
        "    GenerateLine(0);\n"
        "    GenerateLine(1);\n"
        "    GenerateLine(2);\n"
        "}\n"
    };

    ModelLoader loader, norLoader;

    ShaderProgram gshader, norShader;
    gshader.addShaderSourceCode(vtCode, GL_VERTEX_SHADER);
    gshader.addShaderSourceCode(frCode, GL_FRAGMENT_SHADER);
    // gshader.addShaderFile("../shaders/geometryshader.geom", GL_GEOMETRY_SHADER);
    gshader.linkShaderProgram();
    loader.setShaderProgram(&gshader);
    loader.loadModel("../res/nanosuit_reflection/nanosuit.obj");

    norShader.addShaderSourceCode(vtNor, GL_VERTEX_SHADER);
    norShader.addShaderSourceCode(frNor, GL_FRAGMENT_SHADER);
    norShader.addShaderSourceCode(geoNor, GL_GEOMETRY_SHADER);
    norShader.linkShaderProgram();
    norLoader.setShaderProgram(&norShader);
    norLoader.loadModel("../res/nanosuit_reflection/nanosuit.obj");

    glm::mat4 model, view, projection;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        ca.ProcessWindowEvent(glfwGetTime());
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        gshader.use();
        view = ca.GetViewMatrix();
        projection = glm::perspective(glm::radians(ca.fovZoom()), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        
        gshader.setMat4("projection", &projection[0][0]);
        gshader.setMat4("view", &view[0][0]);
        gshader.setMat4("model", &model[0][0]);
        gshader.drawModels();

        norShader.use();
        norShader.setMat4("projection", &projection[0][0]);
        norShader.setMat4("view", &view[0][0]);
        norShader.setMat4("model", &model[0][0]);
        norShader.drawModels();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

