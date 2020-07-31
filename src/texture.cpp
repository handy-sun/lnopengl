#include <iostream>
#ifndef _MSC_VER
#include <libgen.h>
#include <unistd.h>
#elif defined _DEBUG
#include <vld.h>
#endif // _MSC_VER
#include <cstdlib>

#include "package/GLManager.h"
#include "glad.h"
#include "glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const unsigned int screenWidth = 600;
const unsigned int screenHeight = 600;

float cameraSpeed = 0.05f; // adjust accordingly

bool firstMouse = true;
int buttonKey = -1;
bool isCtrlMod;
float yaw = -90.0f;  // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to
                     // the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = screenWidth / 2.0;  // 鼠标之前 x 坐标
float lastY = screenHeight / 2.0;   // 鼠标之前 y 坐标
float deltaTime = 0.0f;      // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
float fov = 45.0f;

float cubeRotation;

glm::vec3 cameraPos = glm::vec3(0.7f, 0.8f, 4.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 cubeModel(1.0f);

void framebuffer_size_callback(GLFWwindow *, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_LEFT_CONTROL && mods & GLFW_MOD_CONTROL)
    {
        isCtrlMod = true;
    }
    else
    {
        isCtrlMod = false;
    }
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    static float sensitivity = 0.05f;
    if (xpos > screenWidth || ypos > screenHeight)
    {
        firstMouse = true;
        cubeRotation = 0.0f;
        return;
    }
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }   

    double xOffset = xpos - lastX;
    double yOffset = lastY - ypos; // y坐标是从底部往顶部依次增大的
    lastX = xpos;
    lastY = ypos;

    if (isCtrlMod)
    {
        yaw += xOffset;
        pitch += yOffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        else if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }
    else
    {
        if (buttonKey >= 0)
        {
            if (buttonKey == GLFW_MOUSE_BUTTON_LEFT)
                cubeRotation = 0.2f * xOffset;
            else if (buttonKey == GLFW_MOUSE_BUTTON_RIGHT)
                cubeRotation = 0.2f * yOffset;
        }
        else
        {
            cubeRotation = 0.0f;
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        buttonKey = button;
    }
    else
    {
        buttonKey = -1;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    else if (fov > 45.0f)
        fov = 45.0f;
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
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

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
    GLManager cube, light;

    std::string pictures[] =
    {
        "llvm.png",
        "QQ2017.png",
        "block.png",
        "webq.png",
        "awesomeface.png",
        "compass_plate.png"
    };

    stbi_set_flip_vertically_on_load(true);
    for (int i = 0; i < 6; ++i)
    {
        std::string folders("../res/");
        folders.append(pictures[i].c_str());
        data = stbi_load(folders.c_str(), &width, &height, &nrChannels, 0);
        cube.genImageData(data, width, height, i, nrChannels);
        free(data);
        data = NULL;
    }

    cube.readShaderFile("../shaders/rectTexture.vert", "../shaders/rectTexture.frag");
    cube.setVertexArray(0, 3, vertices, sizeof(vertices));
    cube.setVertexArray(1, 3, colors, sizeof(colors));
    cube.setVertexArray(2, 2, texCoords, sizeof(texCoords));
    cube.setVertexArray(3, 3, normals, sizeof(normals));
    cube.setIndexArray(indices, sizeof(indices));

    light.readShaderFile("../shaders/lightcube.vert", "../shaders/lightcube.frag");
    light.setVertexArray(0, 3, vertices, sizeof(vertices));
    light.setIndexArray(indices, sizeof(indices));

    glm::mat4 view, projection, mvp, lightModel;
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        cameraSpeed = 3.5f * deltaTime;
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        projection = glm::perspective(glm::radians(fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        if (buttonKey >= 0)
        {
            cubeModel = glm::rotate(cubeModel, glm::radians(cubeRotation), buttonKey == 0 ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f));
        }
        mvp = projection * view * cubeModel;
        cube.use();
        glUniformMatrix4fv(glGetUniformLocation(cube.programId(), "modViewProj"), 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(cube.programId(), "model"), 1, GL_FALSE, &cubeModel[0][0]);
        glUniform3fv(glGetUniformLocation(cube.programId(), "lightColor"), 1, &lightColor[0]);
        glUniform3fv(glGetUniformLocation(cube.programId(), "lightPos"), 1, &lightPos[0]);
        glUniform3fv(glGetUniformLocation(cube.programId(), "viewPos"), 1, &cameraPos[0]);
        cube.paintTriangles(0, sizeof(indices) / sizeof(GLuint));

        lightModel = glm::translate(glm::mat4(1.0f), lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.2f));
        mvp = projection * view * lightModel;
        light.use();
        glUniformMatrix4fv(glGetUniformLocation(light.programId(), "modViewProj"), 1, GL_FALSE, &mvp[0][0]);
        light.paintTriangles(0, sizeof(indices) / sizeof(GLuint));

        //glfwSwapInterval(1);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

