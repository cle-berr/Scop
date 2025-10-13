#pragma once

#include <GL/glew.h>
#include <vector>
#include <string>
#include <iostream>
#include "stb_image.h"

class Texture
{
	public:
		Texture() : _textureID(0), _width(0), _height(0) {}
		~Texture() { if (_textureID) glDeleteTextures(1, &_textureID); }

		bool loadFromFile(const std::string &filename);
		void bind() const { glBindTexture(GL_TEXTURE_2D, _textureID); }
		void unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }
		int getWidth() const { return _width; }
		int getHeight() const { return _height; }

	private:
		GLuint _textureID;
		int _width;
		int _height;
};