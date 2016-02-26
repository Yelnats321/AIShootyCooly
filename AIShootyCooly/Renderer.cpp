#include "stdafx.h"
#include "Renderer.h"
#include "Model.h"

namespace {
void error_callback(int error, const char* desc) {
	std::cerr << "ERROR " << error << " - " << desc << std::endl;
}
float toRad(float deg) {
	return deg * float(M_PI) / 180.f;
}
}

Renderer::Renderer() {
	glfwInit();
	glfwSetErrorCallback(error_callback);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(WindowWidth, WindowHeight, "AI Shooty Cooly", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Setup a sample program
	program.gen("shaders/pass.vert", "shaders/pass.frag");

	viewProjectionLocation = glGetUniformLocation(program, "VP");
	modelMatrixLocation = glGetUniformLocation(program, "M");

	// Bind textures
	glUniform1i(glGetUniformLocation(program, "diffuseTex"), 0);
}

Renderer::~Renderer() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

bool Renderer::isOpen() const {
	return !glfwWindowShouldClose(window);
}

void Renderer::updateLocation(float dt) {
	static auto MouseSpeed = 0.12f, Speed = 10.0f;

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	xpos = WindowWidth / 2.0 - xpos;
	ypos = WindowHeight / 2.0 - ypos;
	glfwSetCursorPos(window, WindowWidth / 2.0, WindowHeight / 2.0);

	horizontalAngle += MouseSpeed * dt * float(xpos);
	verticalAngle += MouseSpeed * dt * float(ypos);
	if (verticalAngle < -M_PI / 2)
		verticalAngle = -float(M_PI) / 2;

	else if (verticalAngle > M_PI / 2)
		verticalAngle = float(M_PI) / 2;

	if (horizontalAngle < 0 || horizontalAngle > 2 * M_PI) {
		horizontalAngle = fmodf(horizontalAngle, 2 * float(M_PI));
	}

	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
		);
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - M_PI / 2.0f),
		0,
		cos(horizontalAngle - M_PI / 2.0f)
		);
	glm::vec3 forward = glm::vec3(
		sin(horizontalAngle),
		0,
		cos(horizontalAngle)
		);

	glm::vec3 up = glm::cross(right, direction);
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += forward * dt * Speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= forward * dt * Speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos += right * dt * Speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos -= right * dt * Speed;
	}
	// Float up
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		cameraPos += glm::vec3(0, 1, 0) * dt * Speed;
	}
	// Float down
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		cameraPos -= glm::vec3(0, 1, 0) * dt * Speed;
	}

	cameraMatrix = glm::lookAt(cameraPos,
							   cameraPos + direction,
							   up);
}

void Renderer::draw(const Model & model) {
	static auto projection = glm::perspective<float>(toRad(50), float(WindowWidth) / WindowHeight,
													 0.1f, 50);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	glUniformMatrix4fv(viewProjectionLocation, 1, GL_FALSE, glm::value_ptr(projection * cameraMatrix));
	glViewport(0, 0, WindowWidth, WindowHeight);

	model.draw(modelMatrixLocation);

	glfwSwapBuffers(window);
	glfwPollEvents();
}
