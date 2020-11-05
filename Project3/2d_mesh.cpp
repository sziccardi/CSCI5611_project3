#include "2d_mesh.h"

Mesh2D::Mesh2D(vector<Vertex> vertices, vector<unsigned int> inds, char const* texture) {
    mVerts = vertices;
    mIndices = inds;

    setupMesh();
    mTexture = loadTexture(texture);
}

void Mesh2D::draw() {
    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexture);

    // draw mesh
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mVerts.size() * sizeof(Vertex), &mVerts[0], GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Vertex Mesh2D::getVertAt(int iter) {
    if (iter < mVerts.size() && iter >= 0) {
        return mVerts[iter];
    }
    cout << "YOU TRIED TO GET A VERT AT AN ITER THAT DOESN'T EXIST" << endl;
    return Vertex();
}

void Mesh2D::setVertAt(int iter, Vertex value) {
    if (iter < mVerts.size() && iter >= 0) {
        auto prevPos = mVerts[iter].mPosition;
        mVerts[iter].mAcceleration = value.mAcceleration;
        mVerts[iter].mVelocity = value.mVelocity;
        mVerts[iter].mNormal = value.mNormal;
        mVerts[iter].mTexCoord = value.mTexCoord;
        mVerts[iter].mPosition = value.mPosition;

        /* if (iter == 65) {
             cout << "(" << prevPos.x << ", " << prevPos.y << ", " << prevPos.z << ") - (";
             cout << mVerts[iter].mPosition.x << ", " << mVerts[iter].mPosition.y << ", " << mVerts[iter].mPosition.z << ")" << endl;
         }*/
    }
    else {
        cout << "YOU TRIED TO SET A VERT AT AN ITER THAT DOESN'T EXIST" << endl;
    }
}

void Mesh2D::updateVerts(glm::vec3 diff) {
    for (int i = 0; i < mVerts.size(); i++) {
        mVerts[i].mPosition += diff;
    }
}

void Mesh2D::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glCheckError();
    glGenBuffers(1, &VBO);
    glCheckError();
    glGenBuffers(1, &EBO);
    glCheckError();


    glBindVertexArray(VAO);
    glCheckError();
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glCheckError();

    glBufferData(GL_ARRAY_BUFFER, mVerts.size() * sizeof(Vertex), &mVerts[0], GL_STATIC_DRAW);
    glCheckError();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);


    // vertex positions
    glEnableVertexAttribArray(0);
    glCheckError();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glCheckError();
    // vertex normals
    glEnableVertexAttribArray(1);
    glCheckError();
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mNormal));
    glCheckError();
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glCheckError();
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mTexCoord));
    glCheckError();

    glBindVertexArray(0);
    glCheckError();
}

unsigned int Mesh2D::loadTexture(char const* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glCheckError();

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
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
        glCheckError();
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glCheckError();
        glGenerateMipmap(GL_TEXTURE_2D);
        glCheckError();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glCheckError();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glCheckError();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glCheckError();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glCheckError();

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
