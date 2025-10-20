#pragma once
#include <string>
#include <GL/glew.h>

class Material
{
	public:
		Material();
		~Material();

		bool loadFromMTL(const std::string &filename, const std::string &materialName);
		void apply() const;

	private:
		float _ambient[4];   //ka
		float _diffuse[4];   // Kd
		float _specular[4];  // Ks
		float _shininess;    // Ns
		float _opacity;      // d
};
