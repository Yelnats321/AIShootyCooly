#include "stdafx.h"
#include "Mesh.h"

namespace {
struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

auto locationAppend(std::string origin, const std::string & file) {
	boost::replace(origin, '/', '\\');
	auto lastOf = origin.find_last_of('\\');
	if (lastOf != std::string::npos) {
		return origin.substr(0, lastOf + 1) + file;
	}
	throw std::runtime_error("Trying to append a location to a file without a valid origin: " + origin + " file: " + file);
}
}

const std::vector<Mesh> & Mesh::loadModel(const std::string & modelName) {
	static std::unordered_map<std::string, std::vector<Mesh>> meshMap;
	if (meshMap.count(modelName) == 0) {
		Assimp::Importer importer;
		auto scene = importer.ReadFile(modelName,
									   aiProcess_GenNormals |
									   aiProcess_JoinIdenticalVertices |
									   aiProcess_Triangulate |
									   aiProcess_ImproveCacheLocality |
									   aiProcess_RemoveRedundantMaterials |
									   aiProcess_FlipUVs/* |
									   aiProcess_RemoveComponent*/);
		if (!scene) {
			throw std::runtime_error("Could not load model: " + modelName);
		}
		std::vector<Mesh> meshData;
		loadMesh(modelName, meshData, scene->mRootNode, scene);
		meshMap.emplace(modelName, std::move(meshData));
	}
	return meshMap[modelName];
}

void Mesh::loadMesh(const std::string & modelName, std::vector<Mesh> & meshData, aiNode * node,
					const aiScene * scene) {
	for (unsigned i = 0; i < node->mNumMeshes; i++) {
		auto mesh = scene->mMeshes[node->mMeshes[i]];
		meshData.emplace_back(Mesh(modelName, mesh, scene));
	}

	for (unsigned i = 0; i < node->mNumChildren; i++) {
		loadMesh(modelName, meshData, node->mChildren[i], scene);
	}
}

Mesh::Mesh(const std::string & modelName, aiMesh * mesh, const aiScene * scene) {
	static gl::TextureManager manager;
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	if (!mesh->HasTextureCoords(0)) {
		throw std::runtime_error("Mesh does not have texture coords");
	}
	auto material = scene->mMaterials[mesh->mMaterialIndex];
	bool useTexture = false;
	if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		useTexture = true;
		aiString path;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		// TODO: Passing a const char * instead of a std::string
		texture_ = &manager.loadTexture(locationAppend(modelName, path.C_Str()));
	}

	for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
		glm::vec2 uv;
		if (useTexture) {
			uv = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
		}
		vertices.emplace_back(Vertex{
			{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
			{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z},
			uv
		});
	}
	for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
		auto face = mesh->mFaces[i];
		for (unsigned j = 0; j < face.mNumIndices; ++j) {
			indices.emplace_back(face.mIndices[j]);
		}
	}

	numTriangles_ = indices.size();
	vao_.gen();
	glBindVertexArray(vao_);

	vbo_.gen();
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	ebo_.gen();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}


void Mesh::draw() const {
	glBindVertexArray(vao_);
	if (texture_) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, *texture_);
	}
	glDrawElements(GL_TRIANGLES, numTriangles_, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}
