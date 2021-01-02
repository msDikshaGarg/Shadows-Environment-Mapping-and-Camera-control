#define STB_IMAGE_IMPLEMENTATION

#include "Helpers.h"
#include <stb_image.h>
#include <filesystem.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <vector>
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#else
#include <GLFW/glfw3.h>
#endif
#include <chrono>
#include <glm/vec4.hpp>
#include <sstream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

using namespace std;

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        float xoffset, yoffset;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        
        xoffset = Position.x;
        yoffset = Position.y;
        
        Yaw   += xoffset;
        Pitch += yoffset; 
        updateCameraVectors();
    }

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }
        updateCameraVectors();
    }

    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f; 
    }

private:
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp)); 
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const * path);
void renderScene(const Program &prgm);
void renderCube();
unsigned int loadCubemap(vector<std::string> faces);

const unsigned int SCR_WIDTH = 640;
const unsigned int SCR_HEIGHT = 480;
int width, height;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
int state=0;

unsigned int planeVAO;

int main()
{
    GLFWwindow* window;
    cout << "Executing main successfully............" << endl;
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Diksha's Assignment 4", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
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

    glewExperimental = GL_TRUE;
    glewInit();

    Program program, program_depth, program_quad, program_env;

    const GLchar* vertex_shader =
            "#version 150 core\n"
                    "in vec3 aPos;"
                    "in vec3 aNormal;"
                    "in vec2 aTexCoords;"
                    "out vec2 TexCoords;"
                    "out VS_OUT {"
                    "    vec3 FragPos;"
                    "    vec3 Normal;"
                    "    vec2 TexCoords;"
                    "    vec4 FragPosLightSpace;"
                    "} vs_out;"
                    "uniform mat4 projection;"
                    "uniform mat4 view;"
                    "uniform mat4 model;"
                    "uniform mat4 lightSpaceMatrix;"

                    
                    "void main()"
                    "{"
                    "    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));"
                    "    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;"
                    "    vs_out.TexCoords = aTexCoords;"
                    "    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);"
                    "    gl_Position = projection * view * model * vec4(aPos, 1.0);"
                    "}";

    const GLchar* fragment_shader =
            "#version 150 core\n"
                    "out vec4 FragColor;"
                    "in VS_OUT {"
                    "    vec3 FragPos;"
                    "    vec3 Normal;"
                    "    vec2 TexCoords;"
                    "    vec4 FragPosLightSpace;"
                    "} fs_in;"
                    "uniform sampler2D diffuseTexture;"
                    "uniform sampler2D shadowMap;"
                    "uniform vec3 lightPos;"
                    "uniform vec3 viewPos;"

                    "float ShadowCalculation(vec4 fragPosLightSpace)"
                    "{"
                    "    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;"
                    "    projCoords = projCoords * 0.5 + 0.5;"
                    "    float closestDepth = texture(shadowMap, projCoords.xy).r; "
                    "    float currentDepth = projCoords.z;"
                    "    vec3 normal = normalize(fs_in.Normal);"
                    "    vec3 lightDir = normalize(lightPos - fs_in.FragPos);"
                    "    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);"
                    "    float shadow = 0.0;"
                    "    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);"
                    "    for(int x = -1; x <= 1; ++x)"
                    "    {"
                    "        for(int y = -1; y <= 1; ++y)"
                    "        {"
                    "            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; "
                    "            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        "
                    "        }    "
                    "    }"
                    "    shadow /= 9.0;"
                    "    if(projCoords.z > 1.0)"
                    "        shadow = 0.0;"
                    "    return shadow;"
                    "}"

                    "void main()"
                    "{"
                    "    vec3 color = vec3(0.8f, 0.8f, 0.8f);"
                    "    vec3 normal = normalize(fs_in.Normal);"
                    "    vec3 lightColor = vec3(0.3);"
                    "    vec3 ambient = 0.3 * color;"
                    "    vec3 lightDir = normalize(lightPos - fs_in.FragPos);"
                    "    float diff = max(dot(lightDir, normal), 0.0);"
                    "    vec3 diffuse = diff * lightColor;"
                    "    vec3 viewDir = normalize(viewPos - fs_in.FragPos);"
                    "    vec3 reflectDir = reflect(-lightDir, normal);"
                    "    float spec = 0.0;"
                    "    vec3 halfwayDir = normalize(lightDir + viewDir);  "
                    "    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);"
                    "    vec3 specular = spec * lightColor;    "
                    "    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);"
                    "    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    "
                    "    FragColor = vec4(lighting, 1.0);"
                    "}";

    const GLchar* depth_vertex_shader = 
            "#version 150 core\n"
                    "in vec3 aPos;"
                    "uniform mat4 lightSpaceMatrix;"
                    "uniform mat4 model;"

                    "void main()"
                    "{"
                    "    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);"
                    "}";

    const GLchar* depth_fragment_shader = 
            "#version 150 core\n"
                    "void main()"
                    "{"
                    "}";

    const GLchar* quad_vertex_shader = 
            "#version 150 core\n"
                    "in vec3 aPos;"
                    "in vec2 aTexCoords;"
                    "out vec2 TexCoords;"

                    "void main()"
                    "{"
                    "    TexCoords = aTexCoords;"
                    "    gl_Position = vec4(aPos, 1.0);"
                    "}";

    const GLchar* quad_fragment_shader = 
            "#version 150 core\n"
                    "out vec4 FragColor;"
                    "in vec2 TexCoords;"
                    "uniform sampler2D depthMap;"
                    "uniform float near_plane;"
                    "uniform float far_plane;"
                    // "float near_plane = 1.0f;"
                    // "float far_plane = 7.5f;"

                    "float LinearizeDepth(float depth)"
                    "{"
                    "    float z = depth * 2.0 - 1.0;"
                    "    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));"
                    "}"

                    "void main()"
                    "{"
                    "     float depthValue = texture(depthMap, TexCoords).r;"
                    "     FragColor = vec4(vec3(depthValue), 1.0);"
                    "}";

    const GLchar* env_vertex_shader =
            "#version 150 core\n"
                    "in vec3 aPos;"
                    "uniform mat4 projection;"
                    "uniform mat4 view;"
                    "out vec3 TexCoords;"

                    
                    "void main()"
                    "{"
                    "   TexCoords = aPos;"
                    "    vec4 pos = projection * view * vec4(aPos, 1.0);"
                    "    gl_Position = pos.xyww;"
                    "}";

    const GLchar* env_fragment_shader =
            "#version 150 core\n"
                    "in vec3 TexCoords;"
                    "out vec4 FragColor;"
                    "uniform samplerCube skybox;"
                    "void main()"
                    "{"
                    "    FragColor = texture(skybox, TexCoords);"
                    "}";

    program.init(vertex_shader, fragment_shader, "FragColor");
    program.bind();

    program_depth.init(depth_vertex_shader, depth_fragment_shader, "");
    program_depth.bind();

    program_quad.init(quad_vertex_shader, quad_fragment_shader, "FragColor");
    program_quad.bind();

    program_env.init(env_vertex_shader, env_fragment_shader, "FragColor");
    program_env.bind();


    float planeVertices[] = {
         1.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
        -1.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -1.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,

         1.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
        -1.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
         1.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    vector<std::string> faces
    {
        FileSystem::getPath("/data/night_posx.png"),
        FileSystem::getPath("/data/night_negx.png"),
        FileSystem::getPath("/data/night_posy.png"),
        FileSystem::getPath("/data/night_negy.png"),
        FileSystem::getPath("/data/night_posz.png"),
        FileSystem::getPath("/data/night_negz.png"),
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    const unsigned int SHADOW_WIDTH = 640, SHADOW_HEIGHT = 480;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glEnable(GL_DEPTH_TEST);

    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f); 

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwGetWindowSize(window, &width, &height);
        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 7.5f;

        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        program_depth.bind();
        glUniformMatrix4fv(program_depth.uniform("lightSpaceMatrix"),1,GL_FALSE,&lightSpaceMatrix[0][0]);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        renderScene(program_depth);  // need to check how to render
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        program.bind();
        glUniformMatrix4fv(program.uniform("view"),1,GL_FALSE,&view[0][0]);
        glUniformMatrix4fv(program.uniform("projection"),1,GL_FALSE,&projection[0][0]);

        glUniform3f(program.uniform("viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        glUniform3f(program.uniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniformMatrix4fv(program.uniform("lightSpaceMatrix"),1,GL_FALSE,&lightSpaceMatrix[0][0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderScene(program); 

        program_quad.bind();
        
        GLint plane_loc = program_quad.uniform("near_plane");
        GLint far_plane_loc = program_quad.uniform("far_plane");

        if(plane_loc != -1)
            glUniform1f(plane_loc, near_plane);
        if(far_plane_loc != -1)
            glUniform1f(far_plane_loc, far_plane);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        // environment mapping
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 env_view = camera.GetViewMatrix();
        glm::mat4 env_projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        
        glDepthFunc(GL_LEQUAL);  
        env_view = glm::mat4(glm::mat3(camera.GetViewMatrix()));         
        program_env.bind();
        glUniformMatrix4fv(program_env.uniform("view"),1,GL_FALSE,&env_view[0][0]);
        glUniformMatrix4fv(program_env.uniform("projection"),1,GL_FALSE,&env_projection[0][0]);
        
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); 
        glfwSwapBuffers(window);
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    
    program.free();
    program_depth.free();
    program_quad.free();
    program_env.free();

    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset, GL_TRUE);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        cout << "Texture failed to load at path: " << path << endl;
        stbi_image_free(data);
    }

    return textureID;
}

void renderScene(const Program &prgm)
{
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(prgm.uniform("model"),1,GL_FALSE,&model[0][0]);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.1f, 0.0f, 0.1f));
    model = glm::scale(model, glm::vec3(0.5f));
    glUniformMatrix4fv(prgm.uniform("model"),1,GL_FALSE,&model[0][0]);
    renderCube();
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    if (cubeVAO == 0)
    {
        float vertices[] = {
            -0.3f, -0.3f, -0.3f,  0.0f,  0.0f, -0.3f, 0.0f, 0.0f, 
             0.3f,  0.3f, -0.3f,  0.0f,  0.0f, -0.3f, 0.3f, 0.3f, 
             0.3f, -0.3f, -0.3f,  0.0f,  0.0f, -0.3f, 0.3f, 0.0f,          
             0.3f,  0.3f, -0.3f,  0.0f,  0.0f, -0.3f, 0.3f, 0.3f, 
            -0.3f, -0.3f, -0.3f,  0.0f,  0.0f, -0.3f, 0.0f, 0.0f, 
            -0.3f,  0.3f, -0.3f,  0.0f,  0.0f, -0.3f, 0.0f, 0.3f, 
            -0.3f, -0.3f,  0.3f,  0.0f,  0.0f,  0.3f, 0.0f, 0.0f, 
             0.3f, -0.3f,  0.3f,  0.0f,  0.0f,  0.3f, 0.3f, 0.0f, 
             0.3f,  0.3f,  0.3f,  0.0f,  0.0f,  0.3f, 0.3f, 0.3f, 
             0.3f,  0.3f,  0.3f,  0.0f,  0.0f,  0.3f, 0.3f, 0.3f, 
            -0.3f,  0.3f,  0.3f,  0.0f,  0.0f,  0.3f, 0.0f, 0.3f, 
            -0.3f, -0.3f,  0.3f,  0.0f,  0.0f,  0.3f, 0.0f, 0.0f, 
            -0.3f,  0.3f,  0.3f, -0.3f,  0.0f,  0.0f, 0.3f, 0.0f, 
            -0.3f,  0.3f, -0.3f, -0.3f,  0.0f,  0.0f, 0.3f, 0.3f, 
            -0.3f, -0.3f, -0.3f, -0.3f,  0.0f,  0.0f, 0.0f, 0.3f, 
            -0.3f, -0.3f, -0.3f, -0.3f,  0.0f,  0.0f, 0.0f, 0.3f, 
            -0.3f, -0.3f,  0.3f, -0.3f,  0.0f,  0.0f, 0.0f, 0.0f, 
            -0.3f,  0.3f,  0.3f, -0.3f,  0.0f,  0.0f, 0.3f, 0.0f, 
             0.3f,  0.3f,  0.3f,  0.3f,  0.0f,  0.0f, 0.3f, 0.0f, 
             0.3f, -0.3f, -0.3f,  0.3f,  0.0f,  0.0f, 0.0f, 0.3f, 
             0.3f,  0.3f, -0.3f,  0.3f,  0.0f,  0.0f, 0.3f, 0.3f,          
             0.3f, -0.3f, -0.3f,  0.3f,  0.0f,  0.0f, 0.0f, 0.3f, 
             0.3f,  0.3f,  0.3f,  0.3f,  0.0f,  0.0f, 0.3f, 0.0f, 
             0.3f, -0.3f,  0.3f,  0.3f,  0.0f,  0.0f, 0.0f, 0.0f,      
            -0.3f, -0.3f, -0.3f,  0.0f, -0.3f,  0.0f, 0.0f, 0.3f, 
             0.3f, -0.3f, -0.3f,  0.0f, -0.3f,  0.0f, 0.3f, 0.3f, 
             0.3f, -0.3f,  0.3f,  0.0f, -0.3f,  0.0f, 0.3f, 0.0f, 
             0.3f, -0.3f,  0.3f,  0.0f, -0.3f,  0.0f, 0.3f, 0.0f, 
            -0.3f, -0.3f,  0.3f,  0.0f, -0.3f,  0.0f, 0.0f, 0.0f, 
            -0.3f, -0.3f, -0.3f,  0.0f, -0.3f,  0.0f, 0.0f, 0.3f, 
            -0.3f,  0.3f, -0.3f,  0.0f,  0.3f,  0.0f, 0.0f, 0.3f, 
             0.3f,  0.3f , 0.3f,  0.0f,  0.3f,  0.0f, 0.3f, 0.0f, 
             0.3f,  0.3f, -0.3f,  0.0f,  0.3f,  0.0f, 0.3f, 0.3f,      
             0.3f,  0.3f,  0.3f,  0.0f,  0.3f,  0.0f, 0.3f, 0.0f, 
            -0.3f,  0.3f, -0.3f,  0.0f,  0.3f,  0.0f, 0.0f, 0.3f, 
            -0.3f,  0.3f,  0.3f,  0.0f,  0.3f,  0.0f, 0.0f, 0.0f        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glBindVertexArray(0);
    }
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return textureID;
}