#include "stdafx.h"
#include "Model.h"
#include "Mesh.h"

Model::Model(const std::string & meshName) {
	mesh = &Mesh::loadModel(meshName);
}

void Model::draw() const {
	for (auto && i : *mesh) {
		i.draw();
	}
}
