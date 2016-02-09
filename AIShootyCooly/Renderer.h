#pragma once

class Renderer {
	GLFWwindow * window;
	static const int WindowWidth = 800, WindowHeight = 600;
	gl::Program program;
	GLuint viewProjectionLocation;

	glm::vec3 cameraPos;
	float horizontalAngle = 0.f, verticalAngle = 0.f;
	glm::mat4 cameraMatrix;
public:
	Renderer();
	~Renderer();

	bool isOpen() const;
	void updateLocation(float);
	void draw(const class Model &);
};