#include <SOIL/SOIL.h>
#include "texture.hpp"

bool Texture::loadFromFile(const std::string &filename)
{
	_textureID = SOIL_load_OGL_texture(
		filename.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS
	);

	if (_textureID == 0) {
		std::cerr << "Failed to load texture: " << SOIL_last_result() << std::endl;
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, _textureID);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &_width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &_height);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}
