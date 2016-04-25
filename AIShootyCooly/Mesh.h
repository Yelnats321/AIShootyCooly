#pragma once

class Mesh {
	gl::Buffer vbo_, ebo_;
	gl::VAO vao_;
	const gl::Texture * texture_ = nullptr;
	GLuint numTriangles_ = 0;

	Mesh(const std::string &, aiMesh *, const aiScene *);
	static void loadMesh(const std::string &, std::vector<Mesh> &, aiNode *, const aiScene *);
public:
	static const std::vector<Mesh> & loadModel(const std::string &);
	void draw() const;
};