#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Transform.h>
#include <Camera.h>
#include <Mesh.h>

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
uniform float specularStrength;

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
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * VertexColor;
    FragColor = vec4(result, 1.0);
} 

)";

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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	std::uint32_t vao;
	std::uint32_t vbo;
	std::uint32_t nbo;
	std::uint32_t cbo;

	Mesh debugMesh;
	debugMesh.buildSphere(0.5f, glm::vec3(0), glm::vec3(1, 1, 1), 0);

	Mesh mesh;
	// plane
	mesh.buildPlane(2, 20, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), 0); // road
	mesh.buildPlane(9, 20, glm::vec3(5.5, 0, 0), glm::vec3(0, 0.5f, 0), 5.0f);
	mesh.buildPlane(9, 20, glm::vec3(-5.5, 0, 0), glm::vec3(0, 0.5f, 0), 0);

	// Four store multi color building
	mesh.buildCube(2.5, glm::vec3(4, 0, 0), glm::vec3(1, 0, 0), 0.5f);
	mesh.buildCube(2, glm::vec3(4, 2.25f, 0), glm::vec3(0, 1, 0), 0.5f);
	mesh.buildCube(1.5f, glm::vec3(4, 4.25f, 0), glm::vec3(0, 0, 1), 0.5f);
	mesh.buildCube(1, glm::vec3(4, 5.75f, 0), glm::vec3(0.5f, 0.5f, 0.5f), 0.5f);

	// One store red large building
	mesh.buildCube(5, glm::vec3(-5, 0, 0), glm::vec3(1, 0, 0), 0.5f);

	// Five store multi color building with a topping
	mesh.buildCube(2.5, glm::vec3(-4, 0, 5), glm::vec3(1, 0, 0), 0.5f);
	mesh.buildCube(2, glm::vec3(-4, 2.25f, 5), glm::vec3(0, 1, 0), 0.5f);
	mesh.buildCube(1.5f, glm::vec3(-4, 4.25f, 5), glm::vec3(0, 0, 1), 0.5f);
	mesh.buildCube(1, glm::vec3(-4, 5.75f, 5), glm::vec3(0.5f, 0.5f, 0.5f), 0.5f);
	mesh.buildCube(0.5, glm::vec3(-4, 6.75f, 5), glm::vec3(0.5f, 0.5f, 0), 0.5f);
	mesh.buildCube(0.1, glm::vec3(-4, 7.25f, 5), glm::vec3(0.0f, 1, 1), 0.5f);
	mesh.buildCube(0.1, glm::vec3(-4, 7.35f, 5), glm::vec3(0.0f, 1, 1), 0.5f);
	mesh.buildCube(0.1, glm::vec3(-4, 7.45f, 5), glm::vec3(0.0f, 1, 1), 0.5f);
	mesh.buildCube(0.1, glm::vec3(-4, 7.55f, 5), glm::vec3(0.0f, 1, 1), 0.5f);
	mesh.buildCube(0.1, glm::vec3(-4, 7.65f, 5), glm::vec3(0.0f, 1, 1), 0.5f);

	// Sphere
	mesh.buildSphere(1, glm::vec3(0, 5, 4), glm::vec3(1, 0, 0), 0.5f);

	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// vertex buffer
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.size(), &mesh.getVertices()[0], GL_DYNAMIC_DRAW);

	// normal buffer
	glGenBuffers(1, &nbo);
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.size(), &mesh.getNormals()[0], GL_DYNAMIC_DRAW);

	// color buffer
	glGenBuffers(1, &cbo);
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.size(), &mesh.getColors()[0], GL_DYNAMIC_DRAW);

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
	auto specularStrengthLocation = glGetUniformLocation(programID, "specularStrength");
	auto lightPosLocation = glGetUniformLocation(programID, "lightPos");
	auto viewPosLocation = glGetUniformLocation(programID, "viewPos");
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

	Transform lightTransform;
	lightTransform.position = glm::vec3(0.0f, 5.0f, 0.0f);

	float specular = 0.5f;

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

		boxTransform.position = glm::vec3(0, 0, 0);
		boxTransform.updateMatrix();

		auto view = camera.getViewMatrix();
		auto projection = camera.getProjection();
		auto model = boxTransform.getModelMatrix();

		glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
		glUniform1f(specularStrengthLocation, specular);
		glUniform3f(lightPosLocation, lightTransform.position.x, lightTransform.position.y, lightTransform.position.z);
		glUniform3f(viewPosLocation, camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.size(), &mesh.getVertices()[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, nbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.size(), &mesh.getNormals()[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, cbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.size(), &mesh.getColors()[0], GL_DYNAMIC_DRAW);

		auto objectsIndexes = mesh.getObjectsIndexes();
		auto objectsSpeculars = mesh.getObjectsSpeculars();
		int previousIndex = 0;
		for (auto i = 0; i < objectsIndexes.size(); ++i)
		{
			auto currentIndex = objectsIndexes[i];

			//glUniform1f(specularStrengthLocation, objectsSpeculars[i]);

			glDrawArrays(GL_TRIANGLES, previousIndex, currentIndex);

			previousIndex = currentIndex;
		}

		lightTransform.updateMatrix();
		model = lightTransform.getModelMatrix();
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * debugMesh.size(), &debugMesh.getVertices()[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, nbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * debugMesh.size(), &debugMesh.getNormals()[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, cbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * debugMesh.size(), &debugMesh.getColors()[0], GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, debugMesh.size());

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

		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
			lightTransform.position += glm::vec3(0, 0, -10) * (float)dt;

		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
			lightTransform.position += glm::vec3(0, 0, 10) * (float)dt;

		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
			lightTransform.position += glm::vec3(-10, 0, 0) * (float)dt;

		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
			lightTransform.position += glm::vec3(10, 0, 0) * (float)dt;

		if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
			lightTransform.position += glm::vec3(0, 10, 0) * (float)dt;

		if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
			lightTransform.position += glm::vec3(0, -10, 0) * (float)dt;

		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
			specular -= 0.8 * (float)dt;

		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
			specular += 0.8 * (float)dt;

		glfwGetCursorPos(window, &xpos, &ypos);

		camera.updateCursor(xpos, ypos);
	}

	glUseProgram(0);
	glDeleteProgram(programID);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}