#include "project3.h"

/*INPUTS*/
void keyPressed(unsigned char key, int x, int y) {
    keyStates[key] = true;
}

void keyUp(unsigned char key, int x, int y) {
    keyStates[key] = false;
}

void keyOperations(void) {
    float dCam = cameraSpeed * deltaTime;
    //int ww = glutGet(GLUT_WINDOW_WIDTH);
    //int wh = glutGet(GLUT_WINDOW_HEIGHT);

    //OutputDebugStringA(cameraFront.toString().c_str());

    float phi = atan2(-cameraFront.z, cameraFront.x);
    float theta = atan2(sqrt(cameraFront.z * cameraFront.z + cameraFront.x * cameraFront.x), cameraFront.y);
    float oldPhi = phi;
    float oldTheta = theta;


    if (keyStates['w']) { // If the 'a' key has been pressed  
        //turn up
        //in spherical, -- theta
        theta -= dCam;
    }

    if (keyStates['a']) { // If the 'a' key has been pressed  
        //turn left
        //in spherical, ++ phi
        phi += dCam;
    }

    if (keyStates['d']) { // If the 'a' key has been pressed  
        //turn right
        //in spherical, -- phi
        phi -= dCam;
    }

    if (keyStates['s']) { // If the 'a' key has been pressed  
        //turn down
        //in spherical, ++ theta
        //TODO: need a limit!?
        theta += dCam;
    }

    float xFront = sin(theta) * cos(phi);
    float zFront = -sin(theta) * sin(phi);
    float yFront = cos(theta);

    cameraFront.x = xFront;
    cameraFront.y = yFront;
    cameraFront.z = zFront;

    cameraFront = normalize(cameraFront);// maybe not necessary

    //up is phi + M_PI / 2
    float xUp = sin(theta - M_PI / 2) * cos(phi);
    float zUp = -sin(theta - M_PI / 2) * sin(phi);
    float yUp = cos(theta - M_PI / 2);

    cameraUp.x = xUp;
    cameraUp.y = yUp;
    cameraUp.z = zUp;

    cameraUp = normalize(cameraUp);// maybe not necessary

    //right is theta + M_PI / 2
    float xRight = sin(theta) * cos(phi + M_PI / 2);
    float zRight = -sin(theta) * sin(phi + M_PI / 2);
    float yRight = cos(theta);

    cameraRight.x = xRight;
    cameraRight.y = yRight;
    cameraRight.z = zRight;

    cameraRight = normalize(cameraRight);// maybe not necessary

    if (keyStates[' ']) {
        cameraPos += cameraFront * dCam * cameraSpeedScale;
    }

    if (keyStates['r']) { // If the 'a' key has been pressed 
        clearObstacles();
        buildEnvironment();
        buildAgents();
    }

    if (keyStates['b']) {
        for (auto agent : mAgents) {
            agent->buildRRTStar(mObstacles);
            agent->buildSolution();
        }
    }

    //cout << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << endl;
}

void mouse(int button, int state, int x, int y) {
    // Wheel reports as button 3(scroll up) and button 4(scroll down)
    if ((button == 3) || (button == 4)) { // It's a wheel event 
        // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
        if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
        std::printf("Scroll %s At %d %d\n", (button == 3) ? "Up" : "Down", x, y);
    }
    else {  // normal button event
        // Mouse click, spawn particles
       
        glm::vec3 newCameraPos = glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z);
        glm::vec3 newCameraFront = glm::vec3(cameraFront.x, cameraFront.y, cameraFront.z);
        glm::vec3 newCameraUp = glm::vec3(cameraUp.x, cameraUp.y, cameraUp.z);
        glm::mat4 view = glm::lookAt(newCameraPos, newCameraPos + newCameraFront, newCameraUp);

        glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, cameraDepth);

        auto matToUse = projection * view;
        auto matInv = glm::inverse(matToUse);

        glm::vec4 val = glm::vec4(1.f);
        val.x = (2.0f * ((float)(x - 0) / ((float)glutGet(GLUT_WINDOW_WIDTH) - 0))) - 1.0f;
        val.y = 1.0f - (2.0f * ((float)(y - 0) / ((float)glutGet(GLUT_WINDOW_HEIGHT) - 0)));
        

        auto actualPos = matInv * val;
        actualPos.x /= actualPos.w;
        actualPos.y /= actualPos.w;
        actualPos.z /= actualPos.w;

        ///floor equation
        //y = 0
        //ray equation
        //pos = vec * t + cameraPos
        float t = -cameraPos.y / actualPos.y;
        glm::vec3 floorPos = glm::vec3(0.f);
        floorPos.x = actualPos.x * t + cameraPos.x;
        floorPos.z = actualPos.z * t + cameraPos.z;

        buildObstacle(mFloorTransformation * floorPos);
        //printf("Button %s At %d %d\n", (state == GLUT_DOWN) ? "Down" : "Up", x, y);
    }
}

void buildEnvironment() {
    calculateFloorTransform();
    buildFloor();
}

void buildFloor() {
    if (mFloor) {
        auto c = mFloor;
        delete(c);
        mFloor = nullptr;
    }

    vector<Vertex> verts;
    glm::vec3 pos = mFloorTransformation * glm::vec3(0.f, 0.f, mFloorHeight);
    glm::vec3 norm = glm::vec3(0.f, 1.f, 0.f);
    glm::vec2 tex = glm::vec2(0.f, 1.f);
    glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mFloorWidth, 0.f, mFloorHeight);
    norm = glm::vec3(0.f, 1.f, 0.f);
    tex = glm::vec2(1.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mFloorWidth, 0.f, 0.f);
    norm = glm::vec3(0.f, 1.f, 0.f);
    tex = glm::vec2(1.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(0.f, 0.f, 0.f);
    norm = glm::vec3(0.f, 1.f, 0.f);
    tex = glm::vec2(0.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    vector<unsigned int> indices;
    indices = { 0, 3, 1, 3, 2, 1 };

    mFloor = new Mesh2D(verts, indices, mFloorTexture);

}

void calculateFloorTransform() {
    glm::vec3 v1;
    glm::vec3 v2;
    if (mFloorNormal.z != 0) {
        v1.x = 1;
        v1.y = 0;
        v1.z = -mFloorNormal.x / mFloorNormal.z;

        v2 = glm::cross(mFloorNormal, v1);

    } else if (mFloorNormal.x != 0) {
        v1.x = -mFloorNormal.z / mFloorNormal.x;
        v1.y = 0;
        v1.z = 1;

        v2 = glm::cross(mFloorNormal, v1);
    } else if (mFloorNormal.y != 0) {
        v1.x = 0;
        v1.y = -mFloorNormal.z / mFloorNormal.y;
        v1.z = 1;

        v2 = glm::cross(mFloorNormal, v1);
    }

    if (length(v1) > 0) v1 *= (1 / length(v1));
    if (length(v2) > 0) v2 *= (1 / length(v2));

    mFloorTransformation = glm::mat3(1.f);
    mFloorTransformation[0][0] = mFloorNormal.x;
    mFloorTransformation[1][0] = mFloorNormal.z;
    mFloorTransformation[2][0] = mFloorNormal.y;

    mFloorTransformation[0][1] = v1.x;
    mFloorTransformation[1][1] = v1.z;
    mFloorTransformation[2][1] = v1.y;

    mFloorTransformation[0][2] = v2.x;
    mFloorTransformation[1][2] = v2.z;
    mFloorTransformation[2][2] = v2.y;

    cout << "| " << mFloorTransformation[0][0] << "  " << mFloorTransformation[0][1] << "  " << mFloorTransformation[0][2] << " |" << endl;
    cout << "| " << mFloorTransformation[1][0] << "  " << mFloorTransformation[1][1] << "  " << mFloorTransformation[1][2] << " |" << endl;
    cout << "| " << mFloorTransformation[2][0] << "  " << mFloorTransformation[2][1] << "  " << mFloorTransformation[2][2] << " |\n" << endl;
}

void buildObstacles() {

    mObstacles.clear();
    for (int i = 0; i < mNumObstacles; i++) {
        auto randX = rand() % (int)mFloorWidth;
        auto randY = rand() % (int)mFloorHeight;
        auto randRad = mMinRadius + rand() % (int)(mMaxRadius - mMinRadius);

        mObstacles.push_back(make_pair(glm::vec2(randX, randY), randRad));
    }


    clearObstacles();

    for (int i = 0; i < mObstacles.size(); i++) {
        float halfSideLength = sqrt(2.f) * mObstacles[i].second / 2.f;
        glm::vec2 centerpos = mObstacles[i].first;

        vector<Vertex> verts;
        glm::vec3 pos = mFloorTransformation * glm::vec3(centerpos.x - halfSideLength, 0, centerpos.y + halfSideLength);
        glm::vec3 norm = -1.f * normalize(pos);
        glm::vec2 tex = glm::vec2(0.f, 1.f);
        glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f);
        glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(centerpos.x + halfSideLength, 0, centerpos.y + halfSideLength);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(1.f, 1.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(centerpos.x + halfSideLength, mObsHeight, centerpos.y + halfSideLength);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(1.f, 0.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(centerpos.x - halfSideLength, mObsHeight, centerpos.y + halfSideLength);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(0.f, 0.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(centerpos.x - halfSideLength, 0, centerpos.y - halfSideLength);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(1.f, 0.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(centerpos.x + halfSideLength, 0, centerpos.y - halfSideLength);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(0.f, 0.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(centerpos.x + halfSideLength, mObsHeight, centerpos.y - halfSideLength);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(0.f, 1.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(centerpos.x - halfSideLength, mObsHeight, centerpos.y - halfSideLength);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(1.f, 1.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));


        vector<unsigned int> indices;
        //              back                front
        indices = { 1, 0, 2, 2, 3, 0,  5, 4, 6, 6, 7, 4,  0, 1, 4, 4, 5, 1,  1, 2, 5, 5, 6, 2,  2, 3, 6, 6, 7, 3,  3, 0, 7, 7, 4, 0 };

        mObstacleMeshes.push_back(new Mesh2D(verts, indices, mObstacleTexture));
    }
}

void buildObstacle(glm::vec3 mypos) {
    auto randRad = mMinRadius + rand() % (int)(mMaxRadius - mMinRadius);

    mObstacles.push_back(make_pair(glm::vec2(mypos.x, mypos.z), randRad));

    float halfSideLength = sqrt(2.f) * randRad / 2.f;
    glm::vec2 centerpos = glm::vec2(mypos.x, mypos.z);

    vector<Vertex> verts;
    glm::vec3 pos = mFloorTransformation * glm::vec3(centerpos.x - halfSideLength, 0, centerpos.y + halfSideLength);
    glm::vec3 norm = -1.f * normalize(pos);
    glm::vec2 tex = glm::vec2(0.f, 1.f);
    glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(centerpos.x + halfSideLength, 0, centerpos.y + halfSideLength);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(1.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(centerpos.x + halfSideLength, mObsHeight, centerpos.y + halfSideLength);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(1.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(centerpos.x - halfSideLength, mObsHeight, centerpos.y + halfSideLength);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(0.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(centerpos.x - halfSideLength, 0, centerpos.y - halfSideLength);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(1.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(centerpos.x + halfSideLength, 0, centerpos.y - halfSideLength);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(0.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(centerpos.x + halfSideLength, mObsHeight, centerpos.y - halfSideLength);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(0.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(centerpos.x - halfSideLength, mObsHeight, centerpos.y - halfSideLength);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(1.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));


    vector<unsigned int> indices;
    //              back                front
    indices = { 1, 0, 2, 2, 3, 0,  5, 4, 6, 6, 7, 4,  0, 1, 4, 4, 5, 1,  1, 2, 5, 5, 6, 2,  2, 3, 6, 6, 7, 3,  3, 0, 7, 7, 4, 0 };

    mObstacleMeshes.push_back(new Mesh2D(verts, indices, mObstacleTexture));
}

void clearObstacles() {
    auto it = mObstacleMeshes.begin();
    while (it != mObstacleMeshes.end())
    {
        if (true) {
            // erase() invalidates the iterator, use returned iterator
            auto i = *it;
            it = mObstacleMeshes.erase(it);
            delete(i);
        }
        // Notice that iterator is incremented only on the else part (why?)
        else {
            ++it;
        }
    }
    mObstacleMeshes.clear();
}

void buildAgents() {
    if (!mAgents.empty()) {
        auto it = mAgents.begin();
        while (it != mAgents.end())
        {
            if (true) {
                // erase() invalidates the iterator, use returned iterator
                auto i = *it;
                it = mAgents.erase(it);
                delete(i);
            }
            // Notice that iterator is incremented only on the else part (why?)
            else {
                ++it;
            }
        }
    }
    for (int i = 0; i < mNumAgents; i++) {
        //auto startEdge = rand() % 4;
        int startEdge = 0;
        int endEdge = 1;
        /*int endEdge = startEdge;
        while (startEdge == endEdge) {
            endEdge = rand() % 4;
        }*/
        auto start = glm::vec2(0.f);
        auto end = glm::vec2(0.f);

        switch (startEdge) {
        case 0:
            start.x = 0.f;
            start.y = mFloorHeight / 4.f + rand() % (int)(mFloorHeight / 2.f);
            break;
        case 1:
            start.x = mFloorWidth;
            start.y = mFloorHeight / 4.f + rand() % (int)(mFloorHeight / 2.f);
            break;
        case 2:
            start.y = 0.f;
            start.x = mFloorWidth / 4.f + rand() % (int)(mFloorWidth / 2.f);
            break;
        case 3:
            start.y = mFloorHeight;
            start.x = mFloorWidth / 4.f + rand() % (int)(mFloorWidth / 2.f);
            break;
        default:
            break;
        }

        switch (endEdge) {
        case 0:
            end.x = 0.f;
            end.y = mFloorHeight / 4.f + rand() % (int)(mFloorHeight / 2.f);
            break;
        case 1:
            end.x = mFloorWidth;
            end.y = mFloorHeight / 4.f + rand() % (int)(mFloorHeight / 2.f);
            break;
        case 2:
            end.y = 0.f;
            end.x = mFloorWidth / 4.f + rand() % (int)(mFloorWidth / 2.f);
            break;
        case 3:
            end.y = mFloorHeight;
            end.x = mFloorWidth / 4.f + rand() % (int)(mFloorWidth / 2.f);
            break;
        default:
            break;
        }

        auto agent = new Agent(start, end);
        //agent->buildRRTStar(mObstacles);
        agent->buildAgent();
        //agent->buildSolution();
        agent->buildStartEnd();

        mAgents.push_back(agent);
    }
}

void Agent::buildStartEnd() {
    if (mStartMesh) {
        auto s = mStartMesh;
        delete(s);
        mStartMesh = nullptr;
    }

    if (mEndMesh) {
        auto e = mEndMesh;
        delete(e);
        mEndMesh = nullptr;
    }

    
    vector<Vertex> verts;
    glm::vec3 pos = mFloorTransformation * glm::vec3(mStartPos.x - 2.f, 0, mStartPos.y);
    glm::vec3 norm = -1.f * normalize(pos);
    glm::vec2 tex = glm::vec2(0.f, 1.f);
    glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mStartPos.x, 0, mStartPos.y);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(1.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mStartPos.x, 2.f, mStartPos.y);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(1.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mStartPos.x - 2.f, 2.f, mStartPos.y);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(0.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mStartPos.x - 2.f, 0, mStartPos.y - 2.f);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(1.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mStartPos.x, 0, mStartPos.y - 2.f);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(0.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mStartPos.x, 2.f, mStartPos.y - 2.f);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(0.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mStartPos.x - 2.f, 2.f, mStartPos.y - 2.f);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(1.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));


    vector<unsigned int> indices;
    //              back                front
    indices = { 1, 0, 2, 2, 3, 0,  5, 4, 6, 6, 7, 4,  0, 1, 4, 4, 5, 1,  1, 2, 5, 5, 6, 2,  2, 3, 6, 6, 7, 3,  3, 0, 7, 7, 4, 0 };

    mStartMesh = new Mesh2D(verts, indices, mStartEndTexture);

    verts.clear();
    pos = mFloorTransformation * glm::vec3(mGoalPos.x - 2.f, 0, mGoalPos.y);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(0.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mGoalPos.x, 0, mGoalPos.y);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(1.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mGoalPos.x, 2.f, mGoalPos.y);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(1.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mGoalPos.x - 2.f, 2.f, mGoalPos.y);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(0.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mGoalPos.x - 2.f, 0, mGoalPos.y - 2.f);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(1.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mGoalPos.x, 0, mGoalPos.y - 2.f);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(0.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mGoalPos.x, 2.f, mGoalPos.y - 2.f);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(0.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mGoalPos.x - 2.f, 2.f, mGoalPos.y - 2.f);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(1.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    mEndMesh = new Mesh2D(verts, indices, mStartEndTexture);
}

void Agent::buildSolution() {
    auto it = mSolutionMeshes.begin();
    while (it != mSolutionMeshes.end())
    {
        if (true) {
            // erase() invalidates the iterator, use returned iterator
            auto i = *it;
            it = mSolutionMeshes.erase(it);
            delete(i);
        }
        // Notice that iterator is incremented only on the else part (why?)
        else {
            ++it;
        }
    }
    mSolutionMeshes.clear();

    auto solution = mMyRRTStar->getSolution();
    if (solution.empty()) return;

    for (int i = 0; i < solution.size() - 1; i++) {
        auto p = solution[i];
        auto q = solution[i + 1];

        auto perp = glm::vec2(1, -(p - q).x / (p - q).y);
        perp = normalize(perp);

        auto a = p + perp * mSolutionMeshThickness / 2.f;
        auto b = q + perp * mSolutionMeshThickness / 2.f;
        auto c = q - perp * mSolutionMeshThickness / 2.f;
        auto d = p - perp * mSolutionMeshThickness / 2.f;

        vector<Vertex> verts;
        glm::vec3 pos = mFloorTransformation * glm::vec3(a.x, 0, a.y);
        glm::vec3 norm = -1.f * normalize(pos);
        glm::vec2 tex = glm::vec2(0.f, 1.f);
        glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f);
        glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(b.x, 0, b.y);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(1.f, 1.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(b.x, mSolutionMeshThickness, b.y);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(1.f, 0.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(a.x, mSolutionMeshThickness, a.y);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(0.f, 0.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(d.x, 0, d.y);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(1.f, 0.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(c.x, 0, c.y);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(0.f, 0.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(c.x, mSolutionMeshThickness, c.y);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(0.f, 1.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(d.x, mSolutionMeshThickness, d.y);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(1.f, 1.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));


        vector<unsigned int> indices;
        //              back                front
        indices = { 1, 0, 2, 2, 3, 0,  5, 4, 6, 6, 7, 4,  0, 1, 4, 4, 5, 1,  1, 2, 5, 5, 6, 2,  2, 3, 6, 6, 7, 3,  3, 0, 7, 7, 4, 0 };


        mSolutionMeshes.push_back(new Mesh2D(verts, indices, mSolutionTexture));
    }

}

void Agent::buildAgent() {
    if (mAgentMesh) {
        auto a = mAgentMesh;
        delete(a);
        mAgentMesh = nullptr;
    }

    float sideLength = sqrt(2.f) * mAgentRadius / 2.f;
    mAgentPos = glm::vec3(mStartPos.x , 0.f, mStartPos.y);
    mAimAt = glm::vec3(0.f);

    vector<Vertex> verts;
    glm::vec3 pos = mFloorTransformation * glm::vec3(mAgentPos.x - sideLength / 2, 0, mAgentPos.z - sideLength / 2);
    glm::vec3 norm = -1.f * normalize(pos);
    glm::vec2 tex = glm::vec2(0.f, 0.f);
    glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mAgentPos.x - sideLength / 2, 0, mAgentPos.z + sideLength / 2);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(1.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mAgentPos.x - sideLength / 2, mObsHeight, mAgentPos.z + sideLength / 2);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(0.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mAgentPos.x - sideLength / 2, mObsHeight, mAgentPos.z - sideLength / 2);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(0.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mAgentPos.x + sideLength / 2, mObsHeight / 2.f, mAgentPos.z - sideLength / 2);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(1.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));


    vector<unsigned int> indices;
    //              back                front
    indices = { 0, 1, 2, 0, 3, 2,   0, 1, 4,   1, 2, 4,   2, 3, 4,   3, 0, 4 };

    mAgentMesh = new Mesh2D(verts, indices, mAgentTexture);
}


void Agent::updateAgent(float dt) {
    if (mMyRRTStar && mAgentMesh) {

        mAimAt = mMyRRTStar->nextAvailablePos(glm::vec2(mAgentPos.x, mAgentPos.z), mAgentRadius);

        if (mAimAt.x > 0 && glm::length(mAgentPos - mAimAt) > 1.0f) {
            auto dir = normalize(mAimAt - mAgentPos);
            glm::vec3 oldVel = glm::vec3(0.f);
            for (int i = 0; i < mAgentMesh->getNumVerts(); i++) {
                auto vert = mAgentMesh->getVertAt(i);
                oldVel = vert.mVelocity;
                vert.mVelocity = (dir + 15.f * oldVel) / 16.f;
                if (glm::length(vert.mVelocity) > 0) vert.mVelocity = glm::normalize(vert.mVelocity);

                float theta = 0.f;
                if (glm::length(oldVel) > 0) {
                    auto myDot = glm::dot(mFloorTransformation * vert.mVelocity, mFloorTransformation * oldVel);
                    if (abs(myDot - 1) < 0.001) {
                        theta = 0.f;
                    }
                    else {
                        theta = acos(myDot);
                    }
                }
                auto rotation = glm::mat3(0.f);
                rotation[0][0] = 1.f;// cos(theta);
                //rotation[0][2] = sin(theta);
                rotation[1][1] = 1.f;
                //rotation[2][0] = -sin(theta);
                rotation[2][2] = 1.f;// cos(theta);

                auto oldPos = vert.mPosition;
                oldPos -= mFloorTransformation * mAgentPos;
                oldPos = rotation * oldPos;
                oldPos += mFloorTransformation * mAgentPos;
                vert.mPosition = oldPos + mFloorTransformation * vert.mVelocity * mAgentSpeed * dt;
                mAgentMesh->setVertAt(i, vert);
            }

            mAgentPos += dir * mAgentSpeed * dt;
        }
    }

}

void Agent::buildRRTStar(vector<pair<glm::vec2, float>> obstacles) {
	if (mMyRRTStar) {
		auto r = mMyRRTStar;
		delete(r);
		mMyRRTStar = nullptr;
	}

    mMyRRTStar = new RRTStar((int)mFloorWidth, (int)mFloorHeight, mStartPos, mGoalPos, mNumVerts);

    for (int i = 0; i < mObstacles.size(); i++) {
        mMyRRTStar->addObstacle(obstacles[i].first, obstacles[i].second);
    }

    mMyRRTStar->start(mAgentRadius);


}

void Agent::drawAgent() {
    for (auto it : mSolutionMeshes) {
        it->draw();
    }
    if (mAgentMesh) mAgentMesh->draw();
    if (mStartMesh) mStartMesh->draw();
    if (mEndMesh) mEndMesh->draw();

}

void display() {
    glm::vec3 lookAt = cameraFront + cameraPos;

    // render
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(mShaderProgram);

    glLoadIdentity();
    gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z, lookAt.x, lookAt.y, lookAt.z, cameraUp.x, cameraUp.y, cameraUp.z);

    // view/projection/model transformations
    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, cameraDepth);
    glUniformMatrix4fv(glGetUniformLocation(mShaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

    glm::vec3 newCameraPos = glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z);
    glm::vec3 newCameraFront = glm::vec3(cameraFront.x, cameraFront.y, cameraFront.z);
    glm::vec3 newCameraUp = glm::vec3(cameraUp.x, cameraUp.y, cameraUp.z);
    glm::mat4 view = glm::lookAt(newCameraPos, newCameraPos + newCameraFront, newCameraUp);
    glUniformMatrix4fv(glGetUniformLocation(mShaderProgram, "view"), 1, GL_FALSE, &view[0][0]);

    glm::mat4 model = glm::mat4(1.0f); //Use if we need to rotate/scale/translate the mesh as a whole
    glUniformMatrix4fv(glGetUniformLocation(mShaderProgram, "model"), 1, GL_FALSE, &model[0][0]);

   //draw meshes
    if (mFloor) mFloor->draw();
    for (auto it : mObstacleMeshes) {
        it->draw();
    }    
    
    for (auto agent : mAgents) {
        agent->drawAgent();
    }

    glutSwapBuffers();

    //FPS calculations
    framesSinceLast++;
    long deltaTime = glutGet(GLUT_ELAPSED_TIME);

    if (deltaTime - lastTimeSecond > 1000) {
        int fps = (int)(framesSinceLast * 1000.0 / (deltaTime - lastTimeSecond));
        lastTimeSecond = deltaTime;
        framesSinceLast = 0;
        cout << "FPS: " << fps;
        int totalVerts = 0;
        //TODO: count agent meshes
        totalVerts += mObstacleMeshes.size() * 8;
        //totalVerts += mAgent1->getNumVerts();
        //totalVerts += mAgent2->getNumVerts();
        totalVerts += mFloor->getNumVerts();
        cout << "\t Vertices: ";
        cout << totalVerts << endl;
    }
}

void initGL() {
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(908, 640);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("5611 Project 3");
    glutDisplayFunc(display); // declaring the "draw" function
    glutReshapeFunc(reshape);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glCheckError();
}

void initShader() {
    // compile shaders!
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glCheckError();
    glShaderSource(vertex, 1, &vertexShaderSource, NULL);
    glCheckError();
    glCompileShader(vertex);
    glCheckError();
    glCheckError();
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glCheckError();
    glShaderSource(fragment, 1, &fragmentShaderSource, NULL);
    glCheckError();
    glCompileShader(fragment);
    glCheckError();

    // shader Program
    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, vertex);
    glAttachShader(mShaderProgram, fragment);

    glLinkProgram(mShaderProgram);
    glCheckError();
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void reshape(GLsizei width, GLsizei height) {
    // Compute aspect ratio of the new window
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping volume to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix

    glCheckError();
    glLoadIdentity();             // Reset
    glCheckError();
    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(45.0f, aspect, 0.1f, cameraDepth);
    glCheckError();
}

void animLoop(int val) {
    keyOperations();

    framesSinceLast += 1;

    //for (int i = 0; i < 16; i++) {
    for (auto agent : mAgents) {
        //update
        agent->updateAgent(0.016f);
    }

    glutPostRedisplay();
    glutTimerFunc(16, animLoop, 1);
}


int main(int argc, char* argv[]) {
    glutInit(&argc, argv);

    //init the lot
    glewExperimental = GL_TRUE;
    initGL();
    glewInit();
    initShader();

    //buildrrtstar
    buildEnvironment();
    buildAgents();

    /*interactions stuff*/
    glutKeyboardFunc(keyPressed); // Tell GLUT to use the method "keyPressed" for key presses  
    glutKeyboardUpFunc(keyUp); // Tell GLUT to use the method "keyUp" for key up events
    glutMouseFunc(mouse);

    //start the animation after 5 milliseconds as a buffer
    glutTimerFunc(5, animLoop, 1);

    //all done!
    glCheckError();
    glutMainLoop();

    return 0;
}