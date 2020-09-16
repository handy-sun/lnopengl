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
#include "package/shaderprogram.h"
#include "package/firstpersoncamera.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "package/modelloader.h"
#include "package/spherearray.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const unsigned int screenWidth = 800;
const unsigned int screenHeight = 600;

FirstPersonCamera ca(0.0f, 0.0f, 3.0f);

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

    float vertices[] = { // 三角形环绕顺序为逆时针环绕
        -0.5f, -0.5f, 0.5f, // front
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,

        0.5f, -0.5f, -0.5f, // back
        -0.5f, -0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,

        -0.5f, -0.5f, -0.5f, // left
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,

        0.5f, -0.5f, 0.5f, // right
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,

        -0.5f, 0.5f, 0.5f, // top
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,

        0.5f, -0.5f, 0.5f, // bottom
        -0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,
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

    float texCoords[48] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };
    for (int i = 1; i < 6; ++i)
        memcpy(texCoords + 8 * i, texCoords, 8 * sizeof(float));

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

    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    const char *vtCode = {
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aNormal;\n"
        "\n"
        "out vec3 Normal;\n"
        "out vec3 Position;\n"
        "\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    Normal = mat3(transpose(inverse(model))) * aNormal;\n"
        "    Position = vec3(model * vec4(aPos, 1.0));\n"
        "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "}\n"
    };

    const char *frCode = {
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "in vec3 Normal;\n"
        "in vec3 Position;\n"
        "\n"
        "uniform vec3 cameraPos;\n"
        "uniform samplerCube skybox;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    float ratio = 1.00 / 1.52;\n"
        "    vec3 I = normalize(Position - cameraPos);\n"
        "    vec3 R = reflect(I, normalize(Normal));\n"
        // "    vec3 R = refract(I, normalize(Normal), ratio);\n"
        "    FragColor = vec4(texture(skybox, R).rgb, 1.0);\n"
        "}\n"
    };

    int width, height, nrChannels;
    unsigned char *data = NULL;
    const char *pics[] = { 
        (char *)"../res/skybox/right.jpg",
        (char *)"../res/skybox/left.jpg",
        (char *)"../res/skybox/top.jpg",
        (char *)"../res/skybox/bottom.jpg",
        (char *)"../res/skybox/front.jpg",
        (char *)"../res/skybox/back.jpg"
    };
    glm::mat4 view, projection, model(1.0f);
    ShaderProgram cube, nano, skyboxShader, sphere;
    SphereArray sa(30, 60);

    ModelLoader loader;
    nano.addShaderSourceCode(vtCode, frCode);
    loader.setShaderProgram(&nano);
    loader.loadModel("../res/nanosuit_reflection/nanosuit.obj");

    cube.addShaderSourceCode(vtCode, frCode);
    cube.genVertexArray();
    cube.setVertexAttribute(0, 3, vertices, sizeof(vertices));
    cube.setVertexAttribute(1, 3, normals, sizeof(normals));
    cube.setIndexArray(indices, sizeof(indices));
    data = stbi_load("../res/container.jpg", &width, &height, &nrChannels, 0);
    cube.genImageData(data, width, height, nrChannels);
    cube.setInt1("skybox", 0);

    sphere.addShaderSourceCode(vtCode, frCode);
    sphere.genVertexArray();
    sphere.setVertexAttribute(0, 3, sa.vertices(), sa.verticeCount() * sizeof(float));
    sphere.setVertexAttribute(1, 3, sa.vertices(), sa.verticeCount() * sizeof(float));
    // sphere.setVertexAttribute(2, 2, sa.texCoords(), sa.texCoordCount() * sizeof(float));
    sphere.setIndexArray(sa.indices(), sa.indiceCount() * sizeof(GLuint));
    sphere.setInt1("skybox", 0);

    stbi_set_flip_vertically_on_load(false);
    GLuint cubeMapVAO, cubeMapTexId;
    skyboxShader.addShaderFile("../shaders/skyBox.vert", "../shaders/skyBox.frag");
    cubeMapVAO = skyboxShader.genVertexArray();
    skyboxShader.use();
    cubeMapTexId = loadCubemap(pics);
    skyboxShader.setVertexAttribute(0, 3, skyboxVertices, sizeof(skyboxVertices));
    skyboxShader.setInt1("skybox", 0);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        ca.ProcessWindowEvent(glfwGetTime());
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        
        view = ca.GetViewMatrix();
        projection = glm::perspective(glm::radians(ca.fovZoom()), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        model = glm::mat4(1.0f);

        cube.use();
        cube.setMat4("projection", &projection[0][0]);
        cube.setMat4("view", &view[0][0]);
        cube.setMat4("model", &model[0][0]);
        cube.setVec3("cameraPos", ca.cameraPos());
        cube.drawTrianglesElements(1, 36);
        // change depth function so depth test passes when values are equal to depth buffer's content
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(3.3f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        sphere.use();
        sphere.setMat4("projection", &projection[0][0]);
        sphere.setMat4("view", &view[0][0]);
        sphere.setMat4("model", &model[0][0]);
        sphere.setVec3("cameraPos", ca.cameraPos());
        sphere.drawTrianglesElements(1, sa.indiceCount() * sizeof(GLuint));

        model = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, -1.5f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        nano.use();
        nano.setMat4("projection", &projection[0][0]);
        nano.setMat4("view", &view[0][0]);
        nano.setMat4("model", &model[0][0]);
        nano.setVec3("cameraPos", ca.cameraPos());
        nano.drawModels();

        glDepthFunc(GL_LEQUAL);  
        skyboxShader.use();
        view = glm::mat4(glm::mat3(ca.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", &view[0][0]);
        skyboxShader.setMat4("projection", &projection[0][0]);
        
        glBindVertexArray(cubeMapVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexId);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

