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

		bool hasFur() const { return _hasFur; }
		int getFurLayers() const { return _furLayers; }
		float getFurLength() const { return _furLength; }
		float getFurOpacity() const { return _furOpacity; }
		float getFurColor(int index) const;

	private:
		float _ambient[4];   //ka
		float _diffuse[4];   // Kd
		float _specular[4];  // Ks
		float _shininess;    // Ns
		float _opacity;      // d

		bool _hasFur;
		int _furLayers;
		float _furLength;   // en unités objet
		float _furOpacity;  // alpha max 0..1
		float _furColor[3];

		float __ambient[4];   //ka
		float __diffuse[4];   // Kd
		float __specular[4];  // Ks
		float __shininess;    // Ns
		float __opacity;      // d

		bool __hasFur;
		int __furLayers;
		float __furLength;
		float __furOpacity;
		float __furColor[3];
};
