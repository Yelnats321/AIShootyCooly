#pragma once

class Renderer {
	GLFWwindow * window;
	static const int WindowWidth = 800, WindowHeight = 600;
	gl::Program program;
	gl::Buffer ebo, vbo;
	gl::VAO vao;
public:
	Renderer();
	~Renderer();

	bool isOpen() const;
	void draw();
};