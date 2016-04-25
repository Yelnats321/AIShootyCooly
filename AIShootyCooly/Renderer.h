#pragma once

class Renderer {
	GLFWwindow * window_;
	static const int WindowWidth_ = 800, WindowHeight_ = 600;
	gl::Program program_;
	GLuint viewProjectionLocation_, modelMatrixLocation_;

	glm::vec3 cameraPos_;
	float horizontalAngle_ = 0.f, verticalAngle_ = 0.f;
	glm::mat4 cameraMatrix_;
public:
	Renderer();
	~Renderer();

	Renderer(const Renderer &) = delete;
	Renderer & operator=(const Renderer &) = delete;

	bool isOpen() const;
	void updateLocation(float);
	void draw(const class Model &);
};