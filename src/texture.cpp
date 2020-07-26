#include <iostream>
#ifndef _MSC_VER
#include <libgen.h>
#include <unistd.h>
#endif

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
bool isPress = false;
float yaw = -90.0f;  // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to
                     // the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = screenWidth / 2.0;  // 鼠标之前 x 坐标
float lastY = screenHeight / 2.0;   // 鼠标之前 y 坐标
float deltaTime = 0.0f;      // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
float fov = 45.0f;

float objRotation;
glm::vec3 rotationAxis = glm::vec3(1.0f, 1.0f, 0.0f);

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 model(1.0f);

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
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_RIGHT)
        {
            //std::cout << "right" << std::endl;
        }
    }
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    static float sensitivity = 0.05f;
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // y坐标是从底部往顶部依次增大的
    lastX = xpos;
    lastY = ypos;

    if (isPress && xoffset > 3 && yoffset > 3)
    {
        double dis = sqrt(xoffset * xoffset + yoffset * yoffset);
        objRotation += sensitivity * dis;
        if (objRotation > 360.0f)
            objRotation -= 360.0f;
        else if (objRotation < 0.0f)
            objRotation += 360.0f;

        //double angle = asin(yoffset / dis) * M_PI / 180.0f;
        rotationAxis.x += yoffset / dis;
        rotationAxis.y += xoffset / dis;
        std::cout << objRotation << "," << rotationAxis.x << "," << rotationAxis.y << std::endl;
    }

//    xoffset *= sensitivity;
//    yoffset *= sensitivity;

//    yaw += xoffset;
//    pitch += yoffset;

//    if (pitch > 89.0f)
//        pitch = 89.0f;
//    else if (pitch < -89.0f)
//        pitch = -89.0f;

//    glm::vec3 front;
//    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//    front.y = sin(glm::radians(pitch));
//    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//    cameraFront = glm::normalize(front);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        isPress = true;
    }
    else
    {
        isPress = false;
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
//    glfwSetScrollCallback(window, scroll_callback);

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

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
//        glm::vec3( 2.0f,  5.0f, -15.0f),
//        glm::vec3(-1.5f, -2.2f, -2.5f),
//        glm::vec3(-3.8f, -2.0f, -12.3f),
//        glm::vec3( 2.4f, -0.4f, -3.5f),
//        glm::vec3(-1.7f,  3.0f, -7.5f),
//        glm::vec3( 1.3f, -2.0f, -2.5f),
//        glm::vec3( 1.5f,  2.0f, -2.5f),
//        glm::vec3( 1.5f,  0.2f, -1.5f),
//        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    GLManager gm;

    int width, height, nrChannels;
    unsigned char *data = NULL;
    float radius = 15.0f;
    stbi_set_flip_vertically_on_load(true);

    data = stbi_load("../shaders/llvm.png", &width, &height, &nrChannels, 0);
    gm.genImageData(data, width, height, 0);
    data = stbi_load("../shaders/awesomeface.png", &width, &height, &nrChannels, 0);
    gm.genImageData(data, width, height, 1);

    gm.readShaderFile("../shaders/rectTexture.vert", "../shaders/rectTexture.frag");
    gm.setVertexArray(0, 3, vertices, sizeof(vertices));
    gm.setVertexArray(1, 2, texCoords, sizeof(texCoords));
    gm.setIndexArray(indices, sizeof(indices));

    glm::mat4 view, projection, mvp;
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe Mode

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        cameraSpeed = 3.5f * deltaTime;
        //processInput(window);

        glEnable(GL_DEPTH_TEST);
//        glEnable(GL_TEXTURE_2D);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        //view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        projection = glm::perspective(glm::radians(fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        //projection = glm::ortho(0.0f, 800.0f, 0.0f, (float)screenHeight, 0.1f, 100.0f);

        GLuint mvpLoc = glGetUniformLocation(gm.programId(), "modViewProj");
        for (int i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); ++i)
        {
            //model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
            float _angle = 15.0f * (i + 1);
            if (isPress)
            model = glm::rotate(model, glm::radians(objRotation), rotationAxis/*glm::vec3(0.0f, 1.0f, 0.0f)*/);
            mvp = projection * view * model;

            glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
            gm.paintTriangles(0, sizeof(indices) / sizeof(GLuint));
        }

        glfwSwapInterval(1);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    stbi_image_free(data);
    return 0;
}

