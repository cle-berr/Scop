#pragma once
#include <string>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>

class Material
{
	public:
		Material();
		~Material();

		bool loadFromMTL(const std::string &filename, const std::string &materialName);
		void apply() const;
		void setvalue();
		void reset();

	private:
		float _ambient[4];   //ka
		float _diffuse[4];   // Kd
		float _specular[4];  // Ks
		float _shininess;    // Ns
		float _opacity;      // d

		float __ambient[4];   //ka
		float __diffuse[4];   // Kd
		float __specular[4];  // Ks
		float __shininess;    // Ns
		float __opacity;      // d
};
