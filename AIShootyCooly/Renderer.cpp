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
	window_ = glfwCreateWindow(WindowWidth_, WindowHeight_, "AI Shooty Cooly", nullptr, nullptr);

	glfwMakeContextCurrent(window_);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Setup a sample program
	program_.gen("shaders/pass.vert", "shaders/pass.frag");

	viewProjectionLocation_ = glGetUniformLocation(program_, "VP");
	modelMatrixLocation_ = glGetUniformLocation(program_, "M");

	// Bind textures
	glUniform1i(glGetUniformLocation(program_, "diffuseTex"), 0);
}

Renderer::~Renderer() {
	glfwDestroyWindow(window_);
	glfwTerminate();
}

bool Renderer::isOpen() const {
	return !glfwWindowShouldClose(window_);
}

void Renderer::updateLocation(float dt) {
	static auto MouseSpeed = 0.12f, Speed = 10.0f;

	double xpos, ypos;
	glfwGetCursorPos(window_, &xpos, &ypos);
	xpos = WindowWidth_ / 2.0 - xpos;
	ypos = WindowHeight_ / 2.0 - ypos;
	glfwSetCursorPos(window_, WindowWidth_ / 2.0, WindowHeight_ / 2.0);

	horizontalAngle_ += MouseSpeed * dt * float(xpos);
	verticalAngle_ += MouseSpeed * dt * float(ypos);
	if (verticalAngle_ < -M_PI / 2)
		verticalAngle_ = -float(M_PI) / 2;

	else if (verticalAngle_ > M_PI / 2)
		verticalAngle_ = float(M_PI) / 2;

	if (horizontalAngle_ < 0 || horizontalAngle_ > 2 * M_PI) {
		horizontalAngle_ = fmod(horizontalAngle_, 2 * float(M_PI));
	}

	glm::vec3 direction(
		cos(verticalAngle_) * sin(horizontalAngle_),
		sin(verticalAngle_),
		cos(verticalAngle_) * cos(horizontalAngle_)
		);
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle_ - M_PI / 2.0f),
		0,
		cos(horizontalAngle_ - M_PI / 2.0f)
		);
	glm::vec3 forward = glm::vec3(
		sin(horizontalAngle_),
		0,
		cos(horizontalAngle_)
		);

	glm::vec3 up = glm::cross(right, direction);
	// Move forward
	if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos_ += forward * dt * Speed;
	}
	// Move backward
	if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos_ -= forward * dt * Speed;
	}
	// Strafe right
	if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos_ += right * dt * Speed;
	}
	// Strafe left
	if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos_ -= right * dt * Speed;
	}
	// Float up
	if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS) {
		cameraPos_ += glm::vec3(0, 1, 0) * dt * Speed;
	}
	// Float down
	if (glfwGetKey(window_, GLFW_KEY_Z) == GLFW_PRESS) {
		cameraPos_ -= glm::vec3(0, 1, 0) * dt * Speed;
	}

	if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window_, true);
	}

	cameraMatrix_ = glm::lookAt(cameraPos_,
								cameraPos_ + direction,
								up);
}

void Renderer::draw(const Model & model) {
	static auto projection = glm::perspective<float>(toRad(50), float(WindowWidth_) / WindowHeight_,
													 0.1f, 50);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program_);
	glUniformMatrix4fv(viewProjectionLocation_, 1, GL_FALSE, glm::value_ptr(projection * cameraMatrix_));
	glViewport(0, 0, WindowWidth_, WindowHeight_);

	model.draw(modelMatrixLocation_);

	glfwSwapBuffers(window_);
	glfwPollEvents();
}
