#define STB_IMAGE_IMPLEMENTATION
#include "texture.hpp"

bool Texture::loadFromFile(const std::string &filename)
{
	int channels;
	unsigned char *data = stbi_load(filename.c_str(), &_width, &_height, &channels, 0);
	if (!data) {
		std::cerr << "Failed to load texture image: " << filename << std::endl;
		return false;
	}

	glGenTextures(1, &_textureID);
	glBindTexture(GL_TEXTURE_2D, _textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, (channels == 4) ? GL_RGBA : GL_RGB, _width, _height, 0,
	             (channels == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
	return true;
}
