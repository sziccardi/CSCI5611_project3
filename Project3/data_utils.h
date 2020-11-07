#pragma once
#include <cmath>
#include <vector>
#include <unordered_map>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/polar_coordinates.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <algorithm>

using namespace std;

namespace std {

	template <>
	struct hash<glm::vec2>
	{
		std::size_t operator()(const glm::vec2& k) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			// Compute individual hash values for first,
			// second and third and combine them using XOR
			// and bit shifting:

			return ((hash<float>()(k.x)
				^ (hash<float>()(k.y) << 1)) >> 1);
		}
	};

}

class Node {
public:
	Node* mParent = nullptr;
	glm::vec2 mPosition = glm::vec2(-1, -1);
	float mCost = 0.f;
	vector<Node*> mConnectedNodes;

	Node(glm::vec2 position, Node* parent, float newCost) {
		mPosition = position;
		mParent = parent;
		if (mParent) mCost = mParent->mCost + newCost;
		else mCost = newCost;
	}
	~Node() {
		
	}

	void addConnection(Node* newNode) {
		mConnectedNodes.push_back(newNode);
	}
};

class Tree {

public:
	Tree() {
		myList = unordered_map<glm::vec2, Node*>();
	}
	~Tree() {
		for (auto it : myList) {
			delete(it.second);
		}
		myList.clear();
	}

	unordered_map<glm::vec2, Node*> getList() { return myList; }

	Node* getNode(glm::vec2 pos) {
		return myList.at(pos);
	}

	void addVertex(Node* myNode) {
		if (!myNode) {
			cout << "You tried to add an empty node! So naughty.." << endl;
			return;
		}
		myList.insert(make_pair(myNode->mPosition, myNode));
	}

	void addEdge(Node* source, Node* destination) {
		if (!source || !destination) {
			cout << "You tried to connnect empty nodes! So naughty.." << endl;
			return;
		}

		auto finding = myList.find(source->mPosition);
		if (finding == myList.end()) {
			cout << "Couldn't add that edge because its not from a real vertex." << endl;
			return;
		}
		finding = myList.find(destination->mPosition);
		if (finding != myList.end()) {
			addVertex(destination);
		}

		source->addConnection(destination);
	}

	Node* getNearestNode(glm::vec2 pointC) {
		float delta = 10000000000000.;
		Node* nearest = new Node(glm::vec2(-1, -1), nullptr, 0.f);
		for (auto myPair : myList) {
			auto actualPos = glm::vec2(myPair.first.x, myPair.first.y);
			float tempDelta = glm::length(actualPos - pointC);
			if (tempDelta < delta) {
				delta = tempDelta;
				nearest = myPair.second;
			}
		}
		return nearest;
	}

	vector<Node*> getNearNodes(glm::vec2 pointC, float searchRad) {
		vector<Node*> list;
		for (auto myPair : myList) {
			auto actualPos = glm::vec2(myPair.first.x, myPair.first.y);
			float tempDelta = glm::length(actualPos - pointC);
			if (tempDelta < searchRad) {
				list.push_back(myPair.second);
			}
		}
		return list;
	}

	void clearDeadLeaves() {
		auto it = myList.begin();
		while (it != myList.end())
		{
			// remove odd numbers
			if (!it->second->mParent) {
				// erase() invalidates the iterator, use returned iterator
				clearChildren(it->second);
				it = myList.erase(it);
			}
			// Notice that iterator is incremented only on the else part (why?)
			else {
				++it;
			}
		}
	}

	int getTreeSize() { return myList.size(); }

private:
	unordered_map<glm::vec2, Node*> myList;

	void clearChildren(Node* parent) {
		auto it = parent->mConnectedNodes.begin();
		while (it != parent->mConnectedNodes.end())
		{
			// remove odd numbers
			if (!(*it)->mParent) {
				// erase() invalidates the iterator, use returned iterator
				clearChildren((*it));
				auto toDel = (*it);
				removeMeFromMap(*it);
				it = parent->mConnectedNodes.erase(it);
				delete(toDel);
			}
			// Notice that iterator is incremented only on the else part (why?)
			else {
				++it;
			}
		}

	}

	void removeMeFromMap(Node* node) {
		auto it = myList.begin();
		while (it != myList.end())
		{
			// remove odd numbers
			if (it->second == node) {
				// erase() invalidates the iterator, use returned iterator
				it = myList.erase(it);
			}
			// Notice that iterator is incremented only on the else part (why?)
			else {
				++it;
			}
		}
	}
};