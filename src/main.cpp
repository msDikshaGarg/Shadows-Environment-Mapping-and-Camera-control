/*#define STB_IMAGE_IMPLEMENTATION
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

VertexBufferObject VBO;
VertexBufferObject VBO_Color;
VertexBufferObject VBO_Centres;
VertexBufferObject VBO_Translations;
VertexBufferObject VBO_Rotations;
VertexBufferObject VBO_Scalings;
VertexBufferObject VBO_Flat_Norm;
VertexBufferObject VBO_Phong_Norm;

std::vector<glm::vec4> V(1);
std::vector<glm::vec4> V_Line(1);
std::vector<glm::vec4> Color(1);
std::vector<glm::vec4> Line_Color(1);
std::vector<glm::vec3> Centres(1);
std::vector<glm::vec3> Line_Centres(1);
std::vector<glm::vec3> Translations(1);
std::vector<glm::vec3> Rotations(1);
std::vector<glm::vec3> Scalings(1);
std::vector<glm::vec3> Line_Translations(1);
std::vector<glm::vec3> Line_Rotations(1);
std::vector<glm::vec3> Line_Scalings(1);
std::vector<glm::vec3> Flat_Normals(1);
std::vector<glm::vec3> Phong_Normals(1);

std::ifstream bumpyFile;

std::vector<glm::vec3> Temp1(502);
std::vector<glm::vec3> Temp2(1000);
std::vector<glm::vec3> Bumpy_vert(1);
std::vector<glm::vec3> Bumpy_Flat_Store(1);
std::vector<glm::vec3> Bumpy_Smooth_Store(1);

std::vector<glm::vec4> V_Bumpy(1);
std::vector<glm::vec4> Color_Bumpy(1);
std::vector<glm::vec4> Lines_Bumpy(1);
std::vector<glm::vec4> Lines_Color_Bumpy(1);
std::vector<glm::vec3> Bumpy_Translations(1);
std::vector<glm::vec3> Bumpy_Rotations(1);
std::vector<glm::vec3> Bumpy_Scalings(1);
std::vector<glm::vec3> Bumpy_Line_Translations(1);
std::vector<glm::vec3> Bumpy_Line_Rotations(1);
std::vector<glm::vec3> Bumpy_Line_Scalings(1);

std::vector<glm::vec3> Bumpy_Centres(1);
std::vector<glm::vec3> Bumpy_Line_Centres(1);
std::vector<glm::vec3> Bumpy_Flat_Normals(1);
std::vector<glm::vec3> Bumpy_Phong_Normals(1);

std::ifstream bunnyFile;

std::vector<glm::vec3> Bunny_vert(1);
std::vector<glm::vec3> Bunny_Flat_store(1);
std::vector<glm::vec3> Bunny_Phong_store(1);

std::vector<glm::vec4> V_Bunny(1);
std::vector<glm::vec4> Color_bunny(1);
std::vector<glm::vec4> Lines_bunny(1);
std::vector<glm::vec4> Lines_color_bunny(1);
std::vector<glm::vec3> Bunny_Translations(1);
std::vector<glm::vec3> Bunny_Rotations(1);
std::vector<glm::vec3> Bunny_Scalings(1);
std::vector<glm::vec3> Bunny_Line_Translations(1);
std::vector<glm::vec3> Bunny_Line_Rotations(1);
std::vector<glm::vec3> Bunny_Line_Scalings(1);

std::vector<glm::vec3> Bunny_Centres(1);
std::vector<glm::vec3> Bunny_Line_Centres(1);
std::vector<glm::vec3> Bunny_Flat_Normals(1);
std::vector<glm::vec3> Bunny_Phong_Normals(1);

int state=0;
double View_Scale=1.0, X_shift=0.0, Y_shift=0.0;
const float cameraSpeed = 0.05f;
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

int selection = 0, selection_Bumpy = -1, selection_Bunny = -1 ,selected_index = -1, V_select_start = -1, V_select_end = -1, Line_select_start = -1, Line_select_end = 1;
float timeGlobal;

const unsigned int SCR_WIDTH = 640;
const unsigned int SCR_HEIGHT = 480;

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

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int planeVAO;

std::vector<glm::vec3> Triangle_Normals(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3)
{
    std::vector<glm::vec3> N(1);
    std::vector<glm::vec3> U2(1);
    std::vector<glm::vec3> V2(1);
    U2[0] = glm::vec3(x2-x1, y2-y1, z2-z1);
    V2[0] = glm::vec3(x3-x1, y3-y1, z3-z1);
    N[0] = glm::vec3(U2[0].y * V2[0].z - U2[0].z * V2[0].y
              , U2[0].z * V2[0].x - U2[0].x * V2[0].z
              , U2[0].x * V2[0].y - U2[0].y * V2[0].x);
    return(N);
}

bool Pt_in_Triangle(int vertex_ID, int Triangle_ID)
{
    if((V[vertex_ID].y == V[Triangle_ID].y) && (V[vertex_ID].z == V[Triangle_ID].z) && (V[vertex_ID].q == V[Triangle_ID].q))
    {
        return(true);
    }
    else if((V[vertex_ID].y == V[Triangle_ID+1].y) && (V[vertex_ID].z == V[Triangle_ID+1].z) && (V[vertex_ID].q == V[Triangle_ID+1].q))
    {
        return(true);
    }
    else if((V[vertex_ID].y == V[Triangle_ID+2].y) && (V[vertex_ID].z == V[Triangle_ID+2].z) && (V[vertex_ID].q == V[Triangle_ID+2].q))
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

void Flat_Norms(int iteration, double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3)
{
    std::vector<glm::vec3> N = Triangle_Normals(x1, y1, z1, x2, y2, z2, x3, y3, z3);
    int n = Flat_Normals.size();
    int i;

    if(n == 1) {
        Flat_Normals.resize(0);
        n = Flat_Normals.size();
    }

    for(i = n; i < n + iteration; i++)
    {
        Flat_Normals.resize(Flat_Normals.size()+1);
        Flat_Normals[i] = glm::vec3(N[0].x, N[0].y, N[0].z);
    }
}

void Phong_Norms()
{
    int i,j;
    int count = 0;
    std::vector<glm::vec3> N(1);
    std::vector<glm::vec3> Sum_norm(1);

    for(i = V_select_start; i < V_select_end; i+=1)
    {
        N = Triangle_Normals(V[i+0].y, V[i+0].z, V[i+0].q,
                             V[i+1].y, V[i+1].z, V[i+1].q,
                             V[i+2].y, V[i+2].z, V[i+2].q);
        Sum_norm[0] = glm::vec3(0.0, 0.0, 0.0);
        count = 0;
        for(j = V_select_start; j < V_select_end; j+=3)
        {
            if(Pt_in_Triangle(i,j))
            {
                N = Triangle_Normals(V[j+0].y, V[j+0].z, V[j+0].q,
                                     V[j+1].y, V[j+1].z, V[j+1].q,
                                     V[j+2].y, V[j+2].z, V[j+2].q);
                Sum_norm[0] = glm::vec3(Sum_norm[0].x + N[0].x, 
                                        Sum_norm[0].y + N[0].y, 
                                        Sum_norm[0].z + N[0].z);
                count += 1;
            }
        }

        Phong_Normals.resize(i+1);
        Phong_Normals[i] = glm::vec3((float)((float)Sum_norm[0].x/count), 
                                     (float)((float)Sum_norm[0].y/count), 
                                     (float)((float)Sum_norm[0].z/count));
    }    
}

void Draw_Lines(double x1, double y1, double z1, double x2, double y2, double z2)
{
    int i = V_Line.size();

    if(i == 1) {
        V_Line.resize(0);
        Line_Color.resize(0);
        Line_Rotations.resize(0);
        Line_Translations.resize(0);
        Line_Scalings.resize(0);
        i = V_Line.size();
    }

    V_Line.resize(i+2);
    Line_Color.resize(i+2);

    V_Line[i] = glm::vec4(i,x1,y1,z1);
    V_Line[i+1] = glm::vec4(i+1,x2,y2,z2);

    Line_Color[i] = glm::vec4(0.0, 0.0, 0.0, 1.0);
    Line_Color[i+1] = glm::vec4(0.0, 0.0, 0.0 ,1.0);

    Line_Rotations.resize(i+2);
    Line_Translations.resize(i+2);
    Line_Scalings.resize(i+2);

    Line_Rotations[i] = glm::vec3(0.0, 0.0, 0.0);
    Line_Rotations[i+1] = glm::vec3(0.0, 0.0, 0.0);
    
    Line_Translations[i] = glm::vec3(0.0, 0.0, 0.0);
    Line_Translations[i+1] = glm::vec3(0.0, 0.0, 0.0);

    Line_Scalings[i] = glm::vec3(1.0, 1.0, 1.0);
    Line_Scalings[i+1] = glm::vec3(1.0, 1.0, 1.0);
}

void Draw_Triangles(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3)
{
    int i = V.size();

    if(i == 1) {
        V.resize(0);
        Color.resize(0);
        Rotations.resize(0);
        Translations.resize(0);
        Scalings.resize(0);
        i = V.size();
    }

    V.resize(i+3);
    Color.resize(i+3);

    V[i] = glm::vec4(i,x1,y1,z1);
    V[i+1] = glm::vec4(i+1,x2,y2,z2);
    V[i+2] = glm::vec4(i+2,x3,y3,z3);

    Draw_Lines(x1, y1, z1, x2, y2, z2);
    Draw_Lines(x2, y2, z2, x3, y3, z3);
    Draw_Lines(x3, y3, z3, x1, y1, z1);

    Color[i+0] = glm::vec4(0.5, 0.0, 0.5, 0.8);
    Color[i+1] = glm::vec4(0.5, 0.0, 0.5, 0.8);
    Color[i+2] = glm::vec4(0.5, 0.0, 0.5, 0.8);

    Rotations.resize(i+3);
    Translations.resize(i+3);
    Scalings.resize(i+3);

    Rotations[i] = glm::vec3(0.0, 0.0, 0.0);
    Rotations[i+1] = glm::vec3(0.0, 0.0, 0.0);
    Rotations[i+2] = glm::vec3(0.0, 0.0, 0.0);
    
    Translations[i] = glm::vec3(0.0, 0.0, 0.0);
    Translations[i+1] = glm::vec3(0.0, 0.0, 0.0);
    Translations[i+2] = glm::vec3(0.0, 0.0, 0.0);
    
    Scalings[i] = glm::vec3(1.0, 1.0, 1.0);
    Scalings[i+1] = glm::vec3(1.0, 1.0, 1.0);
    Scalings[i+2] = glm::vec3(1.0, 1.0, 1.0);

    Flat_Norms(3, x1, y1, z1, x2, y2, z2, x3, y3, z3);
}

void Bumpy_Lines(double x1, double y1, double z1, double x2, double y2, double z2)
{
    int i = Lines_Bumpy.size();

    if(i == 1) {
        Lines_Bumpy.resize(0);
        Lines_Color_Bumpy.resize(0);
        Bumpy_Line_Rotations.resize(0);
        Bumpy_Line_Translations.resize(0);
        Bumpy_Line_Scalings.resize(0);
        i = Lines_Bumpy.size();
    }

    Lines_Bumpy.resize(i+2);
    Lines_Color_Bumpy.resize(i+2);

    Lines_Bumpy[i] = glm::vec4( i,x1,y1,z1);
    Lines_Bumpy[i+1] = glm::vec4(i+1,x2,y2,z2);

    Lines_Color_Bumpy[i]   = glm::vec4(0.0, 0.0, 0.0, 1.0);
    Lines_Color_Bumpy[i+1] = glm::vec4(0.0, 0.0, 0.0 ,1.0);

    Bumpy_Line_Rotations.resize(i+2);
    Bumpy_Line_Translations.resize(i+2);
    Bumpy_Line_Scalings.resize(i+2);

    Bumpy_Line_Rotations[i]   = glm::vec3(0.0, 0.0, 0.0);
    Bumpy_Line_Rotations[i+1] = glm::vec3(0.0, 0.0, 0.0);
    
    Bumpy_Line_Translations[i]   = glm::vec3(0.0, 0.0, 0.0);
    Bumpy_Line_Translations[i+1] = glm::vec3(0.0, 0.0, 0.0);
    
    Bumpy_Line_Scalings[i]   = glm::vec3(1.0, 1.0, 1.0);
    Bumpy_Line_Scalings[i+1] = glm::vec3(1.0, 1.0, 1.0);

}

void drawBumpyTriangle(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3){
    int i = V_Bumpy.size();

    if(i == 1) {
        V_Bumpy.resize(0);
        Color_Bumpy.resize(0);
        Bumpy_Rotations.resize(0);
        Bumpy_Translations.resize(0);
        Bumpy_Scalings.resize(0);
        i = V_Bumpy.size();
    }

    V_Bumpy.resize(i+3);
    Color_Bumpy.resize(i+3);

    V_Bumpy[i]   = glm::vec4(i,x1,y1,z1);
    V_Bumpy[i+1] = glm::vec4(i+1,x2,y2,z2);
    V_Bumpy[i+2] = glm::vec4(i+2,x3,y3,z3);

    Bumpy_Lines(x1, y1, z1, x2, y2, z2);
    Bumpy_Lines(x2, y2, z2, x3, y3, z3);
    Bumpy_Lines(x3, y3, z3, x1, y1, z1);

    Color_Bumpy[i+0] = glm::vec4(0.0, 0.5, 0.5, 0.8);
    Color_Bumpy[i+1] = glm::vec4(0.0, 0.5, 0.5, 0.8);
    Color_Bumpy[i+2] = glm::vec4(0.0, 0.5, 0.5, 0.8);

    Bumpy_Rotations.resize(i+3);
    Bumpy_Translations.resize(i+3);
    Bumpy_Scalings.resize(i+3);

    Bumpy_Rotations[i]   = glm::vec3(0.0, 0.0, 0.0);
    Bumpy_Rotations[i+1] = glm::vec3(0.0, 0.0, 0.0);
    Bumpy_Rotations[i+2] = glm::vec3(0.0, 0.0, 0.0);
    
    Bumpy_Translations[i]   = glm::vec3(0.0, 0.0, 0.0);
    Bumpy_Translations[i+1] = glm::vec3(0.0, 0.0, 0.0);
    Bumpy_Translations[i+2] = glm::vec3(0.0, 0.0, 0.0);
    
    Bumpy_Scalings[i]   = glm::vec3(1.0, 1.0, 1.0);
    Bumpy_Scalings[i+1] = glm::vec3(1.0, 1.0, 1.0);
    Bumpy_Scalings[i+2] = glm::vec3(1.0, 1.0, 1.0);
}

void drawBunnyLines(double x1, double y1, double z1, double x2, double y2, double z2)
{
    int i = Lines_bunny.size();

    if(i == 1) {
        Lines_bunny.resize(0);
        Lines_color_bunny.resize(0);
        Bunny_Line_Rotations.resize(0);
        Bunny_Line_Translations.resize(0);
        Bunny_Line_Scalings.resize(0);
        i = Lines_bunny.size();
    }

    Lines_bunny.resize(i+2);
    Lines_color_bunny.resize(i+2);

    Lines_bunny[i]   = glm::vec4(i,x1,y1,z1);
    Lines_bunny[i+1] = glm::vec4(i+1,x2,y2,z2);

    Lines_color_bunny[i]   = glm::vec4(0.0, 0.0, 0.0, 1.0);
    Lines_color_bunny[i+1] = glm::vec4(0.0, 0.0, 0.0 ,1.0);

    Bunny_Line_Rotations.resize(i+2);
    Bunny_Line_Translations.resize(i+2);
    Bunny_Line_Scalings.resize(i+2);

    Bunny_Line_Rotations[i]   = glm::vec3(0.0, 0.0, 0.0);
    Bunny_Line_Rotations[i+1] = glm::vec3(0.0, 0.0, 0.0);
    
    Bunny_Line_Translations[i]   = glm::vec3(0.0, 0.0, 0.0);
    Bunny_Line_Translations[i+1] = glm::vec3(0.0, 0.0, 0.0);
    
    Bunny_Line_Scalings[i]   = glm::vec3(1.0, 1.0, 1.0);
    Bunny_Line_Scalings[i+1] = glm::vec3(1.0, 1.0, 1.0);
}

void drawBunnyTriangle(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3)
{
    int i = V_Bunny.size();

    if(i == 1) {
        V_Bunny.resize(0);
        Color_bunny.resize(0);
        Bunny_Rotations.resize(0);
        Bunny_Translations.resize(0);
        Bunny_Scalings.resize(0);
        i = V_Bunny.size();
    }

    V_Bunny.resize(i+3);
    Color_bunny.resize(i+3);

    V_Bunny[i]   = glm::vec4(i,x1,y1,z1);
    V_Bunny[i+1] = glm::vec4(i+1,x2,y2,z2);
    V_Bunny[i+2] = glm::vec4(i+2,x3,y3,z3);

    drawBunnyLines(x1, y1, z1, x2, y2, z2);
    drawBunnyLines(x2, y2, z2, x3, y3, z3);
    drawBunnyLines(x3, y3, z3, x1, y1, z1);

    Color_bunny[i+0] = glm::vec4(0.5, 0.5, 0.0, 0.8);
    Color_bunny[i+1] = glm::vec4(0.5, 0.5, 0.0, 0.8);
    Color_bunny[i+2] = glm::vec4(0.5, 0.5, 0.0, 0.8);

    Bunny_Rotations.resize(i+3);
    Bunny_Translations.resize(i+3);
    Bunny_Scalings.resize(i+3);

    Bunny_Rotations[i]   = glm::vec3(0.0, 0.0, 0.0);
    Bunny_Rotations[i+1] = glm::vec3(0.0, 0.0, 0.0);
    Bunny_Rotations[i+2] = glm::vec3(0.0, 0.0, 0.0);
    
    Bunny_Translations[i]   = glm::vec3(0.0, 0.0, 0.0);
    Bunny_Translations[i+1] = glm::vec3(0.0, 0.0, 0.0);
    Bunny_Translations[i+2] = glm::vec3(0.0, 0.0, 0.0);
    
    Bunny_Scalings[i]   = glm::vec3(1.0, 1.0, 1.0);
    Bunny_Scalings[i+1] = glm::vec3(1.0, 1.0, 1.0);
    Bunny_Scalings[i+2] = glm::vec3(1.0, 1.0, 1.0);
}

void deleteSelected() {
    int i, total_elements = 0;
    
    if(selection != 0)
    {
        if(V.size()>0) {
            selected_index = 0;
        }
        else {
            selected_index = -1;
        }

        V_select_start = selected_index*36;
        V_select_end = (selected_index+1)*36;
        Line_select_start =  selected_index*72;
        Line_select_end = (selected_index+1)*72;
    }

    selection = 0;
    if(selection == 0) {
        if(selected_index > -1)
        {
            total_elements = V.size()/36;
            if(selected_index == total_elements-1) {
                V.resize(V.size()-36);
                Color.resize(Color.size()-36);        
                Centres.resize(Centres.size()-36);      
                Translations.resize(Translations.size()-36); 
                Rotations.resize(Rotations.size()-36);    
                Scalings.resize(Scalings.size()-36);     
                Flat_Normals.resize(Flat_Normals.size()-36); 
                Phong_Normals.resize(Phong_Normals.size()-36);
                
                V_Line.resize(V_Line.size()-72);
                Line_Color.resize(Line_Color.size()-72);
                Line_Centres.resize(Line_Centres.size()-72);
                Line_Translations.resize(Line_Translations.size()-72);
                Line_Rotations.resize(Line_Rotations.size()-72);
                Line_Scalings.resize(Line_Scalings.size()-72);
            }else {
                for(i = V_select_start; i < V.size(); i++)
                {
                    V[i] = V[i+36];
                    Color[i] = Color[i+36];
                    Centres[i] = Centres[i+36];
                    Translations[i] = Translations[i+36];
                    Rotations[i] = Rotations[i+36];
                    Scalings[i] = Scalings[i+36];
                    Flat_Normals[i] = Flat_Normals[i+36];
                    Phong_Normals[i] = Phong_Normals[i+36];
                }

                V.resize(V.size()-36);
                Color.resize(Color.size()-36);        
                Centres.resize(Centres.size()-36);      
                Translations.resize(Translations.size()-36); 
                Rotations.resize(Rotations.size()-36);    
                Scalings.resize(Scalings.size()-36);     
                Flat_Normals.resize(Flat_Normals.size()-36); 
                Phong_Normals.resize(Phong_Normals.size()-36);

                for(i = selected_index*72; i < V_Line.size(); i++)
                {
                    V_Line[i] = V_Line[i+72];
                    Line_Color[i] = Line_Color[i+72];
                    Line_Centres[i] = Line_Centres[i+72];
                    Line_Translations[i] = Line_Translations[i+72];
                    Line_Rotations[i] = Line_Rotations[i+72];
                    Line_Scalings[i] = Line_Scalings[i+72];
                }
                
                V_Line.resize(V_Line.size()-72);
                Line_Color.resize(Line_Color.size()-72);
                Line_Centres.resize(Line_Centres.size()-72);
                Line_Translations.resize(Line_Translations.size()-72);
                Line_Rotations.resize(Line_Rotations.size()-72);
                Line_Scalings.resize(Line_Scalings.size()-72);
            }
        }

        if (V.size() == 0)
        {
            selected_index = -1;
            V_select_start = -1;
            V_select_end = -1;
            V.resize(1);
            Color.resize(1);        
            Centres.resize(1);      
            Translations.resize(1); 
            Rotations.resize(1);    
            Scalings.resize(1);     
            Flat_Normals.resize(1); 
            Phong_Normals.resize(1);

            V_Line.resize(1);
            Line_Color.resize(1);
            Line_Centres.resize(1);
            Line_Translations.resize(1);
            Line_Rotations.resize(1);
            Line_Scalings.resize(1);
            return;
        }else if((selected_index+1)*36 == V.size())
        {
            selected_index = 0;
        }else
        {
            selected_index += 1;
        }
        V_select_start = selected_index*36;
        V_select_end = (selected_index+1)*36;
        Line_select_start =  selected_index*72;
        Line_select_end = (selected_index+1)*72;
    }else if(selection == 2) {
        if(selection_Bumpy > -1)
        {
            total_elements = V_Bumpy.size()/3000;
            if(selection_Bumpy == total_elements-1) {
                V_Bumpy.resize(V_Bumpy.size()-3000);
                Color_Bumpy.resize(Color_Bumpy.size()-3000);        
                Bumpy_Centres.resize(Bumpy_Centres.size()-3000);      
                Bumpy_Translations.resize(Bumpy_Translations.size()-3000); 
                Bumpy_Rotations.resize(Bumpy_Rotations.size()-3000);    
                Bumpy_Scalings.resize(Bumpy_Scalings.size()-3000);     
                Bumpy_Flat_Normals.resize(Bumpy_Flat_Normals.size()-3000); 
                Bumpy_Phong_Normals.resize(Bumpy_Phong_Normals.size()-3000);
                Bumpy_vert.resize(Bumpy_vert.size()-3000);
                Bumpy_Flat_Store.resize(Bumpy_Flat_Store.size()-3000);
                Bumpy_Smooth_Store.resize(Bumpy_Smooth_Store.size()-3000);

                Lines_Bumpy.resize(Lines_Bumpy.size()-6000);
                Lines_Color_Bumpy.resize(Lines_Color_Bumpy.size()-6000);
                Bumpy_Line_Centres.resize(Bumpy_Line_Centres.size()-6000);
                Bumpy_Line_Translations.resize(Bumpy_Line_Translations.size()-6000);
                Bumpy_Line_Rotations.resize(Bumpy_Line_Rotations.size()-6000);
                Bumpy_Line_Scalings.resize(Bumpy_Line_Scalings.size()-6000);
            }else {
                for(i = selection_Bumpy*3000; i < V_Bumpy.size(); i++)
                {
                    V_Bumpy[i] = V_Bumpy[i+3000];
                    Color_Bumpy[i] = Color_Bumpy[i+3000];
                    Bumpy_Centres[i] = Bumpy_Centres[i+3000];
                    Bumpy_Translations[i] = Bumpy_Translations[i+3000];
                    Bumpy_Rotations[i] = Bumpy_Rotations[i+3000];
                    Bumpy_Scalings[i] = Bumpy_Scalings[i+3000];
                    Bumpy_Flat_Normals[i] = Bumpy_Flat_Normals[i+3000];
                    Bumpy_Phong_Normals[i] = Bumpy_Phong_Normals[i+3000];
                    Bumpy_vert[i] = Bumpy_vert[i+3000];
                    Bumpy_Flat_Store[i] = Bumpy_Flat_Store[i+3000];
                    Bumpy_Smooth_Store[i] = Bumpy_Smooth_Store[i+3000];
                }

                V_Bumpy.resize(V_Bumpy.size()-3000);
                Color_Bumpy.resize(Color_Bumpy.size()-3000);        
                Bumpy_Centres.resize(Bumpy_Centres.size()-3000);      
                Bumpy_Translations.resize(Bumpy_Translations.size()-3000); 
                Bumpy_Rotations.resize(Bumpy_Rotations.size()-3000);    
                Bumpy_Scalings.resize(Bumpy_Scalings.size()-3000);     
                Bumpy_Flat_Normals.resize(Bumpy_Flat_Normals.size()-3000); 
                Bumpy_Phong_Normals.resize(Bumpy_Phong_Normals.size()-3000);
                Bumpy_vert.resize(Bumpy_vert.size()-3000);
                Bumpy_Flat_Store.resize(Bumpy_Flat_Store.size()-3000);
                Bumpy_Smooth_Store.resize(Bumpy_Smooth_Store.size()-3000);

                for(i = selection_Bumpy*6000; i < Lines_Bumpy.size(); i++)
                {
                    Lines_Bumpy[i] = Lines_Bumpy[i+6000];
                    Lines_Color_Bumpy[i] = Lines_Color_Bumpy[i+6000];
                    Bumpy_Line_Centres[i] = Bumpy_Line_Centres[i+6000];
                    Bumpy_Line_Translations[i] = Bumpy_Line_Translations[i+6000];
                    Bumpy_Line_Rotations[i] = Bumpy_Line_Rotations[i+6000];
                    Bumpy_Line_Scalings[i] = Bumpy_Line_Scalings[i+6000];
                }
                
                Lines_Bumpy.resize(Lines_Bumpy.size()-6000);
                Lines_Color_Bumpy.resize(Lines_Color_Bumpy.size()-6000);
                Bumpy_Line_Centres.resize(Bumpy_Line_Centres.size()-6000);
                Bumpy_Line_Translations.resize(Bumpy_Line_Translations.size()-6000);
                Bumpy_Line_Rotations.resize(Bumpy_Line_Rotations.size()-6000);
                Bumpy_Line_Scalings.resize(Bumpy_Line_Scalings.size()-6000);
            }
        }
        if(V_Bumpy.size() == 0) {
            selection_Bumpy = -1;
            V_select_start = -1;
            V_select_end = -1;
            V_Bumpy.resize(1);
            Color_Bumpy.resize(1);        
            Bumpy_Centres.resize(1);      
            Bumpy_Translations.resize(1); 
            Bumpy_Rotations.resize(1);    
            Bumpy_Scalings.resize(1);     
            Bumpy_Flat_Normals.resize(1); 
            Bumpy_Phong_Normals.resize(1);
            Bumpy_vert.resize(1);
            Bumpy_Flat_Store.resize(1);
            Bumpy_Smooth_Store.resize(1);

            Lines_Bumpy.resize(1);
            Lines_Color_Bumpy.resize(1);
            Bumpy_Line_Centres.resize(1);
            Bumpy_Line_Translations.resize(1);
            Bumpy_Line_Rotations.resize(1);
            Bumpy_Line_Scalings.resize(1);
        }else if((selection_Bumpy+1)*3000 == V.size())
        {
            selection_Bumpy = 0;
        }else
        {
            selection_Bumpy += 1;
        }
        V_select_start = selection_Bumpy*3000;
        V_select_end = (selection_Bumpy+1)*3000;
        Line_select_start =  selection_Bumpy*6000;
        Line_select_end = (selection_Bumpy+1)*6000;
    }
}

void Next_sel()
{
    int i;

    if(selection != 0)
    {
        if(V.size()>0) {
            selected_index = 0;
        }
        else {
            selected_index = -1;
        }

        V_select_start = selected_index*36;
        V_select_end = (selected_index+1)*36;
        Line_select_start =  selected_index*72;
        Line_select_end = (selected_index+1)*72;
    }

    selection = 0;
    if(selected_index > -1)
    {
        for(i = V_select_start; i < V_select_end; i++)
        {
            Color[i] = glm::vec4(0.5, 0.0, 0.5, Color[i].q);
        }
        for(i = selected_index*72; i < (selected_index+1)*72; i++)
        {
            Line_Color[i] = glm::vec4(0.0, 0.0, 0.0, Line_Color[i].q);
        }
    }
    if(selection_Bumpy > -1)
    {
        for(i = selection_Bumpy*3000; i < (selection_Bumpy+1)*3000; i++)
        {
            Color_Bumpy[i] = glm::vec4(0.0, 0.5, 0.5, Color_Bumpy[i].q);
        }
        for(i = selection_Bumpy*6000; i < (selection_Bumpy+1)*6000; i++)
        {
            Lines_Color_Bumpy[i] = glm::vec4(0.0, 0.0, 0.0, Lines_Color_Bumpy[i].q);
        }
    }
    if(selection_Bunny > -1)
    {
        for(i = selection_Bunny*3000; i < (selection_Bunny+1)*3000; i++)
        {
            Color_bunny[i] = glm::vec4(0.5, 0.5, 0.0, Color_bunny[i].q);
        }
        for(i = selection_Bunny*6000; i < (selection_Bunny+1)*6000; i++)
        {
            Lines_color_bunny[i] = glm::vec4(0.0, 0.0, 0.0, Lines_color_bunny[i].q);
        }
    }
    if (V.size() == 0)
    {
        selected_index = -1;
        V_select_start = -1;
        V_select_end = -1;
        return;
    }
    else if((selected_index+1)*36 == V.size())
    {
        selected_index = 0;
    }
    else
    {
        selected_index += 1;
    }
    V_select_start = selected_index*36;
    V_select_end = (selected_index+1)*36;
    Line_select_start =  selected_index*72;
    Line_select_end = (selected_index+1)*72;
   
    if(selected_index > -1)
    {
        for(i = V_select_start; i < V_select_end; i++)
        {
            Color[i] = glm::vec4(0.0, 0.05, 0.05, Color[i].q);
        }
        if(Color[V_select_start].q < 0.1)
        {
            for(i = Line_select_start; i < Line_select_end; i++){
                Line_Color[i] = glm::vec4(1.0, 0.0, 0.0, Line_Color[i].q);
            }
        }
    }
}

void Next_selBumpy()
{
    int i;
    if(selection != 2)
    {
        if(V_Bumpy.size()>0){
            selection_Bumpy = 0;
        }
        else{
            selection_Bumpy = -1;
        }

        V_select_start = selection_Bumpy*36;
        V_select_end = (selection_Bumpy+1)*36;
        Line_select_start =  selection_Bumpy*72;
        Line_select_end = (selection_Bumpy+1)*72;
    }

    selection = 2;
    if(selection_Bunny > -1)
    {
        for(i = selection_Bunny*3000; i < (selection_Bunny+1)*3000; i++)
        {
            Color_bunny[i] = glm::vec4(0.5, 0.5, 0.0, Color_bunny[i].q);
        }
        for(i = selection_Bunny*6000; i < (selection_Bunny+1)*6000; i++)
        {
            Lines_color_bunny[i] = glm::vec4(0.0, 0.0, 0.0, Lines_color_bunny[i].q);
        }
    }
    if(selected_index > -1){
        for(i = selected_index*36; i < (selected_index+1)*36; i++)
        {
            Color[i] = glm::vec4(0.5, 0.0, 0.5, Color[i].q);
        }
        for(i = selected_index*72; i < (selected_index+1)*72; i++)
        {
            Line_Color[i] = glm::vec4(0.0, 0.0, 0.0, Line_Color[i].q);
        }
    }
    if(selection_Bumpy > -1)
    {
        for(i = selection_Bumpy*3000; i < (selection_Bumpy+1)*3000; i++)
        {
            Color_Bumpy[i] = glm::vec4(0.0, 0.5, 0.5, Color_Bumpy[i].q);
        }
        for(i = selection_Bumpy*6000; i < (selection_Bumpy+1)*6000; i++)
        {
            Lines_Color_Bumpy[i] = glm::vec4(0.0, 0.0, 0.0, Lines_Color_Bumpy[i].q);
        }
    }
    if (V_Bumpy.size() == 0)
    {
        selection_Bumpy = -1;
        V_select_start = -1;
        V_select_end = -1;
        return;
    }
    else if((selection_Bumpy+1)*3000 == V_Bumpy.size())
    {
        selection_Bumpy = 0;
    }
    else
    {
        selection_Bumpy += 1;
    }
    V_select_start = selection_Bumpy*3000;
    V_select_end = (selection_Bumpy+1)*3000;
    Line_select_start =  selection_Bumpy*6000;
    Line_select_end = (selection_Bumpy+1)*6000;
   
    if(selection_Bumpy > -1)
    {
        for(i = V_select_start; i < V_select_end; i++)
        {
            Color_Bumpy[i] = glm::vec4(0.0, 0.05, 0.05, Color_Bumpy[i].q);
        }
        if(Color_Bumpy[V_select_start].q < 0.1)
        {
            for(i = Line_select_start; i < Line_select_end; i++)
            {
                Lines_Color_Bumpy[i] = glm::vec4(1.0, 0.0, 0.0, Lines_Color_Bumpy[i].q);
            }
        }
    }
}
void Next_selBunny()
{
    int i;
    if(selection != 1)
    {
        if(V_Bunny.size()>0){
            selection_Bunny = 0;
        }
        else{
            selection_Bunny = -1;
        }
        V_select_start = selection_Bunny*36;
        V_select_end = (selection_Bunny+1)*36;
        Line_select_start =  selection_Bunny*72;
        Line_select_end = (selection_Bunny+1)*72;
    }
    selection = 1;
    if(selection_Bunny > -1)
    {
        for(i = V_select_start; i < V_select_end; i++)
        {
            Color_bunny[i] = glm::vec4(0.5, 0.5, 0.0, Color_bunny[i].q);
        }
        for(i = selection_Bunny*6000; i < (selection_Bunny+1)*6000; i++)
        {
            Lines_color_bunny[i] = glm::vec4(0.0, 0.0, 0.0, Lines_color_bunny[i].q);
        }
    }
    if(selected_index > -1)
    {
        for(i = selected_index*36; i < (selected_index+1)*36; i++)
        {
            Color[i] = glm::vec4(0.5, 0.0, 0.5, Color[i].q);
        }
        for(i = selected_index*72; i < (selected_index+1)*72; i++)
        {
            Line_Color[i] = glm::vec4(0.0, 0.0, 0.0, Line_Color[i].q);
        }
    }
    if(selection_Bumpy > -1)
    {
        for(i = selection_Bumpy*3000; i < (selection_Bumpy+1)*3000; i++)
        {
            Color_Bumpy[i] = glm::vec4(0.0, 0.5, 0.5, Color_Bumpy[i].q);
        }
        for(i = selection_Bumpy*6000; i < (selection_Bumpy+1)*6000; i++)
        {
            Lines_Color_Bumpy[i] = glm::vec4(0.0, 0.0, 0.0, Lines_Color_Bumpy[i].q);
        }
    }
    if (V_Bunny.size() == 0)
    {
        selected_index = -1;
        V_select_start = -1;
        V_select_end = -1;
        return;
    }
    else if((selection_Bunny+1)*3000 == V_Bunny.size())
    {
        selection_Bunny = 0;
    }
    else
    {
        selection_Bunny += 1;
    }
    V_select_start = selection_Bunny*3000;
    V_select_end = (selection_Bunny+1)*3000;
    Line_select_start =  selection_Bunny*6000;
    Line_select_end = (selection_Bunny+1)*6000;
   
    if(selection_Bunny > -1)
    {
        for(i = V_select_start; i < V_select_end; i++)
        {
            Color_bunny[i] = glm::vec4(0.0, 0.05, 0.05, Color_bunny[i].q);
        }
        if(Color_bunny[V_select_start].q < 0.1)
        {
            for(i = Line_select_start; i < Line_select_end; i++)
            {
                Lines_color_bunny[i] = glm::vec4(1.0, 0.0, 0.0, Lines_color_bunny[i].q);
            }
        }
    }
}

void Bunny_data(float x0, float Y0, float z0, float x1, float Y1, float z1, float x2, float Y2, float z2)
{
    int i = Bunny_vert.size();

    if(i == 1) {
        Bunny_vert.resize(0);
        i = Bunny_vert.size();
    }
    Bunny_vert.resize(i+3);
    Bunny_vert[i]   = glm::vec3(x0,Y0,z0);
    Bunny_vert[i+1] = glm::vec3(x1,Y1,z1);
    Bunny_vert[i+2] = glm::vec3(x2,Y2,z2);
}

void Bumpy_data(float x0, float Y0, float z0, float x1, float Y1, float z1, float x2, float Y2, float z2)
{
    int i = Bumpy_vert.size();

    if(i == 1) {
        Bumpy_vert.resize(0);
        i = Bumpy_vert.size();
    }

    Bumpy_vert.resize(i+3);
    Bumpy_vert[i]   = glm::vec3(x0,Y0,z0);
    Bumpy_vert[i+1] = glm::vec3(x1,Y1,z1);
    Bumpy_vert[i+2] = glm::vec3(x2,Y2,z2);
}

std::vector<glm::vec3> Scaling_B(std::vector<glm::vec3> M)
{
    std::vector<glm::vec3> res(0);
    int i;
    float x,y,z;
    float XSum = 0;
    float YSum = 0;
    float ZSum = 0;

    float xMax = 0., yMax = 0., zMax = 0.;
    float xVal, yVal, zVal;
    for(i=0; i<M.size(); i++)
    {
        xVal = M[i].x;
        yVal = M[i].y;
        zVal = M[i].z;
       
        xMax = abs(xVal) > abs(xMax) ? abs(xVal):abs(xMax);
        yMax = abs(yVal) > abs(yMax) ? abs(yVal):abs(yMax);
        zMax = abs(zVal) > abs(zMax) ? abs(zVal):abs(zMax);
    }
    float maxAll = xMax > yMax ? (xMax > zMax) ? xMax:zMax : (yMax > zMax) ? yMax : zMax;

    res.resize(M.size());
    for(i=0;i<M.size();i++){
        x = (M[i].x)/(2*maxAll);
        y = (M[i].y)/(2*maxAll);
        z = (M[i].z)/(2*maxAll);
        XSum += x;
        YSum += y;
        ZSum += z;
        res[i] = glm::vec3(x, y ,z);
    }

    for(i=0;i<M.size();i++)
    {
        x = res[i].x;
        res[i] = glm::vec3(M[i].x/(2*maxAll), M[i].y/(2*maxAll), M[i].z/(2*maxAll));
    }

    x = XSum/(float)res.size();
    y = YSum/(float)res.size();
    z = ZSum/(float)res.size();

    for(i=0; i<res.size(); i++)
    {
        res[i] = glm::vec3(res[i].x-x, res[i].y-y, res[i].z-z);
    }

    return res;
}

void initializeBunny()
{
    Temp1.resize(0);
    Temp2.resize(0);
    Temp1.resize(502);
    Temp2.resize(1000);
    bunnyFile.open("../data/bunny.off");
    int i = 1;
    int j = 0;
    int count = 0;
    if(bunnyFile.is_open())
    {
        while(!bunnyFile.eof()){
            std::string line;
            getline(bunnyFile,line);
            std::istringstream ss(line);
            if(i != 1 && i != 2 && i<=504)
            {
                j = 0;
                do
                {
                    std::string num;
                    ss >> num;
                    Temp1[i-3][j] = stof(num);
                    j++;
                }while(ss && j!=3);
            }
            if(i != 1 && i != 2 && i>504 && i<=1504)
            {
                int j=0;
                do
                {
                    std::string num;
                    ss>>num;
                    if(j>0){
                        Temp2[i-505][j-1] = stoi(num);
                    }
                    j++;
                }
                while(ss && j!=4);
            }
            i++;
        }
    }
    bunnyFile.close();
    Bunny_vert.resize(0);
    
    for(j=0; j<Temp2.size(); j++)
    {
        Bunny_data(Temp1[Temp2[j][0]][0], Temp1[Temp2[j][0]][1], Temp1[Temp2[j][0]][2],
                   Temp1[Temp2[j][1]][0], Temp1[Temp2[j][1]][1], Temp1[Temp2[j][1]][2],
                   Temp1[Temp2[j][2]][0], Temp1[Temp2[j][2]][1], Temp1[Temp2[j][2]][2]);
    }
    
    Bunny_vert = Scaling_B(Bunny_vert);
    std::vector<glm::vec3> N(1);
    int n;
    
    if(Bunny_Flat_store.size() == 0) {
        Bunny_Flat_store.resize(0);
    }

    Bunny_Flat_store.resize(Bunny_vert.size());
    for(i=0; i < Bunny_vert.size(); i+=3)
    {
        N = Triangle_Normals(Bunny_vert[i].x,   Bunny_vert[i].y,   Bunny_vert[i].z,
                                                    Bunny_vert[i+1].x, Bunny_vert[i+1].y, Bunny_vert[i+1].z,
                                                    Bunny_vert[i+2].x, Bunny_vert[i+2].y, Bunny_vert[i+2].z);

        Bunny_Flat_store[i]   = glm::vec3(N[0].x, N[0].y, N[0].z);
        Bunny_Flat_store[i+1] = glm::vec3(N[0].x, N[0].y, N[0].z);
        Bunny_Flat_store[i+2] = glm::vec3(N[0].x, N[0].y, N[0].z);
    }

    if(Bunny_Phong_store.size() == 0) {
        Bunny_Phong_store.resize(0);
    }

    Bunny_Phong_store.resize(Bunny_vert.size());
    
    std::vector<glm::vec3> Sum_norm(1);
    for(i = 0; i < Bunny_vert.size(); i+=1)
    {
        N = Triangle_Normals(Bunny_vert[i].x,   Bunny_vert[i].y,   Bunny_vert[i].z,
                             Bunny_vert[i+1].x, Bunny_vert[i+1].y, Bunny_vert[i+1].z,
                             Bunny_vert[i+2].x, Bunny_vert[i+2].y, Bunny_vert[i+2].z);
        Sum_norm[0] = glm::vec3(0.0, 0.0, 0.0);
        count = 0;
        for(j = 0; j < Bunny_vert.size(); j+=3)
        {
            if(((Bunny_vert[i].x == Bunny_vert[j].x)   && (Bunny_vert[i].y == Bunny_vert[j].y)   && (Bunny_vert[i].z == Bunny_vert[j].z)) ||
               ((Bunny_vert[i].x == Bunny_vert[j+1].x) && (Bunny_vert[i].y == Bunny_vert[j+1].y) && (Bunny_vert[i].z == Bunny_vert[j+1].z)) ||
               ((Bunny_vert[i].x == Bunny_vert[j+2].x) && (Bunny_vert[i].y == Bunny_vert[j+2].y) && (Bunny_vert[i].z == Bunny_vert[j+2].z)))
            {
                N = Triangle_Normals(Bunny_vert[j].x,   Bunny_vert[j].y,   Bunny_vert[j].z,
                                     Bunny_vert[j+1].x, Bunny_vert[j+1].y, Bunny_vert[j+1].z,
                                     Bunny_vert[j+2].x, Bunny_vert[j+2].y, Bunny_vert[j+2].z);
                Sum_norm[0] = glm::vec3(Sum_norm[0].x + N[0].x, Sum_norm[0].y + N[0].y, Sum_norm[0].z + N[0].z);
                count += 1;
            }
        }
        Bunny_Phong_store[i] =  glm::vec3((float)((float)Sum_norm[0].x/count), (float)((float)Sum_norm[0].y/count), (float)((float)Sum_norm[0].z/count));
    }
}

void initializeBumpy(){
    Temp1.resize(0);
    Temp2.resize(0);
    Temp1.resize(502);
    Temp2.resize(1000);
    bumpyFile.open("../data/bumpy_cube.off");
    int i = 1;
    int j = 0;
    int count = 0;
    if(bumpyFile.is_open())
    {
        while(!bumpyFile.eof())
        {
            std::string line;
            getline(bumpyFile,line);
            std::istringstream ss(line);
            if(i != 1 && i != 2 && i<=504)
            {
                j = 0;
                do
                {
                    std::string num;
                    ss >> num;
                    Temp1[i - 3][j] = stof(num);
                    j++;
                }
                while(ss && j!=3);
            }
            if(i != 1 && i != 2 && i>504 && i<=1504)
            {
                int j=0;
                do
                {
                    std::string num;
                    ss>>num;
                    if(j>0)
                    {
                        Temp2[i-505][j-1] = stoi(num);
                    }
                    j++;
                }
                while(ss && j!=4);
            }
            i++;
        }
    }
    bumpyFile.close();
    Bumpy_vert.resize(0);
    for(j=0; j<Temp2.size(); j++)
    {
               Bumpy_data(Temp1[Temp2[j][0]][0], Temp1[Temp2[j][0]][1], Temp1[Temp2[j][0]][2],
                          Temp1[Temp2[j][1]][0], Temp1[Temp2[j][1]][1], Temp1[Temp2[j][1]][2],
                          Temp1[Temp2[j][2]][0], Temp1[Temp2[j][2]][1], Temp1[Temp2[j][2]][2]);
    }
    Bumpy_vert = Scaling_B(Bumpy_vert);
    std::vector<glm::vec3> N(1);
    int n;

    if(Bumpy_Flat_Store.size() == 1) {
        Bumpy_Flat_Store.resize(0);
    }
    Bumpy_Flat_Store.resize(Bumpy_vert.size());
    for(i=0; i < Bumpy_vert.size(); i+=3)
    {
        N = Triangle_Normals(Bumpy_vert[i].x, Bumpy_vert[i].y, Bumpy_vert[i].z,
                             Bumpy_vert[i+1].x, Bumpy_vert[i+1].y, Bumpy_vert[i+1].z,
                             Bumpy_vert[i+2].x, Bumpy_vert[i+2].y, Bumpy_vert[i+2].z);

        Bumpy_Flat_Store[i]   = glm::vec3(N[0].x, N[0].y, N[0].z);
        Bumpy_Flat_Store[i+1] = glm::vec3(N[0].x, N[0].y, N[0].z);
        Bumpy_Flat_Store[i+2] = glm::vec3(N[0].x, N[0].y, N[0].z);
    }

    if(Bumpy_Smooth_Store.size() == 1) {
        Bumpy_Smooth_Store.resize(0);
    }
    Bumpy_Smooth_Store.resize(Bumpy_vert.size());
    std::vector<glm::vec3> Sum_norm(1);
    for(i = 0; i < Bumpy_vert.size(); i+=1)
    {
        N = Triangle_Normals(Bumpy_vert[i].x, Bumpy_vert[i].y, Bumpy_vert[i].z,
                             Bumpy_vert[i+1].x, Bumpy_vert[i+1].y, Bumpy_vert[i+1].z,
                             Bumpy_vert[i+2].x, Bumpy_vert[i+2].y, Bumpy_vert[i+2].z);
        Sum_norm[0] = glm::vec3(0.0, 0.0, 0.0);
        count = 0;
        for(j = 0; j < Bumpy_vert.size(); j+=3)
        {
            if(((Bumpy_vert[i].x == Bumpy_vert[j].x)   && (Bumpy_vert[i].y == Bumpy_vert[j].y)   && (Bumpy_vert[i].z == Bumpy_vert[j].z)) ||
               ((Bumpy_vert[i].x == Bumpy_vert[j+1].x) && (Bumpy_vert[i].y == Bumpy_vert[j+1].y) && (Bumpy_vert[i].z == Bumpy_vert[j+1].z)) ||
               ((Bumpy_vert[i].x == Bumpy_vert[j+2].x) && (Bumpy_vert[i].y == Bumpy_vert[j+2].y) && (Bumpy_vert[i].z == Bumpy_vert[j+2].z)))
            {
                N = Triangle_Normals(Bumpy_vert[j].x, Bumpy_vert[j].y, Bumpy_vert[j].z,
                                     Bumpy_vert[j+1].x, Bumpy_vert[j+1].y, Bumpy_vert[j+1].z,
                                     Bumpy_vert[j+2].x, Bumpy_vert[j+2].y, Bumpy_vert[j+2].z);
                Sum_norm[0] = glm::vec3(Sum_norm[0].x + N[0].x, Sum_norm[0].y + N[0].y, Sum_norm[0].z + N[0].z);
                count += 1;
            }
        }
        Bumpy_Smooth_Store[i] = glm::vec3((float)((float)Sum_norm[0].x/count), (float)((float)Sum_norm[0].y/count), (float)((float)Sum_norm[0].z/count));
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    double xworld = ((((xpos/double(width))*2)-1)-X_shift)/View_Scale;
    double yworld = (((((height-1-ypos)/double(height))*2)-1)-Y_shift)/View_Scale; 
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {}
    VBO.update(V);
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    int i, iLines, j;
    double xVals, yVals, zVals, alphaVal;
    
    if(action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_1:
                i = V.size();
                iLines = V_Line.size();
                Draw_Triangles(-0.5f,-0.5f,-0.5f, -0.5f,-0.5f, 0.5f, -0.5f, 0.5f, 0.5f);
                Draw_Triangles(0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f, -0.5f, 0.5f,-0.5f);
                Draw_Triangles(0.5f,-0.5f, 0.5f, -0.5f,-0.5f,-0.5f, 0.5f,-0.5f,-0.5f);
                Draw_Triangles(0.5f, 0.5f,-0.5f, 0.5f,-0.5f,-0.5f, -0.5f,-0.5f,-0.5f);
                Draw_Triangles(-0.5f,-0.5f,-0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f);
                Draw_Triangles(0.5f,-0.5f, 0.5f, -0.5f,-0.5f, 0.5f, -0.5f,-0.5f,-0.5f);
                Draw_Triangles(-0.5f, 0.5f, 0.5f, -0.5f,-0.5f, 0.5f, 0.5f,-0.5f, 0.5f);
                Draw_Triangles(0.5f, 0.5f, 0.5f, 0.5f,-0.5f,-0.5f, 0.5f, 0.5f,-0.5f);
                Draw_Triangles(0.5f,-0.5f,-0.5f, 0.5f, 0.5f, 0.5f, 0.5f,-0.5f, 0.5f);
                Draw_Triangles(0.5f, 0.5f, 0.5f, 0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f);
                Draw_Triangles(0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f, -0.5f, 0.5f, 0.5f);
                Draw_Triangles(0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f,-0.5f, 0.5f);
                j = 0;
                xVals = 0;
                yVals = 0;
                zVals = 0;
                
                if(i == 1) {
                    Centres.resize(0);   
                    i = 0; 
                }
                Centres.resize(V.size());

                for(j = i; j < V.size(); j++)
                {
                    xVals += V[j].y;
                    yVals += V[j].z;
                    zVals += V[j].q;
                }

                xVals = xVals/(V.size()-i);
                yVals = yVals/(V.size()-i);
                zVals = zVals/(V.size()-i);
                
                for(j = i; j < V.size(); j++)
                {
                        Centres[j] = glm::vec3(xVals, yVals, zVals);
                }
                
                if(iLines == 1) {
                    Line_Centres.resize(0);
                    iLines = 0;
                }
                Line_Centres.resize(V_Line.size());
                for(j = iLines; j < V_Line.size(); j++)
                {
                    Line_Centres[j] = glm::vec3(xVals, yVals, zVals);
                }
                
                Next_sel();
                Phong_Norms();
                
                VBO.update(V);
                VBO_Color.update(Color);
                VBO.update(V_Line);
                VBO.update(Line_Color);
                VBO_Centres.update(Centres);
                VBO_Flat_Norm.update(Flat_Normals);
                VBO_Phong_Norm.update(Phong_Normals);
                break;
            
            case GLFW_KEY_2:
                for(i = 0; i < Bumpy_vert.size(); i+=3)
                {
                    drawBumpyTriangle(Bumpy_vert[i].x, Bumpy_vert[i].y, Bumpy_vert[i].z,
                                      Bumpy_vert[i+1].x, Bumpy_vert[i+1].y, Bumpy_vert[i+1].z,
                                      Bumpy_vert[i+2].x, Bumpy_vert[i+2].y, Bumpy_vert[i+2].z);

                    j = Bumpy_Centres.size();
                    if(j == 1) {
                        Bumpy_Centres.resize(0);
                        Bumpy_Flat_Normals.resize(0);
                        Bumpy_Phong_Normals.resize(0);
                        Bumpy_Line_Centres.resize(0);
                        j = Bumpy_Centres.size();
                    }
                    Bumpy_Centres.resize(j+3);
                    Bumpy_Flat_Normals.resize(j+3);
                    Bumpy_Phong_Normals.resize(j+3);
                    Bumpy_Line_Centres.resize(j+3);

                    Bumpy_Centres[j]   = glm::vec3(0.0, 0.0, 0.0);
                    Bumpy_Centres[j+1] = glm::vec3(0.0, 0.0, 0.0);
                    Bumpy_Centres[j+2] = glm::vec3(0.0, 0.0, 0.0);

                    Bumpy_Line_Centres[j]   = glm::vec3(0.0, 0.0, 0.0);
                    Bumpy_Line_Centres[j+1] = glm::vec3(0.0, 0.0, 0.0);
                    Bumpy_Line_Centres[j+2] = glm::vec3(0.0, 0.0, 0.0);

                    Bumpy_Flat_Normals[j]   = glm::vec3(Bumpy_Flat_Store[i].x, Bumpy_Flat_Store[i].y, Bumpy_Flat_Store[i].z);
                    Bumpy_Flat_Normals[j+1] = glm::vec3(Bumpy_Flat_Store[i+1].x, Bumpy_Flat_Store[i+1].y, Bumpy_Flat_Store[i+1].z);
                    Bumpy_Flat_Normals[j+2] = glm::vec3(Bumpy_Flat_Store[i+2].x, Bumpy_Flat_Store[i+2].y, Bumpy_Flat_Store[i+2].z);

                    Bumpy_Phong_Normals[j]   = glm::vec3(Bumpy_Smooth_Store[i].x, Bumpy_Smooth_Store[i].y, Bumpy_Smooth_Store[i].z);
                    Bumpy_Phong_Normals[j+1] = glm::vec3(Bumpy_Smooth_Store[i+1].x, Bumpy_Smooth_Store[i+1].y, Bumpy_Smooth_Store[i+1].z);
                    Bumpy_Phong_Normals[j+2] = glm::vec3(Bumpy_Smooth_Store[i+2].x, Bumpy_Smooth_Store[i+2].y, Bumpy_Smooth_Store[i+2].z);
                }
                Next_selBumpy();
                break;
                
                case GLFW_KEY_3:
                for(i = 0; i < Bunny_vert.size(); i+=3)
                {
                    drawBunnyTriangle(Bunny_vert[i].x, Bunny_vert[i].y, Bunny_vert[i].z,
                                      Bunny_vert[i+1].x, Bunny_vert[i+1].y, Bunny_vert[i+1].z,
                                      Bunny_vert[i+2].x, Bunny_vert[i+2].y, Bunny_vert[i+2].z);

                    j = Bunny_Centres.size();
                    
                    if(j == 1) {
                        Bunny_Centres.resize(0);
                        Bunny_Flat_Normals.resize(0);
                        Bunny_Phong_Normals.resize(0);
                        Bunny_Line_Centres.resize(0);
                        j = Bunny_Centres.size();
                    }
                    
                    Bunny_Centres.resize(j+3);
                    Bunny_Flat_Normals.resize(j+3);
                    Bunny_Phong_Normals.resize(j+3);
                    Bunny_Line_Centres.resize(j+3);

                    Bunny_Centres[j]   = glm::vec3(0.0, 0.0, 0.0);
                    Bunny_Centres[j+1] = glm::vec3(0.0, 0.0, 0.0);
                    Bunny_Centres[j+2] = glm::vec3(0.0, 0.0, 0.0);

                    Bunny_Line_Centres[j]   = glm::vec3(0.0, 0.0, 0.0);
                    Bunny_Line_Centres[j+1] = glm::vec3(0.0, 0.0, 0.0);
                    Bunny_Line_Centres[j+2] = glm::vec3(0.0, 0.0, 0.0);

                    Bunny_Flat_Normals[j]   = glm::vec3(Bunny_Flat_store[i].x, Bunny_Flat_store[i].y, Bunny_Flat_store[i].z);
                    Bunny_Flat_Normals[j+1] = glm::vec3(Bunny_Flat_store[i+1].x, Bunny_Flat_store[i+1].y, Bunny_Flat_store[i+1].z);
                    Bunny_Flat_Normals[j+2] = glm::vec3(Bunny_Flat_store[i+2].x, Bunny_Flat_store[i+2].y, Bunny_Flat_store[i+2].z);

                    Bunny_Phong_Normals[j]   = glm::vec3(Bunny_Phong_store[i].x, Bunny_Phong_store[i].y, Bunny_Phong_store[i].z);
                    Bunny_Phong_Normals[j+1] = glm::vec3(Bunny_Phong_store[i+1].x, Bunny_Phong_store[i+1].y, Bunny_Phong_store[i+1].z);
                    Bunny_Phong_Normals[j+2] = glm::vec3(Bunny_Phong_store[i+2].x, Bunny_Phong_store[i+2].y, Bunny_Phong_store[i+2].z);
                }
                Next_selBunny();
                break;

            case GLFW_KEY_G:
                if(selection == 0)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Rotations[i] = glm::vec3(Rotations[i].x+0.2, Rotations[i].y, Rotations[i].z);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Line_Rotations[i] = glm::vec3(Line_Rotations[i].x+0.2, Line_Rotations[i].y, Line_Rotations[i].z);
                    }
                }
                if(selection == 2)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bumpy_Rotations[i] = glm::vec3(Bumpy_Rotations[i].x+0.2, Bumpy_Rotations[i].y, Bumpy_Rotations[i].z);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bumpy_Line_Rotations[i] = glm::vec3(Bumpy_Line_Rotations[i].x+0.2, Bumpy_Line_Rotations[i].y, Bumpy_Line_Rotations[i].z);
                    }
                }
                if(selection == 1)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bunny_Rotations[i] = glm::vec3(Bunny_Rotations[i].x+0.2, Bunny_Rotations[i].y, Bunny_Rotations[i].z);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bunny_Line_Rotations[i] = glm::vec3(Bunny_Line_Rotations[i].x+0.2, Bunny_Line_Rotations[i].y, Bunny_Line_Rotations[i].z);
                    }
                }
                break;

            case GLFW_KEY_H:
                if(selection == 0)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Rotations[i] = glm::vec3(Rotations[i].x, Rotations[i].y + 0.2, Rotations[i].z);  
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Line_Rotations[i] = glm::vec3(Line_Rotations[i].x, Line_Rotations[i].y + 0.2, Line_Rotations[i].z);
                    }
                }
                if(selection == 2)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bumpy_Rotations[i] = glm::vec3(Bumpy_Rotations[i].x, Bumpy_Rotations[i].y + 0.2, Bumpy_Rotations[i].z);  
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bumpy_Line_Rotations[i] = glm::vec3(Bumpy_Line_Rotations[i].x, Bumpy_Line_Rotations[i].y + 0.2, Bumpy_Line_Rotations[i].z);
                    }
                }
                if(selection == 1)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bunny_Rotations[i] = glm::vec3(Bunny_Rotations[i].x, Bunny_Rotations[i].y + 0.2, Bunny_Rotations[i].z);  
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bunny_Line_Rotations[i] = glm::vec3(Bunny_Line_Rotations[i].x, Bunny_Line_Rotations[i].y + 0.2, Bunny_Line_Rotations[i].z);
                    }
                }
                break;

            case GLFW_KEY_J:
                if(selection == 0)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Rotations[i] = glm::vec3(Rotations[i].x, Rotations[i].y, Rotations[i].z + 0.2);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Line_Rotations[i] = glm::vec3(Line_Rotations[i].x, Line_Rotations[i].y, Line_Rotations[i].z + 0.2);
                    }
                }
                if(selection == 2)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bumpy_Rotations[i] = glm::vec3(Bumpy_Rotations[i].x, Bumpy_Rotations[i].y, Bumpy_Rotations[i].z + 0.2);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bumpy_Line_Rotations[i] = glm::vec3(Bumpy_Line_Rotations[i].x, Bumpy_Line_Rotations[i].y, Bumpy_Line_Rotations[i].z + 0.2);
                    }
                }
                if(selection == 1)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bunny_Rotations[i] = glm::vec3(Bunny_Rotations[i].x, Bunny_Rotations[i].y, Bunny_Rotations[i].z + 0.2);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bunny_Line_Rotations[i] = glm::vec3(Bunny_Line_Rotations[i].x, Bunny_Line_Rotations[i].y, Bunny_Line_Rotations[i].z + 0.2);
                    }
                }
                break;

            case GLFW_KEY_RIGHT:
                if(selection == 0)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Translations[i] = glm::vec3(Translations[i].x+0.2, Translations[i].y, Translations[i].z);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Line_Translations[i] = glm::vec3(Line_Translations[i].x+0.2, Line_Translations[i].y, Line_Translations[i].z);
                    }
                }
                if(selection == 2)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bumpy_Translations[i] = glm::vec3(Bumpy_Translations[i].x+0.2, Bumpy_Translations[i].y, Bumpy_Translations[i].z);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bumpy_Line_Translations[i] = glm::vec3(Bumpy_Line_Translations[i].x+0.2, Bumpy_Line_Translations[i].y, Bumpy_Line_Translations[i].z);
                    }
                }
                if(selection == 1)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bunny_Translations[i] = glm::vec3(Bunny_Translations[i].x+0.2, Bunny_Translations[i].y, Bunny_Translations[i].z);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bunny_Line_Translations[i] = glm::vec3(Bunny_Line_Translations[i].x+0.2, Bunny_Line_Translations[i].y, Bunny_Line_Translations[i].z);
                    }
                }
                
                break;

            case GLFW_KEY_LEFT:
                if(selection == 0)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Translations[i] = glm::vec3(Translations[i].x-0.2, Translations[i].y, Translations[i].z);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Line_Translations[i] = glm::vec3(Line_Translations[i].x-0.2, Line_Translations[i].y, Line_Translations[i].z);
                    }
                }
                if(selection == 2)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bumpy_Translations[i] = glm::vec3(Bumpy_Translations[i].x-0.2, Bumpy_Translations[i].y, Bumpy_Translations[i].z);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bumpy_Line_Translations[i] = glm::vec3(Bumpy_Line_Translations[i].x-0.2, Bumpy_Line_Translations[i].y, Bumpy_Line_Translations[i].z);
                    }
                }
                if(selection == 1)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bunny_Translations[i] = glm::vec3(Bunny_Translations[i].x-0.2, Bunny_Translations[i].y, Bunny_Translations[i].z);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bunny_Line_Translations[i] = glm::vec3(Bunny_Line_Translations[i].x-0.2, Bunny_Line_Translations[i].y, Bunny_Line_Translations[i].z);
                    }
                }
                break;

            case GLFW_KEY_UP:
                if(selection == 0)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Translations[i] = glm::vec3(Translations[i].x, Translations[i].y+0.2, Translations[i].z);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Line_Translations[i] = glm::vec3(Line_Translations[i].x, Line_Translations[i].y+0.2, Line_Translations[i].z);
                    }
                }
                if(selection == 2)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bumpy_Translations[i] = glm::vec3(Bumpy_Translations[i].x, Bumpy_Translations[i].y+0.2, Bumpy_Translations[i].z);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bumpy_Line_Translations[i] = glm::vec3(Bumpy_Line_Translations[i].x, Bumpy_Line_Translations[i].y+0.2, Bumpy_Line_Translations[i].z);
                    }
                }
                if(selection == 1)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bunny_Translations[i] = glm::vec3(Bunny_Translations[i].x, Bunny_Translations[i].y+0.2, Bunny_Translations[i].z);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bunny_Line_Translations[i] = glm::vec3(Bunny_Line_Translations[i].x, Bunny_Line_Translations[i].y+0.2, Bunny_Line_Translations[i].z);
                    }
                }
                break;

            case GLFW_KEY_DOWN:
                if(selection == 0)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Translations[i] = glm::vec3(Translations[i].x, Translations[i].y-0.2, Translations[i].z);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Line_Translations[i] = glm::vec3(Line_Translations[i].x, Line_Translations[i].y-0.2, Line_Translations[i].z);
                    }
                }
                if(selection == 2)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bumpy_Translations[i] = glm::vec3(Bumpy_Translations[i].x, Bumpy_Translations[i].y-0.2, Bumpy_Translations[i].z);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bumpy_Line_Translations[i] = glm::vec3(Bumpy_Line_Translations[i].x, Bumpy_Line_Translations[i].y-0.2, Bumpy_Line_Translations[i].z);
                    }
                }
                if(selection == 1)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bunny_Translations[i] = glm::vec3(Bunny_Translations[i].x, Bunny_Translations[i].y-0.2, Bunny_Translations[i].z);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bunny_Line_Translations[i] = glm::vec3(Bunny_Line_Translations[i].x, Bunny_Line_Translations[i].y-0.2, Bunny_Line_Translations[i].z);
                    }
                }
                break;

            case GLFW_KEY_K:
                if(selection == 0)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Scalings[i] = glm::vec3(Scalings[i].x*1.2, Scalings[i].y*1.2, Scalings[i].z*1.2);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Line_Scalings[i] = glm::vec3(Line_Scalings[i].x*1.2, Line_Scalings[i].y*1.2, Line_Scalings[i].z*1.2);
                    }
                }                
                if(selection == 2)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bumpy_Scalings[i] = glm::vec3(Bumpy_Scalings[i].x*1.2, Bumpy_Scalings[i].y*1.2, Bumpy_Scalings[i].z*1.2);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bumpy_Line_Scalings[i] = glm::vec3(Bumpy_Line_Scalings[i].x*1.2, Bumpy_Line_Scalings[i].y*1.2, Bumpy_Line_Scalings[i].z*1.2);
                    }
                }
                if(selection == 1)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bunny_Scalings[i] = glm::vec3(Bunny_Scalings[i].x*1.2, Bunny_Scalings[i].y*1.2, Bunny_Scalings[i].z*1.2);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bunny_Line_Scalings[i] = glm::vec3(Bunny_Line_Scalings[i].x*1.2, Bunny_Line_Scalings[i].y*1.2, Bunny_Line_Scalings[i].z*1.2);
                    }
                }
                break;

            case GLFW_KEY_L:
                if(selection == 0)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Scalings[i] = glm::vec3(Scalings[i].x*0.8, Scalings[i].y*0.8, Scalings[i].z*0.8);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Line_Scalings[i] = glm::vec3(Line_Scalings[i].x*0.8, Line_Scalings[i].y*0.8, Line_Scalings[i].z*0.8);
                    }
                }
                if(selection == 2)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bumpy_Scalings[i] = glm::vec3(Bumpy_Scalings[i].x*0.8, Bumpy_Scalings[i].y*0.8, Bumpy_Scalings[i].z*0.8);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bumpy_Line_Scalings[i] = glm::vec3(Bumpy_Line_Scalings[i].x*0.8, Bumpy_Line_Scalings[i].y*0.8, Bumpy_Line_Scalings[i].z*0.8);
                    }
                }
                if(selection == 1)
                {
                    for(i = V_select_start; i < V_select_end; i++)
                    {
                        Bunny_Scalings[i] = glm::vec3(Bunny_Scalings[i].x*0.8, Bunny_Scalings[i].y*0.8, Bunny_Scalings[i].z*0.8);
                    }
                    for(i = Line_select_start; i < Line_select_end; i++)
                    {
                        Bunny_Line_Scalings[i] = glm::vec3(Bunny_Line_Scalings[i].x*0.8, Bunny_Line_Scalings[i].y*0.8, Bunny_Line_Scalings[i].z*0.8);
                    }
                }
                break;

            case GLFW_KEY_P:
                if(selection == 0)
                {
                    if(selected_index > -1)
                    {
                        for(i = V_select_start; i < V_select_end; i++)
                        {
                            Color[i] = glm::vec4(Color[i].x, Color[i].y, Color[i].z, 0.8);
                        }
                        for(i = Line_select_start; i < Line_select_end; i++)
                        {
                            Line_Color[i] = glm::vec4(0.0, 0.0, 0.0, 1.0);
                        }
                    }
                }
                if(selection == 2)
                {
                    if(selection_Bumpy > -1)
                    {
                        for(i = V_select_start; i < V_select_end; i++)
                        {
                            Color_Bumpy[i] = glm::vec4(Color_Bumpy[i].x, Color_Bumpy[i].y, Color_Bumpy[i].z, 0.8);
                        }
                        for(i = Line_select_start; i < Line_select_end; i++)
                        {
                            Lines_Color_Bumpy[i] = glm::vec4(0.0, 0.0, 0.0, 1.0);
                        }
                    }
                }
                if(selection == 1)
                {
                    if(selection_Bunny > -1)
                    {
                        for(i = V_select_start; i < V_select_end; i++)
                        {
                            Color_bunny[i] = glm::vec4(Color_bunny[i].x, Color_bunny[i].y, Color_bunny[i].z, 0.8);
                        }
                        for(i = Line_select_start; i < Line_select_end; i++)
                        {
                            Lines_color_bunny[i] = glm::vec4(0.0, 0.0, 0.0, 1.0);
                        }
                    }
                }
                break;
            
            case GLFW_KEY_O:
                if(selection == 0)
                {
                    if(selected_index > -1)
                    {
                        for(i = V_select_start; i < V_select_end; i++)
                        {
                            Color[i] = glm::vec4(Color[i].x, Color[i].y, Color[i].z, 0.0);
                        }
                        for(i = Line_select_start; i < Line_select_end; i++)
                        {
                            Line_Color[i] = glm::vec4(0.0, 0.0, 0.0, 1.0);
                        }
                    }
                }
                if(selection == 2)
                {
                    if(selection_Bumpy > -1)
                    {
                        for(i = V_select_start; i < V_select_end; i++)
                        {
                            Color_Bumpy[i] = glm::vec4(Color_Bumpy[i].x, Color_Bumpy[i].y, Color_Bumpy[i].z, 0.0);
                        }
                        for(i = Line_select_start; i < Line_select_end; i++)
                        {
                            Lines_Color_Bumpy[i] = glm::vec4(0.0, 0.0, 0.0, 1.0);
                        }
                    }
                }
                if(selection == 1)
                {
                    if(selection_Bunny > -1)
                    {
                        for(i = V_select_start; i < V_select_end; i++)
                        {
                            Color_bunny[i] = glm::vec4(Color_bunny[i].x, Color_bunny[i].y, Color_bunny[i].z, 0.0);
                        }
                        for(i = Line_select_start; i < Line_select_end; i++)
                        {
                            Lines_color_bunny[i] = glm::vec4(0.0, 0.0, 0.0, 1.0);
                        }
                    }
                }
                break;

            case GLFW_KEY_I:
                if(selection == 0)
                {
                    if(selected_index > -1)
                    {
                        for(i = V_select_start; i < V_select_end; i++)
                        {
                            Color[i] = glm::vec4(Color[i].x, Color[i].y, Color[i].z, 0.5);
                        }
                        for(i = Line_select_start; i < Line_select_end; i++)
                        {
                            Line_Color[i] = glm::vec4(0.0, 0.0, 0.0, 0.0);
                        }
                    }
                }
                if(selection == 2)
                {
                    if(selection_Bumpy > -1)
                    {
                        for(i = V_select_start; i < V_select_end; i++)
                        {
                            Color_Bumpy[i] = glm::vec4(Color_Bumpy[i].x, Color_Bumpy[i].y, Color_Bumpy[i].z, 0.5);
                        }
                        for(i = Line_select_start; i < Line_select_end; i++)
                        {
                            Lines_Color_Bumpy[i] = glm::vec4(0.0, 0.0, 0.0, 0.0);
                        }
                    }
                }
                if(selection == 1)
                {
                    if(selection_Bunny > -1)
                    {
                        for(i = V_select_start; i < V_select_end; i++)
                        {
                            Color_bunny[i] = glm::vec4(Color_bunny[i].x, Color_bunny[i].y, Color_bunny[i].z, 0.5);
                        }
                        for(i = Line_select_start; i < Line_select_end; i++)
                        {
                            Lines_color_bunny[i] = glm::vec4(0.0, 0.0, 0.0, 0.0);
                        }
                    }
                }
                break;
            case GLFW_KEY_Q:
                deleteSelected();
                break;

            case GLFW_KEY_Z:
                Next_selBunny();
                break;

            case GLFW_KEY_X:
                Next_sel();
                break;

            case GLFW_KEY_C:
                Next_selBumpy();
                break;
            default:
               break;
        }
    }
    VBO.update(V);
    VBO_Color.update(Color);
    VBO_Centres.update(Centres);
    VBO_Translations.update(Translations);
    VBO_Rotations.update(Rotations);
    VBO_Scalings.update(Scalings);
    VBO_Flat_Norm.update(Flat_Normals);
    VBO_Phong_Norm.update(Phong_Normals);

    VBO.update(V_Line);
    VBO_Color.update(Line_Color);
    VBO_Centres.update(Line_Centres);
    VBO_Translations.update(Line_Translations);
    VBO_Rotations.update(Line_Rotations);
    VBO_Scalings.update(Line_Scalings);
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


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void renderScene(const Program &prgm)
{
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(prgm.uniform("model"),1,GL_FALSE,&model[0][0]);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    GLFWwindow* window;
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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glEnable(GL_DEPTH_TEST);

    #ifndef __APPLE__
      glewExperimental = true;
      GLenum err = glewInit();
      if(GLEW_OK != err)
      {
       fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
      }
      glGetError(); 
      fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    #endif

    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
    printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
    printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    VertexArrayObject VAO;
    VAO.init();
    VAO.bind();

    VBO.init();
    VBO_Color.init();
    VBO_Centres.init();
    VBO_Translations.init();
    VBO_Rotations.init();
    VBO_Scalings.init();
    VBO_Flat_Norm.init();
    VBO_Phong_Norm.init();

    VBO.update(V);
    VBO_Color.update(Color);
    VBO_Centres.update(Centres);
    VBO_Translations.update(Translations);
    VBO_Rotations.update(Rotations);
    VBO_Scalings.update(Scalings);
    VBO_Flat_Norm.update(Flat_Normals);
    VBO_Phong_Norm.update(Phong_Normals);

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

    Program program, program_env, program_shadow, program_depth, program_quad;

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

    const GLchar* vertex_shader =
            "#version 150 core\n"
                    "in vec4 position;"
                    "in vec4 color;"
                    "in vec3 centroids;"
                    "in vec3 Rotate_val;"
                    "in vec3 Translate_val;"
                    "in vec3 Scale_val;"
                    "in vec3 FlatNormals;"
                    "in vec3 SmoothNormals;"
                    "uniform mat4 projection;"
                    "uniform vec3 vPosition;"
                    "uniform mat4 view;"
                    "out vec4 colorOut;"
                    
                    "mat4 positionMatrix = mat4("
                        "position[1], position[2], position[3], 1.0,"
                        "FlatNormals[0], FlatNormals[1], FlatNormals[2], 0.0,"
                        "SmoothNormals[0], SmoothNormals[1], SmoothNormals[2], 0.0,"
                        "0.0, 0.0, 0.0, 1.0"
                    ");"

                    "mat4 translation1 = mat4("
                    "1.0, 0.0, 0.0, 0.0,"
                    "0.0, 1.0, 0.0, 0.0,"
                    "0.0, 0.0, 1.0, 0.0,"
                    "-centroids[0], -centroids[1], -centroids[2], 1.0"
                    ");"

                    "mat4 rotationXY = mat4("
                    "cos(Rotate_val[0]), -sin(Rotate_val[0]), 0.0, 0.0,"
                    "sin(Rotate_val[0]), cos(Rotate_val[0]), 0.0, 0.0,"
                    "0.0, 0.0, 1.0, 0.0,"
                    "0.0, 0.0, 0.0, 1.0"
                    ");"

                    "mat4 rotationYZ = mat4("
                    "1.0, 0.0, 0.0, 0.0,"  
                    "0.0, cos(Rotate_val[1]), -sin(Rotate_val[1]), 0.0,"
                    "0.0, sin(Rotate_val[1]), cos(Rotate_val[1]), 0.0,"      
                    "0.0, 0.0, 0.0, 1.0"                                                            
                    ");"

                    "mat4 rotationXZ = mat4("
                    "cos(Rotate_val[2]), 0.0, sin(Rotate_val[2]) ,0.0,"
                    "0.0, 1.0, 0.0, 0.0,"
                    "-sin(Rotate_val[2]), 0.0 ,cos(Rotate_val[2]), 0.0,"
                    "0.0, 0.0, 0.0, 1.0"
                    ");"

                    "mat4 scaling = mat4("
                    "Scale_val[0], 0.0, 0.0, 0.0,"
                    "0.0, Scale_val[1], 0.0, 0.0,"
                    "0.0, 0.0, Scale_val[2], 0.0,"
                    "0.0, 0.0, 0.0, 1.0"
                    ");"
                   
                    "mat4 translation2 = mat4("
                    "1.0, 0.0, 0.0, 0.0,"
                    "0.0, 1.0, 0.0, 0.0,"
                    "0.0, 0.0, 1.0, 0.0,"
                    "centroids[0]+Translate_val[0], centroids[1]+Translate_val[1], centroids[2]+Translate_val[2], 1.0"
                    ");"

                    "mat4 resultMatrix = mat4(translation2 * scaling * rotationXZ * rotationYZ  * rotationXY * translation1 * positionMatrix);"
                   
                    "vec3 lightSource = vec3(0.0,-1.5,0.0);"
                    "float amb = 0.3;"
                    "vec4 colorShaded = vec4(0.0, 0.0, 0.0, 0.0);"

                    "void main()"
                    "{"
                    "   if(color[3] > 0.9){" 
                    "       colorShaded = vec4(color[0], color[1], color[2], 1.0);"
                    "   }"
                    "   else if(color[3] > 0.75){" 
                    "       vec3 iVec = vec3(normalize(vec3(resultMatrix[0][0], resultMatrix[0][1], resultMatrix[0][2]) - lightSource));"
                    "       vec3 nVec = vec3(normalize(vec3(resultMatrix[1][0], resultMatrix[1][1], resultMatrix[1][2])));"
                    "       float x = dot(iVec,nVec);"
                    "       colorShaded = vec4(amb + (color[0] * x) , amb + (color[1] * x), amb + (color[2] * x), 1.0);"
                    "   }"
                    "   else if(color[3] > 0.4){" 
                    "       vec3 iVec = vec3(normalize(vec3(resultMatrix[0][0], resultMatrix[0][1], resultMatrix[0][2]) - lightSource));"
                    "       vec3 nVec = vec3(normalize(vec3(resultMatrix[2][0], resultMatrix[2][1], resultMatrix[2][2])));"
                    "       float x = dot(iVec,nVec);"
                    "       colorShaded = vec4(amb + (color[0] * x) , amb + (color[1] * x), amb + (color[2] * x), 1.0);"
                    "   }"
                    "   else {" 
                    "       colorShaded = vec4(0.0, 0.0, 0.0, 0.0);"
                    "   }"
                    "   colorOut = colorShaded;"
                    "   gl_Position = vec4(resultMatrix[0][0], resultMatrix[0][1], resultMatrix[0][2], 1.0);"
                    "}";

    const GLchar* fragment_shader =
            "#version 150 core\n"
                    "in vec4 colorOut;"
                    "out vec4 outColor;"
                    "void main()"
                    "{"
                    "    if(colorOut.a < 0.1){"
                    "       discard;"
                    "    }"
                    "    outColor = colorOut;"
                    "}";

    const GLchar* shadow_vertex_shader =
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

    const GLchar* shadow_fragment_shader =
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

    program.init(vertex_shader,fragment_shader,"outColor");
    program.bind();
    program.bindVertexAttribArray("position",VBO);
    program.bindVertexAttribArray("color",VBO_Color);
    program.bindVertexAttribArray("centroids",VBO_Centres);
    program.bindVertexAttribArray("Translate_val",VBO_Translations);
    program.bindVertexAttribArray("Rotate_val",VBO_Rotations);
    program.bindVertexAttribArray("Scale_val",VBO_Scalings);
    program.bindVertexAttribArray("FlatNormals",VBO_Flat_Norm);
    program.bindVertexAttribArray("SmoothNormals",VBO_Phong_Norm);

    program_env.init(env_vertex_shader, env_fragment_shader, "FragColor");
    program_env.bind();
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    program_shadow.init(shadow_vertex_shader, shadow_fragment_shader, "FragColor");
    program_shadow.bind();

    program_depth.init(depth_vertex_shader, depth_fragment_shader, "");
    program_depth.bind();

    program_quad.init(quad_vertex_shader, quad_fragment_shader, "FragColor");
    program_quad.bind();

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

    float planeVertices[] = {
         2.0f, -0.5f,  2.0f,  0.0f, 1.0f, 0.0f,  2.0f,  0.0f,
        -2.0f, -0.5f,  2.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -2.0f, -0.5f, -2.0f,  0.0f, 1.0f, 0.0f,   0.0f, 2.0f,

         2.0f, -0.5f,  2.0f,  0.0f, 1.0f, 0.0f,  2.0f,  0.0f,
        -2.0f, -0.5f, -2.0f,  0.0f, 1.0f, 0.0f,   0.0f, 2.0f,
         2.0f, -0.5f, -2.0f,  0.0f, 1.0f, 0.0f,  2.0f, 2.0f
    };
    
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

    auto t_start = std::chrono::high_resolution_clock::now();
    glfwSetKeyCallback(window, key_callback);

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(5);

    initializeBunny();
    initializeBumpy();

    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 780;
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

    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f); 

    while (!glfwWindowShouldClose(window))
    {
        auto t_now = std::chrono::high_resolution_clock::now();
        timeGlobal = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        int width, height;

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if(state!=0){
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            glfwGetWindowSize(window, &width, &height);
            double xworld = ((((xpos/double(width))*2)-1)-X_shift)/View_Scale;
            double yworld = (((((height-1-ypos)/double(height))*2)-1)-Y_shift)/View_Scale; // NOTE: y axis is flipped in glfw   
        }
        
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

        glm::mat4 shadow_projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 shadow_view = camera.GetViewMatrix();
        program_shadow.bind();
        glUniformMatrix4fv(program_shadow.uniform("view"),1,GL_FALSE,&shadow_view[0][0]);
        glUniformMatrix4fv(program_shadow.uniform("projection"),1,GL_FALSE,&shadow_projection[0][0]);

        glUniform3f(program_shadow.uniform("viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        glUniform3f(program_shadow.uniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniformMatrix4fv(program_shadow.uniform("lightSpaceMatrix"),1,GL_FALSE,&lightSpaceMatrix[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderScene(program_shadow); 

        program_quad.bind();
        
        GLint plane_loc = program_quad.uniform("near_plane");
        GLint far_plane_loc = program_quad.uniform("far_plane");

        if(plane_loc != -1)
            glUniform1f(plane_loc, near_plane);
        if(far_plane_loc != -1)
            glUniform1f(far_plane_loc, far_plane);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        VAO.bind();

        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST);
        program.bind();
        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        VBO.update(V);
        VBO_Color.update(Color);
        VBO_Centres.update(Centres);
        VBO_Translations.update(Translations);
        VBO_Rotations.update(Rotations);
        VBO_Scalings.update(Scalings);
        VBO_Flat_Norm.update(Flat_Normals);
        VBO_Phong_Norm.update(Phong_Normals);
        glDrawArrays(GL_TRIANGLES, 0, V.size());
       
        VBO.update(V_Line);
        VBO_Color.update(Line_Color);
        VBO_Centres.update(Line_Centres);
        VBO_Translations.update(Line_Translations);
        VBO_Rotations.update(Line_Rotations);
        VBO_Scalings.update(Line_Scalings);
        VBO_Flat_Norm.update(Line_Centres); 
        VBO_Phong_Norm.update(Line_Centres);
        glDrawArrays(GL_LINES, 0, V_Line.size());
       
        VBO.update(V_Bunny);
        VBO_Color.update(Color_bunny);
        VBO_Translations.update(Bunny_Translations);
        VBO_Rotations.update(Bunny_Rotations);
        VBO_Scalings.update(Bunny_Scalings);
        VBO_Centres.update(Bunny_Centres);
        VBO_Flat_Norm.update(Bunny_Flat_Normals);
        VBO_Phong_Norm.update(Bunny_Phong_Normals);
        glDrawArrays(GL_TRIANGLES, 0, V_Bunny.size());
       
        VBO.update(Lines_bunny);
        VBO_Color.update(Lines_color_bunny);
        VBO_Centres.update(Bunny_Line_Centres);
        VBO_Translations.update(Bunny_Line_Translations);
        VBO_Rotations.update(Bunny_Line_Rotations);
        VBO_Scalings.update(Bunny_Line_Scalings);
        VBO_Flat_Norm.update(Bunny_Line_Centres); 
        VBO_Phong_Norm.update(Bunny_Line_Centres); 
        glDrawArrays(GL_LINES, 0, Lines_bunny.size());

        VBO.update(V_Bumpy);
        VBO_Color.update(Color_Bumpy);
        VBO_Translations.update(Bumpy_Translations);
        VBO_Rotations.update(Bumpy_Rotations);
        VBO_Scalings.update(Bumpy_Scalings);
        VBO_Centres.update(Bumpy_Centres);
        VBO_Flat_Norm.update(Bumpy_Flat_Normals);
        VBO_Phong_Norm.update(Bumpy_Phong_Normals);
        glDrawArrays(GL_TRIANGLES, 0, V_Bumpy.size());
       
        VBO.update(Lines_Bumpy);
        VBO_Color.update(Lines_Color_Bumpy);
        VBO_Centres.update(Bumpy_Line_Centres);
        VBO_Translations.update(Bumpy_Line_Translations);
        VBO_Rotations.update(Bumpy_Line_Rotations);
        VBO_Scalings.update(Bumpy_Line_Scalings);
        VBO_Flat_Norm.update(Bumpy_Line_Centres); 
        VBO_Phong_Norm.update(Bumpy_Line_Centres); 
        glDrawArrays(GL_LINES, 0, Lines_Bumpy.size());

        
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glDepthFunc(GL_LEQUAL);  
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); 

        program_env.bind();

        glUniformMatrix4fv(program.uniform("view"),1,GL_FALSE,&view[0][0]);
        glUniformMatrix4fv(program.uniform("projection"),1,GL_FALSE,&projection[0][0]);
        
        glUniformMatrix4fv(program_env.uniform("view"),1,GL_FALSE,&view[0][0]);
        glUniformMatrix4fv(program_env.uniform("projection"),1,GL_FALSE,&projection[0][0]);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LEQUAL);

        glfwSwapBuffers(window);
        glfwPollEvents();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    }
    program.free();
    program_env.free();
    VAO.free();
    VBO.free();
    VBO_Color.free();
    VBO_Centres.free();
    VBO_Translations.free();
    VBO_Rotations.free();
    VBO_Scalings.free();
    VBO_Flat_Norm.free();
    VBO_Phong_Norm.free();

    glfwTerminate();
    return 0;
}*/