#pragma once

class Mesh;

class Model {
	const std::vector<Mesh> * mesh;
public:
	Model(const std::string &);
	void draw() const;
};
