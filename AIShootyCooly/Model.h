#pragma once

class Mesh;

class Model {
	const std::vector<Mesh> * mesh_;
	glm::mat4 modelMatrix_;
	glm::vec3 scale_, position_, posOrigin_;
	glm::quat rotation_, rotOrigin_;
	void change();
public:
	Model(const std::string &);
	void draw(GLuint modelMatrixUniform) const;
	void setScale(glm::vec3 &);
	void setScale(float, float, float);
	void setRotation(glm::vec3 &);
	void setRotation(float, float, float);
	void setPosition(glm::vec3 &);
	void setPosition(float, float, float);
};
