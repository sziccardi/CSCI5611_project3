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

RRTStar::~RRTStar() {
	if (myTree) delete(myTree);
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

glm::vec3 RRTStar::nextAvailablePos(glm::vec2 currentPos, float agentRad) {
	if (mSolutionPath.empty()) return glm::vec3(-1.f);
	glm::vec3 nextPos = glm::vec3(mSolutionPath[mSolutionPath.size() - 1 - mCurrentAimIter].x, 0.f, mSolutionPath[mSolutionPath.size() - 1 - mCurrentAimIter].y);
	//is goal visible?
	glm::vec2 goalray = mGoalPos - currentPos;
	bool goalcollides = false;
	for (auto obs : mObstacles) {
		goalcollides = collides(obs.first, (obs.second + agentRad), currentPos, glm::normalize(goalray), glm::length(goalray));
		if (goalcollides) {
			break;
		}
	}
	if (!goalcollides) {
		return glm::vec3(mGoalPos.x, 0.f, mGoalPos.y);
	}

	//if cant head to the goal right away, lets go to something closer
	for (auto it : mSolutionPath) {
		if (mCurrentAimIter < mSolutionPath.size() - 1) {
			//check next
			int tempIter = mCurrentAimIter + 1;
			glm::vec2 ray = mSolutionPath[mSolutionPath.size() - 1 - tempIter] - currentPos;
			bool collided = true;
			for (auto obs : mObstacles) {
				collided = collides(obs.first, (obs.second + agentRad), currentPos, normalize(ray), glm::length(ray));
				if (collided) break;
			}
			if (!collided) {
				mCurrentAimIter = tempIter;
				nextPos = glm::vec3(mSolutionPath[mSolutionPath.size() - 1 - mCurrentAimIter].x, 0.f, mSolutionPath[mSolutionPath.size() - 1 - mCurrentAimIter].y);
			}
		}
	}
	return nextPos;
}

bool RRTStar::collides(glm::vec2 center, float r, glm::vec2 l_start, glm::vec2 l_dir, float max_t) {

	//Compute displacement vector pointing from the start of the line segment to the center of the circle
	glm::vec2 toCircle = center - l_start;

	//Solve quadratic equation for intersection point (in terms of l_dir and toCircle)
	float a = 1;  //Length of l_dir (we normalized it)
	float b = -2 * glm::dot(l_dir, toCircle); //-2*dot(l_dir,toCircle)
	float c = (glm::length(toCircle) * glm::length(toCircle)) - (r * r); //different of squared distances

	float d = b * b - 4 * a * c; //discriminant 

	if (d >= 0) {
		//If d is positive we know the line is colliding, but we need to check if the collision line within the line segment
		//  ... this means t will be between 0 and the length of the line segment
		float t1 = (-b - sqrt(d)) / (2 * a); //Optimization: we only need the first collision
		float t2 = (-b + sqrt(d)) / (2 * a); //Optimization: we only need the first collision
		//println(hit.t,t1,t2);
		if (t1 > 0 && t1 < max_t) { //We intersect the circle
			return true;
		}
		else if (t1 < 0 && t2 > 0) { //We start in the circle
			return true;
		}

	}

	return false;
}

void RRTStar::addObstacle(glm::vec2 pos, float radius) {
	mObstacles.push_back(make_pair(pos, radius));
}

vector<glm::vec2> RRTStar::start(float agentRad) {
	mAgentRadius = agentRad;
	letsBuildRRTStar();
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
		if (glm::length(obstacle.first - nearby) <= obstacle.second + mAgentRadius) {
			return glm::vec2(-1.0, -1.0);
		}

		float a = glm::length(diffVec) * glm::length(diffVec);
		float b = -2 * dot((obstacle.first - nearby), diffVec);
		float c = glm::length(nearby - obstacle.first) * glm::length(nearby - obstacle.first) - (obstacle.second + mAgentRadius) * (obstacle.second + mAgentRadius);

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
	Node* newNode = new Node(mInitPos, nullptr, 0.f);
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

		//cout << "my rand pos is " << randPos.x << ", " << randPos.y << endl;
		vector<Node*> nearNodes = myTree->getNearNodes(randPos, mNeighborRadius);
		//cout << " I found " << nearNodes.size() << " neighbors" << endl;
		if (nearNodes.empty()) nearNodes.push_back(nearestNode(randPos));
		float minCost = 999999999.f;
		Node* cheapestNeighbor = nullptr;
		for (Node* testNode : nearNodes) {
			if (testNode->mCost < minCost) {
				minCost = testNode->mCost;
				cheapestNeighbor = testNode;
			}
		}

		if (cheapestNeighbor) {
			glm::vec2 tempNewPos = newConf(cheapestNeighbor->mPosition, randPos);
			if (tempNewPos.x > 0) {
				newNode = new Node(tempNewPos, cheapestNeighbor, glm::length(tempNewPos - cheapestNeighbor->mPosition));
				myTree->addVertex(newNode);
				myTree->addEdge(cheapestNeighbor, newNode);
			}
		}
		//TODO: figure out why this is intersecting
		//gotta rewire!
		//check through neighbors, if changing my parent to this new node decreases my cost, do it
		//for (Node* neighborNode : nearNodes) {
		//	if (neighborNode->mParent) {
		//		float parentCost = neighborNode->mParent->mCost;
		//		float newParentCost = newNode->mCost;
		//		if (parentCost > newParentCost) {
		//			auto oldParent = neighborNode->mParent;
		//			//remove me from the connected list
		//			auto it = oldParent->mConnectedNodes.begin();
		//			while (it != oldParent->mConnectedNodes.end()) {
		//				if ((*it)->mPosition.x == neighborNode->mPosition.x && (*it)->mPosition.y == neighborNode->mPosition.y) {
		//					it = oldParent->mConnectedNodes.erase(it);
		//				}
		//				else {
		//					++it;
		//				}
		//			}

		//			//change the parent
		//			neighborNode->mParent = newNode;
		//			newNode->mConnectedNodes.push_back(neighborNode);
		//		}
		//	}
		//}

		//clear the tree of dead nodes
		//myTree->clearDeadLeaves();


		/*
		Node* nearNode = nearestNode(randPos);
		glm::vec2 tempNewPos = newConf(nearNode->mPosition, randPos);

		if (tempNewPos.x > 0) {
			newNode = new Node(tempNewPos, nearNode, glm::length(tempNewPos - nearNode->mPosition));
			myTree->addVertex(newNode);
			myTree->addEdge(nearNode, newNode);
		}
		*/
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
		cout << "my solution path has " << mSolutionPath.size() << " points" << endl;
	}
}

void RRTStar::initEnvironment() {
	Node* initNode = new Node(mInitPos, nullptr, 0.f);
	if (myTree) {
		auto t = myTree;
		delete(t);
		myTree = nullptr;
	}
	myTree = new Tree();
	myTree->addVertex(initNode);
}


