#include "stdafx.h"
#include "Model.h"
#include "Mesh.h"

Model::Model(const std::string & meshName) {
	mesh_ = &Mesh::loadModel(meshName);
}

void Model::draw(GLuint modelMatrixUniform) const {
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(getMatrix()));
	for (auto && i : *mesh_) {
		i.draw();
	}
}