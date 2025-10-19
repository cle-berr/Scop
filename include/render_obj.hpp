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
		
		void setTextureFace(int face) { _currentTextureFace = face; }
		int getTextureFace() const { return _currentTextureFace; }
		void nextTextureFace() { _currentTextureFace = (_currentTextureFace + 1) % 7; }

		void rend(const class Texture* texture, bool useTexture);
		
		float objectX, objectY, objectZ;

	private:
		FaceData _faceData;
		float _centerX, _centerY, _centerZ;
		bool _useMaterial;
		float _rotationAngle;
		float _rotationSpeed;
		int _currentTextureFace;

		void setupTransformations();
		void setupTexture(const Texture* texture, bool useTexture);
		void cleanupTexture(const Texture* texture, bool useTexture);
		void renderTriangle(size_t triangleIndex, const Texture* texture, bool useTexture);
		int calculateTriangleFace(float vertices[3][3]);
		void calculateTextureCoords(float x, float y, float z, int triangleFace, float& u, float& v);
};