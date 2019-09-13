#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Transform.h>
#include <Camera.h>

const GLfloat ONE = 1.0f;

std::string VERTEX_SHADER = R"(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec3 FragPos;
out vec3 Normal;
out vec3 VertexColor;

void main()
{
    FragPos = vec3(M * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(M))) * aNormal;  
	VertexColor = aColor;
    
    gl_Position = P * V * vec4(FragPos, 1.0);
}

)";

std::string FRAGMENT_SHADER = R"(
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;  
in vec3 VertexColor;
  
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * VertexColor;
    FragColor = vec4(result, 1.0);
} 

)";

using Mesh = struct
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> colors;
};

Mesh buildPlane(float width, float length) {
	Mesh mesh;

	float height = 0;
	float halfWidth = width * 0.5f;
	float halfLength = length * 0.5f;

	mesh.vertices = {
		glm::vec3(-halfWidth, height, -halfLength),
		glm::vec3(halfWidth, height, -halfLength),
		glm::vec3(-halfWidth, height, halfLength),

		glm::vec3(halfWidth, height, -halfLength),
		glm::vec3(halfWidth, height, halfLength),
		glm::vec3(-halfWidth, height, halfLength)
	};

	for (auto i = 0; i < mesh.vertices.size(); ++i)
		mesh.colors.push_back(glm::vec3(1, 0, 0));

	mesh.normals.resize(mesh.vertices.size());

	for (auto i = 0; i < mesh.vertices.size(); i += 3)
	{
		auto normal = glm::triangleNormal(
			mesh.vertices[i + 0],
			mesh.vertices[i + 1],
			mesh.vertices[i + 2]
		);

		mesh.normals[i + 0] = normal;
		mesh.normals[i + 1] = normal;
		mesh.normals[i + 2] = normal;
	}

	return mesh;
}

Mesh buildCube(float size)
{
	Mesh mesh;

	float half = size * 0.5f;
	float top = size;
	float bottom = 0;

	mesh.vertices = {
		// Face 1
		// left bottom
		glm::vec3(-half, bottom, -half),
		glm::vec3(-half, bottom,  half),
		glm::vec3(-half, -top, half),
		// left top
		glm::vec3(-half, bottom, -half),
		glm::vec3(-half, -top, half),
		glm::vec3(-half, -top,-half),
		// Face 2
		// front bottom
		glm::vec3(-half, bottom,  half),
		glm::vec3(half, bottom,  half),
		glm::vec3(-half, -top, half),
		// front top
		glm::vec3(half, bottom,  half),
		glm::vec3(half, -top, half),
		glm::vec3(-half, -top, half),
		// Face 3
		// right bottom
		glm::vec3(half, bottom,  half),
		glm::vec3(half, bottom, -half),
		glm::vec3(half, -top, half),
		// right top
		glm::vec3(half, bottom, -half),
		glm::vec3(half, -top,-half),
		glm::vec3(half, -top, half),
		// Face 4
		// back bottom
		glm::vec3(half, bottom, -half),
		glm::vec3(-half, bottom, -half),
		glm::vec3(half, -top, -half),
		// back top
		glm::vec3(-half, bottom, -half),
		glm::vec3(-half, -top, -half),
		glm::vec3(half, -top, -half),
		// Face 5
		// up bottom
		glm::vec3(half, -top,-half),
		glm::vec3(-half, -top,-half),
		glm::vec3(half, -top, half),
		// up top
		glm::vec3(-half, -top,-half),
		glm::vec3(-half, -top, half),
		glm::vec3(half, -top, half),
		// Face 6
		// down bottom
		glm::vec3(-half, bottom,  half),
		glm::vec3(-half, bottom, -half),
		glm::vec3(half, bottom,  half),
		// down top
		glm::vec3(-half, bottom, -half),
		glm::vec3(half, bottom, -half),
		glm::vec3(half, bottom,  half)
	};

	for (auto i = 0; i < mesh.vertices.size(); ++i)
		mesh.colors.push_back(glm::vec3(1, 0, 0));

	mesh.normals.resize(mesh.vertices.size());

	for (auto i = 0; i < mesh.vertices.size(); i += 3)
	{
		auto normal = glm::triangleNormal(
			mesh.vertices[i + 0],
			mesh.vertices[i + 1],
			mesh.vertices[i + 2]
		);

		mesh.normals[i + 0] = normal;
		mesh.normals[i + 1] = normal;
		mesh.normals[i + 2] = normal;
	}

	return mesh;
}

void checkCompilationStatus(std::uint32_t shaderID)
{
	GLint isCompiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_FALSE)
	{
		GLint type = 0;
		glGetShaderiv(shaderID, GL_SHADER_TYPE, &type);

		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		std::string kind;
		switch (type)
		{
		case GL_VERTEX_SHADER:
			kind = "VERTEX";
			break;

		case GL_FRAGMENT_SHADER:
			kind = "FRAGMENT";
			break;

		default:
			kind = "???";
		}

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, &errorLog[0]);
		std::cerr << "SHADER " << kind << std::string(errorLog.begin(), errorLog.end()) << "\n";
	}
	else {
		std::cout << "Shader compiled\n";
	}
}

int main(int argc, char* argv[])
{
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize";
		return 1;
	}

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

	GLFWwindow* window = glfwCreateWindow(1280, 1024, "Camera", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to init GLEW";
		return 1;
	}

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	std::uint32_t vao;
	std::uint32_t vbo;
	std::uint32_t nbo;
	std::uint32_t cbo;

	Mesh mesh;
	Mesh cubeSmall = buildCube(1);
	Mesh cubeBig = buildCube(3);
	Mesh plane = buildPlane(10, 10);

	mesh = cubeSmall;

	mesh.vertices.insert(mesh.vertices.end(), plane.vertices.begin(), plane.vertices.end());
	mesh.colors.insert(mesh.colors.end(), plane.colors.begin(), plane.colors.end());
	mesh.normals.insert(mesh.normals.end(), plane.normals.begin(), plane.normals.end());

	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// vertex buffer
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(), &mesh.vertices[0], GL_STATIC_DRAW);

	// normal buffer
	glGenBuffers(1, &nbo);
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.normals.size(), &mesh.normals[0], GL_STATIC_DRAW);

	// color buffer
	glGenBuffers(1, &cbo);
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.colors.size(), &mesh.colors[0], GL_STATIC_DRAW);

	// vertex attribute
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// normal attribute
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// color attribute
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// ubind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// shaders
	std::uint32_t vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	std::uint32_t fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	std::uint32_t programID = glCreateShader(GL_FRAGMENT_SHADER);

	const auto vertexShaderSource = VERTEX_SHADER.c_str();
	const auto fragmentShaderSource = FRAGMENT_SHADER.c_str();

	glShaderSource(vertexShaderID, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShaderID);
	checkCompilationStatus(vertexShaderID);

	glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShaderID);
	checkCompilationStatus(fragmentShaderID);

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	glUseProgram(programID);

	auto lightColorLocation = glGetUniformLocation(programID, "lightColor");
	auto lightPosLocation = glGetUniformLocation(programID, "lightPos");
	auto viewPosLocation = glGetUniformLocation(programID, "viewPosLocation");
	auto modelLocation = glGetUniformLocation(programID, "M");
	auto viewLocation = glGetUniformLocation(programID, "V");
	auto projectionLocation = glGetUniformLocation(programID, "P");

	// mark for deletion
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(0);

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	Camera camera(xpos, ypos);
	Transform boxTransform;

	camera.moveAndLookAt(glm::vec3(12, 18, 12), glm::vec3(0, 0, 0));

	auto lastFrameTime = glfwGetTime();

	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	while (!glfwWindowShouldClose(window))
	{

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		camera.setPerspective(width, height);

		float bkgColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, bkgColor);
		glClearBufferfv(GL_DEPTH, 0, &ONE);

		glUseProgram(programID);
		glBindVertexArray(vao);

		boxTransform.position = glm::vec3(2, 0, 0);
		boxTransform.updateMatrix();

		auto model = boxTransform.getModelMatrix();
		auto view = camera.getViewMatrix();
		auto projection = camera.getProjection();

		glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLocation, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(viewPosLocation, camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDrawArrays(GL_TRIANGLES, 36, 42);

		boxTransform.position = glm::vec3(-2, 0, 0);
		boxTransform.updateMatrix();
		model = boxTransform.getModelMatrix();
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDrawArrays(GL_TRIANGLES, 36, 72);
		//glDrawArrays(GL_TRIANGLES, 0, 18);
		//glUniform3f(lightColorLocation, -1.0f, 6.0f, -1.0f);
		//glDrawArrays(GL_TRIANGLES, 18, 36);

		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window);
		glfwPollEvents();

		auto now = glfwGetTime();
		auto dt = now - lastFrameTime;
		lastFrameTime = now;

		float moveSpeed = 10 * dt;

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.moveForward(moveSpeed);

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.moveBackward(moveSpeed);

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.moveRight(moveSpeed);

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.moveLeft(moveSpeed);

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			camera.moveUp(moveSpeed);

		float zoomSpeed = 100 * dt;

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			camera.zoomIn(zoomSpeed);

		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			camera.zoomOut(zoomSpeed);

		glfwGetCursorPos(window, &xpos, &ypos);

		camera.updateCursor(xpos, ypos);
	}

	glUseProgram(0);
	glDeleteProgram(programID);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}