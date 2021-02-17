#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include "shaders.h"
#include "camera.h"
#include <SOIL/SOIL.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <vector>


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

GLfloat lastX = 400, lastY = 300;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

bool firstMouse = true;
bool keys[1024];
bool blinn = false;
bool shpik = false;
bool check_screen = false;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024) {
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;
    }
    
    if (key == GLFW_KEY_B && action == GLFW_PRESS)
    {
        blinn = !blinn;
    }

    if (key == GLFW_KEY_N && action == GLFW_PRESS)
    {
        shpik = !shpik;
    }
    
}

void do_movement()
{
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

GLuint loadCubemap(std::vector<std::string> path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    int width_tex, height_tex;
    for (unsigned int i = 0; i < path.size(); i++)
    {
        unsigned char *img = SOIL_load_image(path[i].c_str(), &width_tex, &height_tex, 0, SOIL_LOAD_RGB);
        if(img)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width_tex, height_tex, 0, GL_RGB, GL_UNSIGNED_BYTE, img
            );
            SOIL_free_image_data(img);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << path[i] << std::endl;
            SOIL_free_image_data(img);
        }
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

GLuint loadTexture(std::string path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width_tex, height_tex;
    unsigned char* img = SOIL_load_image(path.c_str(), &width_tex, &height_tex, 0, SOIL_LOAD_RGB);
    if(img)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_tex, height_tex, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
        glGenerateMipmap(GL_TEXTURE_2D);

        SOIL_free_image_data(img);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        SOIL_free_image_data(img);
    }
    
    return textureID;
}



int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glewExperimental = GL_TRUE;
    glewInit();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    
    
    Shader ourShader("/Users/u0da/Desktop/final/bin/shaders/shader.vs", "/Users/u0da/Desktop/final/bin/shaders/shader.frag");
    
    Shader ourTexShader("/Users/u0da/Desktop/final/bin/shaders/shader_tex.vs", "/Users/u0da/Desktop/final/bin/shaders/shader_tex.frag");
    
    Shader lightShader("/Users/u0da/Desktop/final/bin/shaders/shader_light.vs", "/Users/u0da/Desktop/final/bin/shaders/shader_light.frag");
    
    Shader floorShader("/Users/u0da/Desktop/final/bin/shaders/floor_shader.vs", "/Users/u0da/Desktop/final/bin/shaders/floor_shader.frag");
    
    Shader normalShader("/Users/u0da/Desktop/final/bin/shaders/normal.vs", "/Users/u0da/Desktop/final/bin/shaders/normal.frag");
    
    Shader parallaxShader("/Users/u0da/Desktop/final/bin/shaders/parallax1.vs", "/Users/u0da/Desktop/final/bin/shaders/parallax1.frag");
    
    Shader hardShader("/Users/u0da/Desktop/final/bin/shaders/hard_light.vs", "/Users/u0da/Desktop/final/bin/shaders/hard_light.frag");
    
    Shader skyboxShader("/Users/u0da/Desktop/final/bin/shaders/skybox_shader.vs", "/Users/u0da/Desktop/final/bin/shaders/skybox_shader.frag");
    

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    
    float floorVertices[] = {
        // positions            // normals         // texcoords
         10.0f, -0.5f,  20.0f,  0.0f, 1.0f, 0.0f,  20.0f,  0.0f,
        -10.0f, -0.5f,  20.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -20.0f,  0.0f, 1.0f, 0.0f,   0.0f, 20.0f,

         10.0f, -0.5f,  20.0f,  0.0f, 1.0f, 0.0f,  20.0f,  0.0f,
        -10.0f, -0.5f, -20.0f,  0.0f, 1.0f, 0.0f,   0.0f, 20.0f,
         10.0f, -0.5f, -20.0f,  0.0f, 1.0f, 0.0f,  20.0f, 20.0f
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
    
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    
    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    
    GLuint floorVAO, floorVBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
    glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
    glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
    glm::vec3 pos4( 1.0f,  1.0f, 0.0f);

    glm::vec2 uv1(0.0f, 1.0f);
    glm::vec2 uv2(0.0f, 0.0f);
    glm::vec2 uv3(1.0f, 0.0f);
    glm::vec2 uv4(1.0f, 1.0f);
    
    glm::vec3 nm(0.0f, 0.0f, 1.0f);

    glm::vec3 tangent1, bitangent1;
    glm::vec3 tangent2, bitangent2;

    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;

    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);


    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);


    float normalVertices[] = {
        // positions            // normal         // texcoords  // tangent                          // bitangent
        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
    };

    GLuint normalVAO, normalVBO;
    glGenVertexArrays(1, &normalVAO);
    glGenBuffers(1, &normalVBO);
    glBindVertexArray(normalVAO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normalVertices), &normalVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    glBindVertexArray(0);
  
    std::vector<std::string> faces
    {
        "/Users/u0da/Desktop/final/bin/skybox/SaintPetersSquare3/posx.jpg",
        "/Users/u0da/Desktop/final/bin/skybox/SaintPetersSquare3/negx.jpg",
        "/Users/u0da/Desktop/final/bin/skybox/SaintPetersSquare3/posy.jpg",
        "/Users/u0da/Desktop/final/bin/skybox/SaintPetersSquare3/negy.jpg",
        "/Users/u0da/Desktop/final/bin/skybox/SaintPetersSquare3/posz.jpg",
        "/Users/u0da/Desktop/final/bin/skybox/SaintPetersSquare3/negz.jpg"
    };
    GLuint cubemapTexture = loadCubemap(faces);
    
    GLuint texture = loadTexture("/Users/u0da/Desktop/final/bin/textures/1.jpg");
    GLuint texture2 = loadTexture("/Users/u0da/Desktop/final/bin/textures/grass.jpg");
    GLuint texture3 = loadTexture("/Users/u0da/Desktop/final/bin/textures/cube.jpg");
    GLuint normtexture = loadTexture("/Users/u0da/Desktop/final/bin/textures/cube_n.jpg");
    GLuint heighttexture = loadTexture("/Users/u0da/Desktop/final/bin/textures/cube_h.jpg");
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    //glCullFace(GL_BACK);
    
    while(!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glfwPollEvents();
        do_movement();
        
        //glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        //glEnable(GL_DEPTH_TEST);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 model(1.0f);
        glm::mat4 view(1.0f);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width/(float)height, 0.1f, 100.0f);
        
        
        GLint modelLoc;
        GLint viewLoc;
        GLint projLoc;
        
        GLint objectColorLoc;
        GLint lightColorLoc;
        GLint lightPosLoc;
        
        glDepthMask(GL_FALSE);
        
        skyboxShader.Use();
        
            view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        
            modelLoc = glGetUniformLocation(skyboxShader.Program, "model");
            viewLoc = glGetUniformLocation(skyboxShader.Program, "view");
            projLoc = glGetUniformLocation(skyboxShader.Program, "projection");

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        glDepthMask(GL_TRUE);
    
        ourTexShader.Use();
        
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0, 0.005, 0.0));
            view = glm::mat4(1.0f);
        
            view = camera.GetViewMatrix();
            
            modelLoc = glGetUniformLocation(ourTexShader.Program, "model");
            viewLoc = glGetUniformLocation(ourTexShader.Program, "view");
            projLoc = glGetUniformLocation(ourTexShader.Program, "projection");

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
            objectColorLoc = glGetUniformLocation(ourTexShader.Program, "objectColor");
            lightColorLoc = glGetUniformLocation(ourTexShader.Program, "lightColor");
            lightPosLoc = glGetUniformLocation(ourTexShader.Program, "lightPos");
        
            glUniform3f(objectColorLoc, 0.5f, 0.5f, 0.5f);
            glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
            glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(ourTexShader.Program, "Texture"), 0);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
     
     
        GLint viewPosLoc;
        glm::vec3 lightPos2(0.5f, 1.0f, 0.3f);
    
        normalShader.Use();
            view = camera.GetViewMatrix();
            projLoc = glGetUniformLocation(normalShader.Program, "projection");
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            viewLoc = glGetUniformLocation(normalShader.Program, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-2.0, 0.5, -1.5));
            model = glm::scale(model, glm::vec3(0.5f));
            modelLoc = glGetUniformLocation(normalShader.Program, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            viewPosLoc = glGetUniformLocation(normalShader.Program, "viewPos");
            lightPosLoc = glGetUniformLocation(normalShader.Program, "lightPos");
            glUniform3f(lightPosLoc, lightPos2.x, lightPos2.y, lightPos2.z);
            glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
            
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glUniform1i(glGetUniformLocation(normalShader.Program, "diffuseMap"), 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normtexture);
        glUniform1i(glGetUniformLocation(normalShader.Program, "normalMap"), 2);
        glActiveTexture(0);
        
        glBindVertexArray(normalVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        
        parallaxShader.Use();
            view = camera.GetViewMatrix();
            projLoc = glGetUniformLocation(parallaxShader.Program, "projection");
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            viewLoc = glGetUniformLocation(parallaxShader.Program, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-1.0, 0.5, -1.5));
            model = glm::scale(model, glm::vec3(0.5f));
            modelLoc = glGetUniformLocation(parallaxShader.Program, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            viewPosLoc = glGetUniformLocation(parallaxShader.Program, "viewPos");
            lightPosLoc = glGetUniformLocation(parallaxShader.Program, "lightPos");
            glUniform3f(lightPosLoc, lightPos2.x, lightPos2.y, lightPos2.z);
            glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
            GLfloat heightScale = 0.1;
            glUniform1f(glGetUniformLocation(parallaxShader.Program, "heightScale"), heightScale);
        
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glUniform1i(glGetUniformLocation(parallaxShader.Program, "diffuseMap"), 3);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, normtexture);
        glUniform1i(glGetUniformLocation(parallaxShader.Program, "normalMap"), 4);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, heighttexture);
        glUniform1i(glGetUniformLocation(parallaxShader.Program, "depthMap"), 5);
        glActiveTexture(0);
        
        glBindVertexArray(normalVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        
        
        hardShader.Use();
            model = glm::mat4(1.0f);
            view = glm::mat4(1.0f);
            
            model = glm::translate(model, glm::vec3(3.0, 0.0, -2.0));
            view = camera.GetViewMatrix();
                
            modelLoc = glGetUniformLocation(hardShader.Program, "model");
            viewLoc = glGetUniformLocation(hardShader.Program, "view");
            projLoc = glGetUniformLocation(hardShader.Program, "projection");

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            
            objectColorLoc = glGetUniformLocation(hardShader.Program, "objectColor");
            lightPosLoc    = glGetUniformLocation(hardShader.Program, "lightPos");
            viewPosLoc    = glGetUniformLocation(hardShader.Program, "viewPos");
            
            glUniform3f(objectColorLoc, 0.5f, 0.7f, 0.5f);
            glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
    
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

         
        
        floorShader.Use();
            
                model = glm::mat4(1.0f);
                view = glm::mat4(1.0f);
            
                view = camera.GetViewMatrix();
                
                modelLoc = glGetUniformLocation(floorShader.Program, "model");
                viewLoc = glGetUniformLocation(floorShader.Program, "view");
                projLoc = glGetUniformLocation(floorShader.Program, "projection");

                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            
                objectColorLoc = glGetUniformLocation(floorShader.Program, "objectColor");
                lightColorLoc  = glGetUniformLocation(floorShader.Program, "lightColor");
                lightPosLoc    = glGetUniformLocation(floorShader.Program, "lightPos");
            
                glUniform3f(objectColorLoc, 0.5f, 0.5f, 0.5f);
                glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
                glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
                glUniform1i(glGetUniformLocation(floorShader.Program, "blinn"), blinn);
                glUniform1i(glGetUniformLocation(floorShader.Program, "shpik"), shpik);
        
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(glGetUniformLocation(floorShader.Program, "Texture"), 6);
        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
         
        
        lightShader.Use();
        
            model = glm::mat4(1.0f);
            view = glm::mat4(1.0f);
        
            model = glm::translate(model, lightPos);
            model = glm::scale(model, glm::vec3(0.05f));
            view = camera.GetViewMatrix();
            
            modelLoc = glGetUniformLocation(lightShader.Program, "model");
            viewLoc = glGetUniformLocation(lightShader.Program, "view");
            projLoc = glGetUniformLocation(lightShader.Program, "projection");

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        
        glfwSwapBuffers(window);
        
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteVertexArrays(1, &normalVAO);
    glDeleteBuffers(1, &VBO);
   
    
    glfwTerminate();
    return 0;
}
