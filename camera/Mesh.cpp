#include <Mesh.h>
#include <iostream>

Mesh::Mesh() {}

void Mesh::buildCube(float size, glm::vec3 position, glm::vec3 color, float shininess) {
	float half = size * 0.5f;
	float top = size + position.y;
	float bottom = position.y;

	auto oldSize = vertices.size();

	auto cubeVertices = {
		// Face 1
		// left bottom
		glm::vec3(-half + position.x, -top, half + position.z),
		glm::vec3(-half + position.x, -bottom, half + position.z),
		glm::vec3(-half + position.x, -bottom, -half + position.z),
		// left top
		glm::vec3(-half + position.x, -top,-half + position.z),
		glm::vec3(-half + position.x, -top, half + position.z),
		glm::vec3(-half + position.x, -bottom, -half + position.z),
		// Face 2
		// front bottom
		glm::vec3(-half + position.x, -top, half + position.z),
		glm::vec3(half + position.x, -bottom, half + position.z),
		glm::vec3(-half + position.x, -bottom, half + position.z),
		// front top
		glm::vec3(-half + position.x, -top, half + position.z),
		glm::vec3(half + position.x, -top, half + position.z),
		glm::vec3(half + position.x, -bottom, half + position.z),
		// Face 3
		// right bottom
		glm::vec3(half + position.x, -top, half + position.z),
		glm::vec3(half + position.x, -bottom, -half + position.z),
		glm::vec3(half + position.x, -bottom, half + position.z),
		// right top
		glm::vec3(half + position.x, -top, half + position.z),
		glm::vec3(half + position.x, -top,-half + position.z),
		glm::vec3(half + position.x, -bottom, -half + position.z),
		// Face 4
		// back bottom
		glm::vec3(half + position.x, -top, -half + position.z),
		glm::vec3(-half + position.x, -bottom, -half + position.z),
		glm::vec3(half + position.x, -bottom, -half + position.z),
		// back top
		glm::vec3(half + position.x, -top, -half + position.z),
		glm::vec3(-half + position.x, -top, -half + position.z),
		glm::vec3(-half + position.x, -bottom, -half + position.z),
		// Face 5
		// up bottom
		glm::vec3(half + position.x, -top, half + position.z),
		glm::vec3(-half + position.x, -top, -half + position.z),
		glm::vec3(half + position.x, -top, -half + position.z),
		// up top
		glm::vec3(half + position.x, -top, half + position.z),
		glm::vec3(-half + position.x, -top, half + position.z),
		glm::vec3(-half + position.x, -top,-half + position.z),
		// Face 6
		// down bottom
		glm::vec3(half + position.x, -bottom, half + position.z),
		glm::vec3(-half + position.x, -bottom, -half + position.z),
		glm::vec3(-half + position.x, -bottom, half + position.z),
		// down top
		glm::vec3(half + position.x, -bottom, half + position.z),
		glm::vec3(half + position.x, -bottom, -half + position.z),
		glm::vec3(-half + position.x, -bottom, -half + position.z)
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

	objectsOffsets.push_back(vertices.size());
	objectsShininess.push_back(shininess);
}

void Mesh::buildPlane(float width, float length, glm::vec3 position, glm::vec3 color, float shininess) {
	float height = position.y;
	float halfWidth = width * 0.5f;
	float halfLength = length * 0.5f;

	auto oldSize = vertices.size();

	auto planeVertices = {
		glm::vec3(-halfWidth + position.x, height, -halfLength + position.z),
		glm::vec3(halfWidth + position.x, height, -halfLength + position.z),
		glm::vec3(-halfWidth + position.x, height, halfLength + position.z),

		glm::vec3(halfWidth + position.x, height, -halfLength + position.z),
		glm::vec3(halfWidth + position.x, height, halfLength + position.z),
		glm::vec3(-halfWidth + position.x, height, halfLength + position.z)
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

	objectsOffsets.push_back(vertices.size());
	objectsShininess.push_back(shininess);
}

void Mesh::buildSphere(float radius, glm::vec3 position, glm::vec3 color, float shininess)
{
	float x, y, z, xy;                              // vertex position

	auto oldSize = vertices.size();

	int sectorCount = 250;
	int stackCount = 250;

	float sectorStep = 2 * glm::pi<float>() / sectorCount;
	float stackStep = glm::pi<float>() / stackCount;
	float sectorAngle, stackAngle;

	std::vector<glm::vec3> sphereVertices;

	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = glm::pi<float>() / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle) - position.z;              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle) - position.x;             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle) - position.y;             // r * cos(u) * sin(v)
			sphereVertices.push_back(glm::vec3(x, y, z));
		}
	}

	int k1, k2;
	for (int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				vertices.push_back(sphereVertices[k1]);
				colors.push_back(color);
				vertices.push_back(sphereVertices[k2]);
				colors.push_back(color);
				vertices.push_back(sphereVertices[k1 + 1]);
				colors.push_back(color);
			}

			// k1+1 => k2 => k2+1
			if (i != (stackCount - 1))
			{
				vertices.push_back(sphereVertices[k1 + 1]);
				colors.push_back(color);
				vertices.push_back(sphereVertices[k2]);
				colors.push_back(color);
				vertices.push_back(sphereVertices[k2 + 1]);
				colors.push_back(color);
			}
		}
	}

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

	objectsOffsets.push_back(vertices.size());
	objectsShininess.push_back(shininess);
}
