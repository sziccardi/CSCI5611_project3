#pragma once

#include <glm/glm.hpp>

using namespace std;

struct Vertex {
	Vertex() {};
	Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex, glm::vec3 vel, glm::vec3 acc) {
		mPosition = pos;
		mNormal = norm;
		mTexCoord = tex;

		mVelocity = vel;
		mAcceleration = acc;
	}
	glm::vec3 mPosition = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 mNormal = glm::vec3(0.f, 0.f, 0.f);
	glm::vec2 mTexCoord = glm::vec2(0.f, 0.f);

	glm::vec3 mVelocity = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 mAcceleration = glm::vec3(0.f, 0.f, 0.f);
};