#pragma once

#include <vector>
#include <GLM.h>

class Mesh
{
public:
	Mesh();

	void buildCube(float size, glm::vec3 position, glm::vec3 color);
	void buildPlane(float width, float length, glm::vec3 position, glm::vec3 color);
	void buildSphere(float radius, glm::vec3 position, glm::vec3 color);

	int size() const { return vertices.size(); };

	const std::vector<glm::vec3>& getVertices() const { return vertices; }
	const std::vector<glm::vec3>& getNormals() const { return normals; }
	const std::vector<glm::vec3>& getColors() const { return colors; }
	const std::vector<int>& getObjectsIndexes() const { return objectsOffsets; }

private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> colors;
	std::vector<int> objectsOffsets;
};

