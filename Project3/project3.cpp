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

    //cout << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << endl;
}

void mouse(int button, int state, int x, int y) {
    // Wheel reports as button 3(scroll up) and button 4(scroll down)
    if ((button == 3) || (button == 4)) { // It's a wheel event 
        // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
        if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
        printf("Scroll %s At %d %d\n", (button == 3) ? "Up" : "Down", x, y);
    }
    else {  // normal button event
        // Mouse click, spawn particles
        mMouseDown = (state == GLUT_DOWN);
        
        mMousePos = glm::vec2(-1.f, -1.f);
        //printf("Button %s At %d %d\n", (state == GLUT_DOWN) ? "Down" : "Up", x, y);
    }
}

void drag(int x, int y) {
    if (mMouseDown) {
        auto screenPos = glm::vec2(x, y) - mMousePos;//glm::vec2(x / (float)glutGet(GLUT_WINDOW_WIDTH), y / (float)glutGet(GLUT_WINDOW_HEIGHT));
        
        mMousePos += screenPos;
    }
}

void buildEnvironment() {
    calculateFloorTransform();
    buildFloor();
    buildObstacles();
    buildSolution();
}

void buildFloor() {
    if (mFloor) {
        auto c = mFloor;
        delete(c);
        mFloor = nullptr;
    }

    vector<Vertex> verts;
    glm::vec3 pos = mFloorTransformation * glm::vec3(0, 0.f, mFloorHeight);
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

    pos = mFloorTransformation * glm::vec3(mFloorWidth, 0.f, 0);
    norm = glm::vec3(0.f, 1.f, 0.f);
    tex = glm::vec2(1.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(0, 0.f, 0);
    norm = glm::vec3(0.f, 1.f, 0.f);
    tex = glm::vec2(0.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    vector<unsigned int> indices;
    indices = { 0, 1, 3, 3, 2, 1 };

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

    for (int i = 0; i < mObstacles.size(); i++) {
        float sideLength = sqrt(2) * mObstacles[i].second;
        glm::vec2 centerpos = mObstacles[i].first;

        vector<Vertex> verts;
        glm::vec3 pos = mFloorTransformation * glm::vec3(centerpos.x - sideLength / 2, 0, centerpos.y + sideLength / 2);
        glm::vec3 norm = -1.f * normalize(pos);
        glm::vec2 tex = glm::vec2(0.f, 1.f);
        glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f);
        glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(centerpos.x + sideLength / 2, 0, centerpos.y + sideLength / 2);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(1.f, 1.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(centerpos.x + sideLength / 2, mObsHeight, centerpos.y + sideLength / 2);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(1.f, 0.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(centerpos.x - sideLength / 2, mObsHeight, centerpos.y + sideLength / 2);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(0.f, 0.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(centerpos.x - sideLength / 2, 0, centerpos.y - sideLength / 2);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(1.f, 0.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(centerpos.x + sideLength / 2, 0, centerpos.y - sideLength / 2);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(0.f, 0.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(centerpos.x + sideLength / 2, mObsHeight, centerpos.y - sideLength / 2);
        norm = -1.f * normalize(pos);
        tex = glm::vec2(0.f, 1.f);
        vel = glm::vec3(0.f, 0.f, 0.f);
        acc = glm::vec3(0.f, 0.f, 0.f);
        verts.push_back(Vertex(pos, norm, tex, vel, acc));

        pos = mFloorTransformation * glm::vec3(centerpos.x - sideLength / 2, mObsHeight, centerpos.y - sideLength / 2);
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

void buildSolution() {
    auto solution = mMyRRTStar->getSolution();
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

void buildAgent() {
    if (mAgent) {
        auto a = mAgent;
        delete(a);
        mAgent = nullptr;
    }

    float sideLength = sqrt(2) * mAgentRadius;
    mAgentPos = glm::vec3(mStartPos.x , 0.f, mStartPos.y);
    mAimAt = glm::vec3(mStartPos.x, 0.f, mStartPos.y);

    vector<Vertex> verts;
    glm::vec3 pos = mFloorTransformation * glm::vec3(mAgentPos.x - sideLength / 2, 0, mAgentPos.z + sideLength / 2);
    glm::vec3 norm = -1.f * normalize(pos);
    glm::vec2 tex = glm::vec2(0.f, 1.f);
    glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mAgentPos.x + sideLength / 2, 0, mAgentPos.z + sideLength / 2);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(1.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mAgentPos.x + sideLength / 2, mObsHeight, mAgentPos.z + sideLength / 2);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(1.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mAgentPos.x - sideLength / 2, mObsHeight, mAgentPos.z + sideLength / 2);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(0.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mAgentPos.x - sideLength / 2, 0, mAgentPos.z - sideLength / 2);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(1.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mAgentPos.x + sideLength / 2, 0, mAgentPos.z - sideLength / 2);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(0.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mAgentPos.x + sideLength / 2, mObsHeight, mAgentPos.z - sideLength / 2);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(0.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = mFloorTransformation * glm::vec3(mAgentPos.x - sideLength / 2, mObsHeight, mAgentPos.z - sideLength / 2);
    norm = -1.f * normalize(pos);
    tex = glm::vec2(1.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));


    vector<unsigned int> indices;
    //              back                front
    indices = { 1, 0, 2, 2, 3, 0,  5, 4, 6, 6, 7, 4,  0, 1, 4, 4, 5, 1,  1, 2, 5, 5, 6, 2,  2, 3, 6, 6, 7, 3,  3, 0, 7, 7, 4, 0 };

    mAgent = new Mesh2D(verts, indices, mAgentTexture);
}


void updateAgent(float dt) {
    if (!mMyRRTStar) return;

    mAimAt = mMyRRTStar->nextAvailablePos(mAgentPos, mAgentRadius);

    if (glm::length(mAgentPos - mAimAt) > 1.0f) {
        auto dir = normalize(mAimAt - mAgentPos);
        auto diff = dir * mAgentSpeed * dt;
        mAgentPos += diff;

        mAgent->updateVerts(mFloorTransformation * diff);
    }
}

void buildRRTStar() {
	if (mMyRRTStar) {
		auto r = mMyRRTStar;
		delete(r);
		mMyRRTStar = nullptr;
	}

	mMyRRTStar = new RRTStar((int)mFloorWidth, (int)mFloorHeight, mStartPos, mGoalPos, mNumVerts);

    mObstacles.clear();
    for (int i = 0; i < mNumObstacles; i++) {
        glm::vec2 obsPos = glm::vec2(rand() % (int)(mFloorWidth - 2 * mMaxRadius), rand() % (int)(mFloorHeight - 2 * mMaxRadius)) + glm::vec2(mMaxRadius, mMaxRadius);
        float radius = mMinRadius + rand() % (int)(mMaxRadius - mMinRadius);
       /* glm::vec2 obsPos = glm::vec2(-1.f, -1.f);
        float radius = mMinRadius + rand() % (int)(mMaxRadius - mMinRadius);
        bool available = false;
        while (!available) {
            obsPos = glm::vec2(rand() % (int)mFloorWidth, rand() % (int)mFloorHeight);
            bool notAvailable = false;
            for (auto obs : mObstacles) {
                if ((obs.first - obsPos).length() < obs.second + radius) {
                    notAvailable = true;
                    break;
                }
            }
            available = !notAvailable;
        }*/
        mObstacles.push_back(make_pair(obsPos, radius));
        mMyRRTStar->addObstacle(obsPos, radius);
        cout << "building obstacle at (" << obsPos.x << ", " << obsPos.y << ")" << endl;
    }

	vector<glm::vec2> solution = mMyRRTStar->start();
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
    for (auto it : mSolutionMeshes) {
        it->draw();
    }
    if (mAgent) mAgent->draw();

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
        totalVerts += mSolutionMeshes.size() * 8;
        totalVerts += mObstacleMeshes.size() * 8;
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
        //update
        updateAgent(0.016f);
    //}

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

    buildRRTStar();
    buildEnvironment();
    buildAgent();

    /*interactions stuff*/
    glutKeyboardFunc(keyPressed); // Tell GLUT to use the method "keyPressed" for key presses  
    glutKeyboardUpFunc(keyUp); // Tell GLUT to use the method "keyUp" for key up events
    glutMouseFunc(mouse);
    glutMotionFunc(drag);

    //start the animation after 5 milliseconds as a buffer
    glutTimerFunc(5, animLoop, 1);

    //all done!
    glCheckError();
    glutMainLoop();

    return 0;
}