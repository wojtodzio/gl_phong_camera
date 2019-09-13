#include <Transform.h>

Transform::Transform() :
	position(glm::vec3(0, 0, 0)),
	rotation(glm::quat(0, 0, 0, 1)),
	scale(glm::vec3(1, 1, 1))
{
	updateMatrix();
}

void Transform::updateMatrix()
{
	glm::mat4 translationMat(1);
	translationMat = glm::translate(position);

	glm::mat4 rotationMat = glm::mat4_cast(rotation);

	glm::mat4 scaleMat(1);
	scaleMat = glm::scale(scaleMat, scale);

	// apply in order: scale, rotate, translate
	matrix = translationMat * rotationMat * scaleMat;
}
