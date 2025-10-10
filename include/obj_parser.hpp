#pragma once

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <map>
#include <vector>

struct Vertex
{
	float x, y, z;
};

struct Face
{
	std::vector<unsigned int> vertexIndices;
};

class objParser
{
	public:
		objParser();

		std::vector<Vertex> getVertices() const { return _vertices; }
		std::vector<Face> getFaces() const { return _faces; }
		bool parse(const std::string &filename);
		std::string getCurrentMaterial() const { return _currentMaterial; }
		std::string getFileMaterialName() const { return _fileMaterialName; }
		bool hasMaterials() const { return !_fileMaterialName.empty(); }

	private:
		std::vector<Vertex> _vertices;
		std::vector<Face> _faces;
		std::string _currentMaterial;
		std::string _fileMaterialName;
};

