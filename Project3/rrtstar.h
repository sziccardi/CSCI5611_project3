#pragma once
#include <cmath>
#include <vector>
#include <iostream>
#include <random>
#include <iomanip>

#include <objidl.h>

#include "data_utils.h"

#include <unsupported/Eigen/MatrixFunctions>
#include <Eigen/src/Cholesky/LLT.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/Eigen>

#include "eigenmvn.h"

using namespace std;

class RRTStar {
public:
	RRTStar();
	RRTStar(int confWidth, int confHeight, glm::vec2 startPos, glm::vec2 goalPos, int numVerts);

	~RRTStar();

	glm::vec2 getInitPos();
	void setInitPos(glm::vec2 initPos);

	glm::vec2 getGoalPos();
	void setGoalPos(glm::vec2 goalPos);

	float getConfWidth();
	float getConfHeight();
	void setConfSize(float width, float height);

	int getNumVertices();
	void setNumVertices(int numVerts);

	int getNumNodes();
	int getIsSuccessful();

	glm::vec3 nextAvailablePos(glm::vec2 currentPos, float agentRad);
	bool collides(glm::vec2 center, float r, glm::vec2 l_start, glm::vec2 l_dir, float max_t);

	void addObstacle(glm::vec2 pos, float radius);
	vector<glm::vec2> start(float agentRad);

	vector<glm::vec2> getSolution() { return mSolutionPath; }

	void draw();

	bool mUseDist = false;
	int mCountMax = 10000;

private:
	glm::vec2 randConfEven();
	glm::vec2 randConfDist(glm::vec2 means = glm::vec2(-1, -1), float sxx = -1, float syy = -1, float sxy = -1);

	Node* nearestNode(glm::vec2 rand);
	glm::vec2 newConf(glm::vec2 nearby, glm::vec2 rand);

	void letsBuildRRTStar();
	void initEnvironment();

	int mNumVertices = 154;
	Tree* myTree;
	vector<glm::vec2> mSolutionPath;
	int mCurrentAimIter = 0;
	glm::vec2 mInitPos = glm::vec2(-1.0, -1.0);
	glm::vec2 mGoalPos = glm::vec2(-1.0, -1.0);
	vector<pair<glm::vec2, float>> mObstacles;
	float mAgentRadius = 0.f;
	float dq = 10;
	int mWidth = 0;
	int mHeight = 0;

	float mNeighborRadius = 85.f;
};