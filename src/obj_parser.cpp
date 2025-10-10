#include "../include/obj_parser.hpp"

objParser::objParser() : _currentMaterial(""), _fileMaterialName("") {}

bool objParser::parse(const std::string &filename)
{
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filename << std::endl;
		return false;
	}
	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;
		if (prefix == "v") {
			Vertex vertex;
			iss >> vertex.x >> vertex.y >> vertex.z;
			_vertices.push_back(vertex);
		} else if (prefix == "f") {
			Face face;
			while (iss) {
				unsigned int index;
				if (iss >> index) {
					face.vertexIndices.push_back(index - 1);
				}
			}
			_faces.push_back(face);
		}
		else if (prefix == "usemtl") {
			iss >> _currentMaterial;
		} else if (prefix == "mtllib") {
			iss >> _fileMaterialName;
		}
	}
	file.close();
	std::cout << "Parsed " << _vertices.size() << " vertices and " << _faces.size() << " faces." << std::endl;
	return true;
}