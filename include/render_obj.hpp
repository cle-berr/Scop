#pragma once

#include <GL/glew.h>
#include <vector>
#include "obj_parser.hpp"

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

		void rend();
		
		float objectX, objectY, objectZ;

	private:
		FaceData _faceData;
		float _centerX, _centerY, _centerZ;
		bool _useMaterial;
		float _rotationAngle;
		float _rotationSpeed;
		
};