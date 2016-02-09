#pragma once

class Mesh {
	gl::Buffer vbo, ebo;
	gl::VAO vao;
	GLuint numTriangles;
	Mesh(aiMesh *, const aiScene *);
	static void loadMesh(std::vector<Mesh> &, aiNode *, const aiScene *);
public:
	static const std::vector<Mesh> & loadModel(const std::string &);
	void draw() const;
};