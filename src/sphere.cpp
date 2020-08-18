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

FirstPersonCamera ca(0.0f, 0.0f, 3.0f);

typedef glm::vec3 Vector3D;
typedef glm::vec2 Vector2D;

using namespace std;

#define USE_SPHER_CLASS

Vector3D getPosition(float u, float v, float r)
{   // u, v ∈ [0, 1]
    const float pi = glm::pi<float>();
    float y = r * std::cos(pi * u);
    float x = r * std::sin(pi * u) * std::cos(2 * pi * v);
    float z = r * std::sin(pi * u) * std::sin(2 * pi * v);

    return Vector3D(x, y, z);
}

void createSphereVertex(float *sphere, float radius, int Longitude, int Latitude)
{   // Longitude：经线切分个数  Latitude：纬线切分个数
    const float lon_step = 1.0f / Longitude;
    const float lat_step = 1.0f / Latitude;
    int offset = 0;
    Vector3D point1, point2, point3, point4;

    for (int lat = 0; lat < Latitude; ++lat) {  // 纬线 u
        for (int lon = 0; lon < Longitude; ++lon) { // 经线 v
            point1 = getPosition(lat * lat_step, lon * lon_step, radius);
            point2 = getPosition((lat + 1) * lat_step, lon * lon_step, radius);
            point3 = getPosition((lat + 1) * lat_step, (lon + 1) * lon_step, radius);
            point4 = getPosition(lat * lat_step, (lon + 1) * lon_step, radius);

            for (const auto &po : { point1, point4, point3, point1, point3, point2 }) {
                memcpy(sphere + offset, &po[0], 3 * sizeof(float));
                offset += 3;
            }
        }
    }
}

void createSphereTexCoord(float *tex, int Longitude, int Latitude)
{
    const float lon_step = 1.0f / Longitude;
    const float lat_step = 1.0f / Latitude;
    int offset = 0;
    Vector2D point1, point2, point3, point4;

    for (int lat = 0; lat < Latitude; ++lat) {  // 纬线 u
        for (int lon = 0; lon < Longitude; ++lon) { // 经线 v
            // point1 = Vector2D(lat * lat_step, lon * lon_step);
            // point2 = Vector2D((lat + 1) * lat_step, lon * lon_step);
            // point3 = Vector2D((lat + 1) * lat_step, (lon + 1) * lon_step);
            // point4 = Vector2D(lat * lat_step, (lon + 1) * lon_step);
            point1 = Vector2D(lon * lon_step, lat * lat_step);
            point4 = Vector2D(lon * lon_step, (lat + 1) * lat_step);
            
            point3 = Vector2D((lon + 1) * lon_step, (lat + 1) * lat_step);
            point2 = Vector2D((lon + 1) * lon_step, lat * lat_step);

            for (const auto &po : { point1, point4, point3, point1, point3, point2 }) {
                memcpy(tex + offset, &po[0], 2 * sizeof(float));
                offset += 2;
            }
        }
    }
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

    const char *vtc =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        ""
        "out vec2 TexCoord;\n"
        "out vec3 objColor;\n"
        "uniform mat4 mvp;\n"
        ""
        "void main()\n"
        "{\n"
        "    gl_Position = mvp * vec4(aPos, 1.0);\n"
        "    TexCoord = aTexCoord;\n"
        "    objColor = aPos;\n"
        "}\n"
    ;

    const char *frc =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        ""
        "in vec2 TexCoord;\n"
        "in vec3 objColor;\n"
        "uniform sampler2D sampTex;\n"
        ""
        "void main()\n"
        "{\n"
        "    FragColor = texture(sampTex, TexCoord);\n"
//        "    FragColor = vec4(objColor, 1.0f);\n"
        "}\n"
    ;

    const int lats = 30, lons = 60;
    float *vertices = new float[6 * 3 * lats * lons];
    float *texcoords = new float[6 * 2 * lats * lons];
    int width, height, nrChannels;
    unsigned char *data = stbi_load("../res/earth.jpg", &width, &height, &nrChannels, 0);

    createSphereVertex(vertices, 0.9f, lons, lats);
    createSphereTexCoord(texcoords, lons, lats);

    SphereArray sa(30, 60);
    cout << "verticeCount = " << sa.verticeCount() << " texCount = " << sa.texCoordCount() << endl;

    ShaderProgram sphere;
    sphere.addShaderSourceCode(vtc, frc);
    sphere.genVertexArray();

#ifdef USE_SPHER_CLASS
    sphere.setVertexAttribute(0, 3, sa.vertices(), sa.verticeCount() * sizeof(float));
    sphere.setVertexAttribute(1, 2, sa.texCoords(), sa.texCoordCount() * sizeof(float));
    sphere.setIndexArray(sa.indices(), sa.indiceCount() * sizeof(GLuint));
#else
    sphere.setVertexAttribute(0, 3, vertices, 6 * 3 * lats * lons * sizeof(float));
    sphere.setVertexAttribute(1, 2, texcoords, 6 * 2 * lats * lons * sizeof(float));
#endif
    sphere.genImageData(data, width, height, nrChannels);
    free(data);

    float scale = 1.0f;
    glm::mat4 model = glm::mat4(1.0f), view, projection, mvp;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.08f, 0.08f, 0.08f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        ca.ProcessWindowEvent(glfwGetTime());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sphere.use();
        view = ca.GetViewMatrix();
        projection = glm::perspective(glm::radians(ca.fovZoom()), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        // model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        if (ca.mouseButton() == GLFW_MOUSE_BUTTON_LEFT) {
            model = glm::rotate(model, glm::radians(ca.rotation()), Vector3D(1.0f, 0.0f, 0.0f));
        } else if (ca.mouseButton() == GLFW_MOUSE_BUTTON_RIGHT) {    
            model = glm::rotate(model, glm::radians(ca.rotation()), Vector3D(0.0f, 1.0f, 0.0f));
        }
        //model = glm::scale(model, glm::vec3(scale, scale, scale));
        mvp = projection * view * model;

        sphere.setMat4("mvp", &mvp[0][0]);
        sphere.setInt1("sampTex", 0);
#ifdef USE_SPHER_CLASS
        sphere.drawTrianglesElements(1, sa.indiceCount());
#else
        sphere.drawTrianglesArrays(1, 6 * lats * lons);
#endif
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    delete []vertices;
    delete []texcoords;
    return 0;
}





