#pragma once

#include <GLM.h>

class Camera
{
public:
	Camera(double initPosX, double initPosY);
	void setPerspective(std::uint32_t width, std::uint32_t height, float zNear = 0.01f, float zFar = 100.f);

	void lookAt(const glm::vec3& targetPosition);
	void lookAround(float pitch, float yaw);
	void moveAndLookAt(const glm::vec3& position, const glm::vec3& targetPosition);

	void updateCursor(double xpos, double ypos);

	void moveForward(float speed);
	void moveBackward(float speed);
	void moveLeft(float speed);
	void moveRight(float speed);
	void moveUp(float speed);

	void zoomIn(float speed);
	void zoomOut(float speed);

	const glm::mat4& getProjection() const { return projection; }
	const glm::mat4& getViewMatrix() const { return view; }
	const glm::vec3& getPosition() const { return position; }
	void setPosition(const glm::vec3& position) { this->position = position; }

private:
	void update();

private:
	float zNear, zFar;
	float fov;
	float pixelsPerUnit;

	glm::vec2 cursorLast;

	glm::quat rotation;
	glm::vec3 position;
	glm::mat4 view;
	glm::mat4 viewProjection;
	glm::mat4 projection;
};


