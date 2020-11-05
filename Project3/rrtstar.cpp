#include "rrtstar.h"

RRTStar::RRTStar() { initEnvironment(); }

RRTStar::RRTStar(int confWidth, int confHeight, glm::vec2 startPos, glm::vec2 goalPos, int numVerts) {
	mWidth = confWidth;
	mHeight = confHeight;

	mInitPos = startPos;
	mGoalPos = goalPos;

	mNumVertices = numVerts;
	initEnvironment();
	return;
}

glm::vec2 RRTStar::getInitPos() { return mInitPos; }
void RRTStar::setInitPos(glm::vec2 initPos) { mInitPos = initPos; }

glm::vec2 RRTStar::getGoalPos() { return mGoalPos; }
void RRTStar::setGoalPos(glm::vec2 goalPos) { mGoalPos = goalPos; }

float RRTStar::getConfWidth() { return mWidth; }
float RRTStar::getConfHeight() { return mHeight; }
void RRTStar::setConfSize(float width, float height) { mWidth = width; mHeight = height; }

int RRTStar::getNumVertices() { return mNumVertices; }
void RRTStar::setNumVertices(int numVerts) { mNumVertices = numVerts; }

int RRTStar::getNumNodes() { return myTree->getTreeSize(); }
int RRTStar::getIsSuccessful() { return (mSolutionPath.size() > 0); }

void RRTStar::addObstacle(glm::vec2 pos, float radius) {
	mObstacles.push_back(make_pair(pos, radius));
}

vector<glm::vec2> RRTStar::start() {
	letsBuildRRTStar();
	return mSolutionPath;
}

vector<glm::vec2> RRTStar::start(glm::vec2 means, float sxx, float syy, float sxy) {
	letsBuildRRTStarOnDist(means, sxx, syy, sxy);
	return mSolutionPath;
}

void RRTStar::draw() {

	

}

glm::vec2 RRTStar::randConfEven() {
	float randX = rand() % mWidth;
	float randY = rand() % mHeight;
	glm::vec2 myPos = glm::vec2(randX, randY);
	return myPos;
}

glm::vec2 RRTStar::randConfDist(glm::vec2 means, float sxx, float syy, float sxy) {
	/*normal_distribution<> dx{ means.mX, sxx };
	normal_distribution<> dy{ means.mY, syy };
	random_device rd{};
	mt19937 gen{ rd() };

	float a = dx(gen);
	float b = dy(gen);
	Eigen::VectorXd v(2);
	v(0) = a;
	v(1) = b;
	Eigen::MatrixXd A(2, 2);
	A(0, 0) = sxx;
	A(0, 1) = sxy;
	A(1, 0) = sxy;
	A(1, 1) = syy;
	Eigen::MatrixXd L(A.llt().matrixL());
	Eigen::MatrixXd S = A.sqrt();
	Eigen::VectorXd solution = L * v;
	Eigen::VectorXd solution1 = S * v;
	vec2 sample = vec2(solution(0), solution(1));
	vec2 sample1 = vec2(solution1(0), solution1(1));
	sample = sample + means;

	return sample;*/
	normal_distribution<> dx{ means.x, sxx };
	normal_distribution<> dy{ means.y, syy };
	random_device rd{};
	mt19937 gen{ rd() };

	float a = dx(gen);
	float b = dy(gen);
	glm::vec2 sample = glm::vec2(a * sxx + b * sxy, a * sxy + b * syy);
	sample = sample + means;

	return sample;
}

//vec2 nearestPixel(vec2 rand) {
//  //loop through all points in the graph and find the one closest to the rand pos
//  vec2 myPos = myTree.getNearestPoint(rand);

//  return myPos;
//}

Node* RRTStar::nearestNode(glm::vec2 rand) {
	//loop through all points in the graph and find the one closest to the rand pos
	auto myNode = myTree->getNearestNode(rand);

	return myNode;
}

glm::vec2 RRTStar::newConf(glm::vec2 nearby, glm::vec2 rand) {
	//the new point is as far along the near-rand vector as you can
	//if we cant step that way at all, return a failed value (-1, -1)

	//is it the same point?
	glm::vec2 diffVec = (rand - nearby);
	if (glm::length(diffVec) <= 1.0) {
		return glm::vec2(-1.0, -1.0);
	}

	// parameterization
	// circle: (x - center.x)^2 + (y - center.y)^2 = r^2
	// line: x = diffVec.mX * t + near.mX
	// line: y = diffVec.mY * t + near.mY
	// min possible t = 0 max possible t  = 1 for segment
	float tMin = 1;
	for (auto obstacle : mObstacles) {
		//what if the segment starts in the obstacle?
		if (glm::length(obstacle.first - nearby) <= obstacle.second) {
			return glm::vec2(-1.0, -1.0);
		}

		float a = glm::length(diffVec) * glm::length(diffVec);
		float b = -2 * dot((obstacle.first - nearby), diffVec);
		float c = glm::length(nearby - obstacle.first) * glm::length(nearby - obstacle.first) - obstacle.second * obstacle.second;

		float d = b * b - 4 * a * c;
		if (d > 0.0) {
			//intersections possible
			float tPlus = (-b + sqrt(d)) / (2 * a);
			float tMinus = (-b - sqrt(d)) / (2 * a);

			if (tPlus <= 1 && tPlus >= 0) {
				//real intersection! 
				if (tPlus < tMin) {
					tMin = tPlus;
				}
			}

			if (tMinus <= 1 && tMinus >= 0) {
				//real intersection! 
				if (tMinus < tMin) {
					tMin = tMinus;
				}
			}
		}
	}

	auto res = (diffVec * tMin) + nearby;
	return (res);
}

void RRTStar::letsBuildRRTStar() {
	Node* newNode = new Node(mInitPos, nullptr);
	int count = 0;
	while (glm::length(mGoalPos - newNode->mPosition) > 10.f) {
		if (count > mCountMax) break;
		count++;

		glm::vec2 randPos = glm::vec2(-1, -1);
		if (rand() % 100 < 10) {
			randPos = mGoalPos;
		}
		else {
			randPos = randConfEven();
		}

		Node* nearNode = nearestNode(randPos);
		glm::vec2 tempNewPos = newConf(nearNode->mPosition, randPos);

		if (tempNewPos.x > 0) {
			newNode = new Node(tempNewPos, nearNode);
			myTree->addVertex(newNode);
			myTree->addEdge(nearNode, newNode);
		}
	}
	cout << "my tree has " << myTree->getTreeSize() << " nodes" << endl;
	mSolutionPath.clear();
	if (count > mCountMax) {
		cout << "Couldn't find a solution... So saaaad" << endl;
	}
	else {
		cout << "Found a solution! Yay go you!" << endl;
		mSolutionPath.push_back(newNode->mPosition);
		while (abs(newNode->mPosition.x - mInitPos.x) > 1.0 || abs(newNode->mPosition.y - mInitPos.y) > 1.0) {
			if (newNode->mParent == nullptr) {
				break;
			}
			newNode = newNode->mParent;
			mSolutionPath.push_back(newNode->mPosition);
		}
		cout << "my solution path has " << mSolutionPath.size() << endl;
	}
}

void RRTStar::letsBuildRRTStarOnDist(glm::vec2 means, float sxx, float syy, float sxy) {
	Node* newNode = new Node(mInitPos, nullptr);
	bool useDist = (means.x > 0 && means.y > 0);
	Eigen::Vector2f mean(2);
	mean(0) = means.x;
	mean(1) = means.y;
	Eigen::Matrix2f covar(2, 2);
	covar(0, 0) = sxx;
	covar(0, 1) = sxy;
	covar(1, 0) = sxy;
	covar(1, 1) = syy;
	Eigen::EigenMultivariateNormal<float> normX_solver1(mean, covar);
	int count = 0;
	while (glm::length(mGoalPos - newNode->mPosition) > 10.f) {

		if (count > mCountMax) break;
		count++;

		glm::vec2 randPos = glm::vec2(-1, -1);
		if (means.x > 0 && means.y > 0) {
			if (rand() % 100 < 10) {
				randPos = mGoalPos;
			}
			else {
				auto sample = normX_solver1.samples(1);
				randPos = glm::vec2(sample(0, 0), sample(1, 0));
			}
			//cout << "chose " << randPos.mX << ", " << randPos.mY << endl;
		}

		Node* nearNode = nearestNode(randPos);
		glm::vec2 tempNewPos = newConf(nearNode->mPosition, randPos);

		if (tempNewPos.x > 0) {
			newNode = new Node(tempNewPos, nearNode);
			myTree->addVertex(newNode);
			myTree->addEdge(nearNode, newNode);
		}
	}
	mSolutionPath.clear();
	if (count > mCountMax) {
		cout << "Couldn't find a solution... So saaaad" << endl;
	}
	else {
		cout << "Found a solution! Yay go you!" << endl;
		mSolutionPath.push_back(newNode->mPosition);
		while (abs(newNode->mPosition.x - mInitPos.x) > 1.0 || abs(newNode->mPosition.y - mInitPos.y) > 1.0) {
			if (newNode->mParent == nullptr) {
				break;
			}
			newNode = newNode->mParent;
			mSolutionPath.push_back(newNode->mPosition);
		}
	}
}

void RRTStar::initEnvironment() {
	Node* initNode = new Node(mInitPos, nullptr);
	if (myTree) {
		auto t = myTree;
		delete(t);
		myTree = nullptr;
	}
	myTree = new Tree();
	myTree->addVertex(initNode);
}


