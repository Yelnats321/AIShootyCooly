#pragma once

class Mesh {
	gl::Buffer vbo, ebo;
	gl::VAO vao;
	const gl::Texture * texture = nullptr;
	GLuint numTriangles = 0;
	Mesh(const std::string &, aiMesh *, const aiScene *);
	static void loadMesh(const std::string &, std::vector<Mesh> &, aiNode *, const aiScene *);
public:
	static const std::vector<Mesh> & loadModel(const std::string &);
	void draw() const;
};