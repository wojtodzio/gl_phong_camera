#include <Mesh.h>
#include <iostream>

Mesh::Mesh() {}

void Mesh::buildCube(float size, glm::vec3 position, glm::vec3 color) {
	float half = size * 0.5f;
	float top = size + position[1];
	float bottom = position[1];
	std::cout << "top: " << top << "bottom: " << bottom;

	auto oldSize = vertices.size();

	auto cubeVertices = {
		// Face 1
		// left bottom
		glm::vec3(-half + position[0], -bottom, -half + position[2]),
		glm::vec3(-half + position[0], -bottom, half + position[2]),
		glm::vec3(-half + position[0], -top, half + position[2]),
		// left top
		glm::vec3(-half + position[0], -bottom, -half + position[2]),
		glm::vec3(-half + position[0], -top, half + position[2]),
		glm::vec3(-half + position[0], -top,-half + position[2]),
		// Face 2
		// front bottom
		glm::vec3(-half + position[0], -bottom, half + position[2]),
		glm::vec3(half + position[0], -bottom, half + position[2]),
		glm::vec3(-half + position[0], -top, half + position[2]),
		// front top
		glm::vec3(half + position[0], -bottom, half + position[2]),
		glm::vec3(half + position[0], -top, half + position[2]),
		glm::vec3(-half + position[0], -top, half + position[2]),
		// Face 3
		// right bottom
		glm::vec3(half + position[0], -bottom, half + position[2]),
		glm::vec3(half + position[0], -bottom, -half + position[2]),
		glm::vec3(half + position[0], -top, half + position[2]),
		// right top
		glm::vec3(half + position[0], -bottom, -half + position[2]),
		glm::vec3(half + position[0], -top,-half + position[2]),
		glm::vec3(half + position[0], -top, half + position[2]),
		// Face 4
		// back bottom
		glm::vec3(half + position[0], -bottom, -half + position[2]),
		glm::vec3(-half + position[0], -bottom, -half + position[2]),
		glm::vec3(half + position[0], -top, -half + position[2]),
		// back top
		glm::vec3(-half + position[0], -bottom, -half + position[2]),
		glm::vec3(-half + position[0], -top, -half + position[2]),
		glm::vec3(half + position[0], -top, -half + position[2]),
		// Face 5
		// up bottom
		glm::vec3(half + position[0], -top, -half + position[2]),
		glm::vec3(-half + position[0], -top, -half + position[2]),
		glm::vec3(half + position[0], -top, half + position[2]),
		// up top
		glm::vec3(-half + position[0], -top,-half + position[2]),
		glm::vec3(-half + position[0], -top, half + position[2]),
		glm::vec3(half + position[0], -top, half + position[2]),
		// Face 6
		// down bottom
		glm::vec3(-half + position[0], -bottom, half + position[2]),
		glm::vec3(-half + position[0], -bottom, -half + position[2]),
		glm::vec3(half + position[0], -bottom, half + position[2]),
		// down top
		glm::vec3(-half + position[0], -bottom, -half + position[2]),
		glm::vec3(half + position[0], -bottom, -half + position[2]),
		glm::vec3(half + position[0], -bottom, half + position[2])
	};

	vertices.insert(vertices.end(), cubeVertices.begin(), cubeVertices.end());

	for (auto i = 0; i < cubeVertices.size(); ++i)
		colors.push_back(color);

	normals.resize(vertices.size());

	for (auto i = oldSize; i < vertices.size(); i += 3)
	{
		auto normal = glm::triangleNormal(
			vertices[i + 0],
			vertices[i + 1],
			vertices[i + 2]
		);

		normals[i + 0] = normal;
		normals[i + 1] = normal;
		normals[i + 2] = normal;
	}

	objectsIndexes.push_back(glm::vec2(oldSize, vertices.size()));

}

void Mesh::buildPlane(float width, float length, glm::vec3 position, glm::vec3 color) {
	float height = position[1];
	float halfWidth = width * 0.5f;
	float halfLength = length * 0.5f;

	auto oldSize = vertices.size();

	auto planeVertices = {
		glm::vec3(-halfWidth + position[0], height, -halfLength + position[1]),
		glm::vec3(halfWidth + position[0], height, -halfLength + position[1]),
		glm::vec3(-halfWidth + position[0], height, halfLength + position[1]),

		glm::vec3(halfWidth + position[0], height, -halfLength + position[1]),
		glm::vec3(halfWidth + position[0], height, halfLength + position[1]),
		glm::vec3(-halfWidth + position[0], height, halfLength + position[1])
	};

	vertices.insert(vertices.end(), planeVertices.begin(), planeVertices.end());

	for (auto i = 0; i < planeVertices.size(); ++i)
		colors.push_back(color);

	normals.resize(vertices.size());

	for (auto i = oldSize; i < vertices.size(); i += 3)
	{
		auto normal = glm::triangleNormal(
			vertices[i + 0],
			vertices[i + 1],
			vertices[i + 2]
		);

		normals[i + 0] = normal;
		normals[i + 1] = normal;
		normals[i + 2] = normal;
	}

	objectsIndexes.push_back(glm::vec2(oldSize, vertices.size()));
}
