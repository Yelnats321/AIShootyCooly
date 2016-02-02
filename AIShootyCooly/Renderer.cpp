#include "stdafx.h"
#include "Renderer.h"

namespace {
void error_callback(int error, const char* desc) {
	std::cerr << "ERROR " << error << " - " << desc << std::endl;
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
	static const GLuint elements[] = {0, 1, 2};
	static const GLfloat points[] = {
		-0.7f,  1.0f, 0.0f, 0.3f, 0.2f, 0.4f,
		-1.0f, -1.0f, 0.0f, 1.0f, 0.f,  0.f,
		 1.0f, -1.0f, 0.0f, 0.f,  1.f,  0.f};

	program.gen("shaders/pass.vert", "shaders/pass.frag");

	glUseProgram(program);

	vao.gen();
	glBindVertexArray(vao);

	vbo.gen();
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	ebo.gen();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Renderer::~Renderer() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

bool Renderer::isOpen() const {
	return !glfwWindowShouldClose(window);
}

void Renderer::draw() {
	glUseProgram(program);
	glViewport(0, 0, WindowWidth, WindowHeight);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(window);
	glfwPollEvents();
}
