#pragma once

#include "vertex.h"

#include "stb_image.h"
#include "data_utils.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

using namespace std;

class Mesh2D {
public:
    Mesh2D(vector<Vertex> vertices, vector<unsigned int> inds, char const* texture);
    void draw();

    vector<Vertex> getVertices() { return mVerts; }
    Vertex getVertAt(int iter);
    void setVertAt(int iter, Vertex value);

    int getNumVerts() { return mVerts.size(); }

protected:
    vector<Vertex> mVerts;
    vector<unsigned int> mIndices;

    unsigned int VAO, VBO, EBO;
    void setupMesh();

    unsigned int loadTexture(char const* path);
    unsigned int mTexture;

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
#define glCheckError() glCheckError_(__FILE__, __LINE__)

};