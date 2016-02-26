#include "stdafx.h"
#include "Model.h"
#include "Mesh.h"

Model::Model(const std::string & meshName) {
	mesh = &Mesh::loadModel(meshName);
}

void Model::draw(GLuint modelMatrixUniform) const {
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	for (auto && i : *mesh) {
		i.draw();
	}
}

void Model::change() {
	modelMatrix = glm::mat4(1.0);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix *= glm::mat4_cast(rotation * rotOrigin);
	modelMatrix = glm::scale(modelMatrix, scale);
	modelMatrix = glm::translate(modelMatrix, -posOrigin);
}

void Model::setRotation(glm::vec3 & rot) {
	rotation = glm::quat(rot);
	change();
}
void Model::setRotation(float x, float y, float z) {
	setRotation(glm::vec3(x, y, z));
}

void Model::setScale(glm::vec3 & _scale) {
	scale = _scale;
	change();
}
void Model::setScale(float x, float y, float z) {
	setScale(glm::vec3(x, y, z));
}

void Model::setPosition(glm::vec3 & pos) {
	position = pos;
	change();
}
void Model::setPosition(float x, float y, float z) {
	setPosition(glm::vec3(x, y, z));
}
