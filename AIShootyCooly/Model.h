#pragma once

class Mesh;

class Model {
	const std::vector<Mesh> * mesh;
	glm::mat4 modelMatrix;
	glm::vec3 scale, position, posOrigin;
	glm::quat rotation, rotOrigin;
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
