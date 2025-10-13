#pragma once

#include <GL/glew.h>
#include <vector>
#include "obj_parser.hpp"

// Forward declaration
class Texture;

struct FaceData
{
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
};

class renderObj
{
	public:
		renderObj();
		~renderObj();

		void setUseMaterial(bool use) { _useMaterial = use; }
		bool setupFromParser(const objParser &parser);
		void calculateCenter();
		void setRotationSpeed(float speed) { _rotationSpeed = speed; }
		float getRotationSpeed() const { return _rotationSpeed; }
		void setRotationAngle(float angle) { _rotationAngle = angle; }
		float getRotationAngle() const { return _rotationAngle; }
		
		void setTextureFace(int face) { _currentTextureFace = face; } // 0=toutes, 1-6=faces
		int getTextureFace() const { return _currentTextureFace; }
		void nextTextureFace() { _currentTextureFace = (_currentTextureFace + 1) % 7; }
		
		void setTextureRepeating(float tiling) { _textureRepeating = tiling; }
		float getTextureRepeating() const { return _textureRepeating; }
		void increaseTextureRepeating() { _textureRepeating += 0.5f; if (_textureRepeating > 50.0f) _textureRepeating = 50.0f; }
		void decreaseTextureRepeating() { _textureRepeating -= 0.5f; if (_textureRepeating < 0.5f) _textureRepeating = 0.5f; }

		void rend();
		void rend(const class Texture* texture, bool useTexture);
		
		float objectX, objectY, objectZ;

	private:
		FaceData _faceData;
		float _centerX, _centerY, _centerZ;
		bool _useMaterial;
		float _rotationAngle;
		float _rotationSpeed;
		int _currentTextureFace;
		float _textureRepeating;
		
};