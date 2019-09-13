#include <Camera.h>

const auto step = glm::vec3(0, 0, -1);
const auto up = glm::vec3(0, 1, 0);
const auto mouseSensitivity = glm::vec2(0.05f, 0.08f);

Camera::Camera(double initPosX, double initPosY) :
	zNear(0.1f),
	zFar(1000.0f),
	pixelsPerUnit(100),
	fov(60) {
	cursorLast = glm::vec2(initPosX, initPosY);
}

void Camera::setPerspective(std::uint32_t width, std::uint32_t height, float zNear, float zFar)
{
	this->zFar = zFar;
	this->zNear = zNear;
	projection = glm::perspectiveFov(glm::radians(fov), (float)width, (float)height, zNear, zFar);

	update();
}

void Camera::lookAt(const glm::vec3 & targetPosition)
{
	auto view = glm::lookAt(position, targetPosition, glm::vec3(0, 1.0f, 0));
	rotation = glm::normalize(glm::quat_cast(view));

	update();
}

void Camera::lookAround(float pitch, float yaw)
{
	auto anglesPitch = glm::quat(glm::vec3(glm::radians(pitch), 0.0f, 0.0f));
	auto anglesYaw = glm::quat(glm::vec3(0.0f, glm::radians(yaw), 0.0f));

	rotation = glm::quat(anglesPitch * rotation * anglesYaw);

	update();
}

void Camera::moveAndLookAt(const glm::vec3 & position, const glm::vec3 & targetPosition)
{
	auto view = glm::lookAt(position, targetPosition, glm::vec3(0, 1.0f, 0));

	this->position = position;
	this->rotation = glm::normalize(glm::quat_cast(view));

	update();
}

void Camera::update()
{
	auto translationMat = glm::mat4(1);
	translationMat = glm::translate(translationMat, glm::vec3(-position.x, -position.y, -position.z));

	auto rotationMat = glm::mat4_cast(rotation);

	view = rotationMat * translationMat;
	viewProjection = projection * view;
}

void Camera::updateCursor(double xpos, double ypos)
{
	auto newCursor = glm::vec2(xpos, ypos);
	auto cursorDelta = cursorLast - newCursor;
	auto cameraDelta = cursorDelta * mouseSensitivity;

	lookAround(cameraDelta[1], cameraDelta[0]);

	cursorLast = newCursor;
}

void Camera::moveForward(float speed)
{
	auto direction = glm::conjugate(rotation) * step;
	position += direction * speed;
	update();
}

void Camera::moveBackward(float speed)
{
	auto direction = glm::conjugate(rotation) * step;
	position -= direction * speed;
	update();
}

void Camera::moveLeft(float speed)
{
	auto direction = glm::conjugate(rotation) * step;
	direction = glm::cross(direction, up);
	position -= direction * speed;
	update();
}

void Camera::moveRight(float speed)
{
	auto direction = glm::conjugate(rotation) * step;
	direction = glm::cross(direction, up);
	position += direction * speed;
	update();
}

void Camera::zoomIn(float speed)
{
	fov += speed;
	if (fov >= 180)
		fov = 180;
	update();
}

void Camera::zoomOut(float speed)
{
	fov -= speed;
	if (fov <= 0)
		fov = 0.1;
	update();
} 