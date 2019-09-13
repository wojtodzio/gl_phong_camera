#pragma once

#include <GLM.h>

class Transform
{
public:
	Transform();

	void updateMatrix();

	const glm::mat4& getModelMatrix() const { return matrix; }

public:
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

private:
	glm::mat4 matrix;
};
