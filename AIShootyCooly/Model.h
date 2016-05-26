#pragma once

class Mesh;

class Model : public gl::Model {
	const std::vector<Mesh> * mesh_;
public:
	Model(const std::string &);
	void draw(GLuint modelMatrixUniform) const;
};
