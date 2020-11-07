#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "data_utils.h"

#include "rrtstar.h"
#include "2d_mesh.h"

/* References */
//https://learnopengl.com/Getting-started/Camera
//https://learnopengl.com/Getting-started/Textures
//https://learnopengl.com/code_viewer_gh.php?code=src/4.advanced_opengl/3.2.blending_sort/blending_sorted.cpp
//https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/mesh.h
//https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader.h
//https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h
//https://learnopengl.com/code_viewer_gh.php?code=src/3.model_loading/1.model_loading/model_loading.cpp

#define glCheckError() glCheckError_(__FILE__, __LINE__) 
#define GLM_ENABLE_EXPERIMENTAL

#define M_PI 3.14159265358979323846

/* FPS counting things */
int framesSinceLast = 0;
long lastTimeSecond = 0;
float previousFrame = 0.0f;
float deltaTime = 0.001f;


/* Camera things*/
glm::vec3 cameraPos = glm::vec3(0.0f, 155.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(1.f, -1.0f, 1.f);
glm::vec3 cameraUp = glm::vec3(0.0f, 50.0f, 1.0f);
glm::vec3 cameraRight = glm::vec3(3.0f, 0.0f, 0.0f);
glm::vec2 mouseAngles = glm::vec2(0, 0);

float horizontal = 3.14f;
float vertical = 0.0f;

float cameraDepth = 10000.f;
float cameraSpeed = 8.f;
float cameraSpeedScale = 40.f;
float moveMult = 5.0f;
float mouseSpeed = 0.0f;

/* Input handlers */
unsigned char keyStates[256] = { 0 };

void keyPressed(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void keyOperations(void);

void mouse(int button, int state, int x, int y);
void drag(int x, int y);

bool mMouseDown = false;
glm::vec2 mMousePos = glm::vec2(-1.f, -1.f);

int mShaderProgram;
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"uniform mat4 model; \n"
"uniform mat4 view; \n"
"uniform mat4 projection; \n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"   TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D sampler;\n"
"void main()\n"
"{\n"
"   vec4 texColor = texture(sampler, TexCoord);\n"
"   FragColor = texColor;\n"
"}\n\0";

/* Display things */
void display();

void initGL();
void initShader();

void reshape(GLsizei width, GLsizei height);


/* Animation things */
void animLoop(int val);


/* Debugging things*/
GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        cout << error << " | " << file << " (" << line << ")" << endl;
    }
    return errorCode;
}

/* Environment things */
void buildEnvironment();

float mWidth = 600.f;
float mHeight = 600.f;

/* Floor things */
void buildFloor();
void calculateFloorTransform();

Mesh2D* mFloor = nullptr;
const char* mFloorTexture = "gradientTexture1.jpg";
float mFloorHeight = 300.f;
float mFloorWidth = 300.f;
glm::vec3 mFloorNormal = glm::vec3(0, 1, 0);
glm::mat3 mFloorTransformation = glm::mat3();

/* Obstacle things */
void buildObstacles();

vector<Mesh2D*> mObstacleMeshes;
vector<pair<glm::vec2, float>> mObstacles;
int mNumObstacles = 35;
float mMaxRadius = 12.f;
float mMinRadius = 1.f;
float mObsHeight = 10.f;
const char* mObstacleTexture = "obstacleTexture.jpg";

/* Planning things */
void buildStartEnd();
void buildSolution();
void buildAgent();
void updateAgent(float dt);
void buildRRTStar();

RRTStar* mMyRRTStar = nullptr;
int mNumVerts = 1000;

glm::vec2 mStartPos = glm::vec2(0.f, mFloorHeight/2.f);
glm::vec2 mGoalPos = glm::vec2(mFloorWidth, mFloorHeight/2.f);

Mesh2D* mStartMesh = nullptr;
Mesh2D* mEndMesh = nullptr;
const char* mStartEndTexture = "startEndTexture.png";

Mesh2D* mAgent = nullptr;
float mAgentRadius = 5.f;
float mAgentHeight = 20.f;
glm::vec3 mAgentPos = glm::vec3(0.f, 0.f, 0.f);
glm::vec3 mAimAt = glm::vec3(0.f, 0.f, 0.f);
float mAgentSpeed = 15.f;
const char* mAgentTexture = "gradientTexture.jpg";

float mSolutionMeshThickness = 1.f;
const char* mSolutionTexture = "tealTexture.jpg";
vector<Mesh2D*> mSolutionMeshes;

