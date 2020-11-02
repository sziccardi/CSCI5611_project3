#include "rrtstar.h"

// Contructor which initialize the parameters.
//Framework::Framework(int height_, int width_) : height(height_), width(width_) {
//	SDL_Init(SDL_INIT_VIDEO);       // Initializing SDL as Video
//	SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
//	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);      // setting draw color
//	SDL_RenderClear(renderer);      // Clear the newly created window
//	SDL_RenderPresent(renderer);    // Reflects the changes done in the
//									//  window.
//}
//
//// Destructor
//Framework::~Framework() {
//	SDL_DestroyRenderer(renderer);
//	SDL_DestroyWindow(window);
//	SDL_Quit();
//}
//
//void Framework::draw_circle(int center_x, int center_y, int radius_, glm::vec3 color) {
//	// Setting the color to be RED with 100% opaque (0% trasparent).
//	SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 255);
//
//	// Drawing circle
//	for (int x = center_x - radius_; x <= center_x + radius_; x++) {
//		for (int y = center_y - radius_; y <= center_y + radius_; y++) {
//			if ((std::pow(center_y - y, 2) + std::pow(center_x - x, 2)) <=
//				std::pow(radius_, 2)) {
//				SDL_RenderDrawPoint(renderer, x, y);
//			}
//		}
//	}
//
//	// Show the change on the screen
//}
//
//void Framework::draw_tree(Tree* myTree) {
//	//draw tree
//	SDL_SetRenderDrawColor(renderer, 0.f, 0.f, 0.f, 175);
//
//	auto myList = myTree->getList();
//	for (auto realPair : myList) {
//		for (Node* connectedNode : (realPair.second)->mConnectedNodes) {
//			SDL_RenderDrawLine(renderer, realPair.first.mX, realPair.first.mY, connectedNode->mPosition.mX, connectedNode->mPosition.mY);
//		}
//	}
//}
//
//void Framework::draw_solution(vector<glm::vec2> solution) {
//	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
//	glm::vec2 prevPoint = glm::vec2(-1, -1);
//	for (auto val : solution) {
//		if (prevPoint.x >= 0) {
//			SDL_RenderDrawLine(renderer, prevPoint.x, prevPoint.y, val.x, val.y);
//		}
//		prevPoint = val;
//	}
//}
//
//void Framework::draw_dist(float chiSquareVal, float meanX, float meanY, float sXX, float sYY, float sXY, glm::vec3 color) {
//	/*def plotErrorEllipseSigLvl(x_t, Sig_t, sig_lvl, conf_str, ax) :
//		st_sig = Sig_t[:2, : 2]
//		w, v = np.linalg.eig(st_sig)
//		print(w, v)
//		#w = abs(w) #TODO: Is this a hack ?
//		w = np.clip(w, 0, 9e9)
//		ang = math.atan2(v[1][0], v[0][0])
//		ell = matplotlib.patches.Ellipse(xy = (x_t[0], x_t[1]), width = 2.0 * math.sqrt(sig_lvl * w[0]), height = 2.0 * math.sqrt(sig_lvl * w[1]), angle = np.rad2deg(ang), edgecolor = conf_str, fc = 'None', lw = 1)
//		ax.add_patch(ell)*/
//
//		// Setting the color to be RED with 100% opaque (0% trasparent).
//	SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 255);
//
//	Eigen::MatrixXd A(2, 2);
//	A(0, 0) = sXX;
//	A(0, 1) = sXY;
//	A(1, 0) = sXY;
//	A(1, 1) = sYY;
//
//	Eigen::EigenSolver<Eigen::Matrix<double, 2, 2> > s(A);
//	auto eigenVecs = s.eigenvectors();
//	auto eigenVals = s.eigenvalues();
//
//	Eigen::Vector2cd vec1 = Eigen::Vector2cd(eigenVecs(0, 1), eigenVecs(0, 0));
//	Eigen::Vector2cd vec2 = Eigen::Vector2cd(eigenVecs(1, 1), eigenVecs(1, 0));
//
//	double angle = atan2(sqrt(eigenVecs(0, 1).real() * eigenVecs(0, 1).real()), sqrt(eigenVecs(0, 0).real() * eigenVecs(0, 0).real()));
//
//	//Shift the angle to the [0, 2pi] interval instead of [-pi, pi]
//	if (angle < 0)
//		angle += 6.28318530718;
//
//	//Conver to degrees instead of radians
//	angle = 180 * angle / 3.14159265359;
//
//	//Calculate the size of the minor and major axes
//	double halfmajoraxissize = sqrt(chiSquareVal * eigenVals(0).real());
//	double halfminoraxissize = sqrt(chiSquareVal * eigenVals(1).real());
//
//	// Drawing circle
//	for (int x = meanX - halfminoraxissize; x <= meanX + halfminoraxissize; x++) {
//		for (int y = meanY - halfmajoraxissize; y <= meanY + halfminoraxissize; y++) {
//			/*if ((std::pow(meanY - y, 2) + std::pow(meanX - x, 2)) <=
//				std::pow(radius_, 2)) {
//				SDL_RenderDrawPoint(renderer, x, y);
//			}*/
//		}
//	}
//
//}
//
//void Framework::present_render() {
//	SDL_RenderPresent(renderer);
//}



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

	//// Creating the object by passing Height and Width value.
	//Framework fw(600, 600);

	////draw the tree
	//fw.draw_tree(myTree);
	//// Calling the function that draws circle.
	//fw.draw_circle(mInitPos.x, mInitPos.y, 5, glm::vec3(255.f, 0.f, 0.f));
	//fw.draw_circle(mGoalPos.x, mGoalPos.y, 5, glm::vec3(255.f, 0.f, 0.f));

	//for (auto obs : mObstacles) {
	//	fw.draw_circle(obs.first.x, obs.first.y, obs.second, glm::vec3(0.f, 0.f, 255.f));
	//}

	////draw solution
	//fw.draw_solution(mSolutionPath);
	////show
	//fw.present_render();



	//SDL_Event event = SDL_Event();    // Event variable

	//// Below while loop checks if the window has terminated using close in the
	////  corner.
	//while (!(event.type == SDL_QUIT)) {
	//	SDL_Delay(10);  // setting some Delay
	//	SDL_PollEvent(&event);  // Catching the poll event.
	//}

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
	if (diffVec.length() <= 1.0) {
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
		if ((obstacle.first - nearby).length() <= obstacle.second) {
			return glm::vec2(-1.0, -1.0);
		}

		float a = diffVec.length() * diffVec.length();
		float b = -2 * dot((obstacle.first - nearby), diffVec);
		float c = (nearby - obstacle.first).length() * (nearby - obstacle.first).length() - obstacle.second * obstacle.second;

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
	while ((mGoalPos - newNode->mPosition).length() > 10.f) {
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
	while ((mGoalPos - newNode->mPosition).length() > 10.f) {

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


