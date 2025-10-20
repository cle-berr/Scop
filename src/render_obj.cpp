#include "render_obj.hpp"
#include "texture.hpp"
#include <cmath>
#include <algorithm>

renderObj::renderObj() : objectX(0.0f), objectY(0.0f), objectZ(0.0f),
						 _faceData(), _centerX(0.0f), _centerY(0.0f), _centerZ(0.0f),
					 _useMaterial(false), _rotationAngle(0.0f), _rotationSpeed(0.03f), 
					 _currentTextureFace(0) {}renderObj::~renderObj() {}

bool renderObj::setupFromParser(const objParser &parser)
{
	const auto &vertices = parser.getVertices();
	const auto &faces = parser.getFaces();

	_faceData.vertices.clear();
	_faceData.indices.clear();

	for (size_t i = 0; i < vertices.size(); ++i) {
		_faceData.vertices.push_back(vertices[i].x);
		_faceData.vertices.push_back(vertices[i].y);
		_faceData.vertices.push_back(vertices[i].z);
	}

	for (size_t i = 0; i < faces.size(); ++i) {
		const auto& face = faces[i];
		if (face.vertexIndices.size() >= 3) {
			for (size_t j = 1; j < face.vertexIndices.size() - 1; ++j) {
				_faceData.indices.push_back(face.vertexIndices[0]);
				_faceData.indices.push_back(face.vertexIndices[j]);
				_faceData.indices.push_back(face.vertexIndices[j + 1]);
			}
		}
	}

	calculateCenter();

	return !_faceData.vertices.empty() && !_faceData.indices.empty();
}

void renderObj::calculateCenter()
{
	if (_faceData.vertices.empty())
		return;

	_centerX = _centerY = _centerZ = 0.0f;

	for (size_t i = 0; i < _faceData.vertices.size(); i += 3) {
		_centerX += _faceData.vertices[i];
		_centerY += _faceData.vertices[i + 1];
		_centerZ += _faceData.vertices[i + 2];
	}

	_centerX /= (_faceData.vertices.size() / 3);
	_centerY /= (_faceData.vertices.size() / 3);
	_centerZ /= (_faceData.vertices.size() / 3);
}

void renderObj::rend(const Texture* texture, bool useTexture)
{
	if (_faceData.indices.empty() || _faceData.vertices.empty())
		return;
	
	setupTransformations();
	setupTexture(texture, useTexture);
	if (_useMaterial && _material) {
		_material->apply();
	}
	glBegin(GL_TRIANGLES);
	for (size_t i = 0; i < _faceData.indices.size(); i += 3) {
		renderTriangle(i, texture, useTexture);
	}
	glEnd();
	
	cleanupTexture(texture, useTexture);
	glPopMatrix();
}

void renderObj::setupTransformations()
{
	glPushMatrix();

	_rotationAngle += _rotationSpeed;
	if (_rotationAngle > 360.0f)
		_rotationAngle -= 360.0f;

	glTranslatef(0.0f, 0.0f, -5.0f);
	glRotatef(_rotationAngle * 30.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(-_centerX, -_centerY, -_centerZ);
}

void renderObj::setupTexture(const Texture* texture, bool useTexture)
{
	if (useTexture && texture) {
		glEnable(GL_TEXTURE_2D);
		texture->bind();
		glColor3f(1.0f, 1.0f, 1.0f);
	} else {
		glDisable(GL_TEXTURE_2D);
	}
}

void renderObj::cleanupTexture(const Texture* texture, bool useTexture)
{
	if (useTexture && texture) {
		texture->unbind();
		glDisable(GL_TEXTURE_2D);
	}
}

void renderObj::renderTriangle(size_t triangleIndex, const Texture* texture, bool useTexture)
{
	float vertices[3][3];
	for (int i = 0; i < 3; ++i) {
		unsigned int index = _faceData.indices[triangleIndex + i];
		vertices[i][0] = _faceData.vertices[index * 3] - _centerX;
		vertices[i][1] = _faceData.vertices[index * 3 + 1] - _centerY;
		vertices[i][2] = _faceData.vertices[index * 3 + 2] - _centerZ;
	}
	
	float edge1[3] = {
		vertices[1][0] - vertices[0][0],
		vertices[1][1] - vertices[0][1],
		vertices[1][2] - vertices[0][2]
	};
	float edge2[3] = {
		vertices[2][0] - vertices[0][0],
		vertices[2][1] - vertices[0][1],
		vertices[2][2] - vertices[0][2]
	};
	
	float nx = edge1[1] * edge2[2] - edge1[2] * edge2[1];
	float ny = edge1[2] * edge2[0] - edge1[0] * edge2[2];
	float nz = edge1[0] * edge2[1] - edge1[1] * edge2[0];
	
	float nLength = sqrt(nx*nx + ny*ny + nz*nz);
	if (nLength > 0.0f) {
		nx /= nLength;
		ny /= nLength;
		nz /= nLength;
	}
	
	glNormal3f(-nx, -ny, -nz);
	
	if (!useTexture || !texture) {
		size_t triangleNum = triangleIndex / 3;
		float grayValue = 0.3f + (triangleNum % 3) * 0.1f;
		glColor3f(grayValue, grayValue, grayValue);
	}
	
	int triangleFace = calculateTriangleFace(vertices);
	bool applyTexture = useTexture && texture && (_currentTextureFace == 0 || _currentTextureFace == triangleFace);
	
	for (int j = 0; j < 3; ++j) {
		if (applyTexture) {
			float u, v;
			calculateTextureCoords(vertices[j][0], vertices[j][1], vertices[j][2], triangleFace, u, v);
			glTexCoord2f(u, v);
		}
		
		unsigned int index = _faceData.indices[triangleIndex + j];
		glVertex3f(
			_faceData.vertices[index * 3],
			_faceData.vertices[index * 3 + 1],
			_faceData.vertices[index * 3 + 2]
		);
	}
}

int renderObj::calculateTriangleFace(float vertices[3][3])
{
	float edge1[3] = {vertices[1][0] - vertices[0][0], vertices[1][1] - vertices[0][1], vertices[1][2] - vertices[0][2]};
	float edge2[3] = {vertices[2][0] - vertices[0][0], vertices[2][1] - vertices[0][1], vertices[2][2] - vertices[0][2]};
	
	float nx = edge1[1] * edge2[2] - edge1[2] * edge2[1];
	float ny = edge1[2] * edge2[0] - edge1[0] * edge2[2];
	float nz = edge1[0] * edge2[1] - edge1[1] * edge2[0];
	
	float nLength = sqrt(nx*nx + ny*ny + nz*nz);
	if (nLength > 0.0f) {
		nx /= nLength;
		ny /= nLength;
		nz /= nLength;
	}
	
	float absNx = fabs(nx), absNy = fabs(ny), absNz = fabs(nz);
	
	if (absNx > absNy && absNx > absNz) {
		return (nx > 0) ? 1 : 2;
	} else if (absNy > absNx && absNy > absNz) {
		return (ny > 0) ? 3 : 4;
	} else {
		return (nz > 0) ? 5 : 6;
	}
}

void renderObj::calculateTextureCoords(float x, float y, float z, int triangleFace, float& u, float& v)
{
	static float minX = 1000.0f, maxX = -1000.0f;
	static float minY = 1000.0f, maxY = -1000.0f; 
	static float minZ = 1000.0f, maxZ = -1000.0f;
	static bool boundsCalculated = false;
	
	if (!boundsCalculated) {
		for (size_t i = 0; i < _faceData.vertices.size(); i += 3) {
			float vx = _faceData.vertices[i] - _centerX;
			float vy = _faceData.vertices[i + 1] - _centerY;
			float vz = _faceData.vertices[i + 2] - _centerZ;
			
			minX = fmin(minX, vx); maxX = fmax(maxX, vx);
			minY = fmin(minY, vy); maxY = fmax(maxY, vy);
			minZ = fmin(minZ, vz); maxZ = fmax(maxZ, vz);
		}
		boundsCalculated = true;
	}
	
	float coord1, coord2;
	float min1, max1, min2, max2;
	
	if (triangleFace == 1 || triangleFace == 2) {
		coord1 = z; coord2 = y;
		min1 = minZ; max1 = maxZ;
		min2 = minY; max2 = maxY;
	} else if (triangleFace == 3 || triangleFace == 4) {
		coord1 = x; coord2 = z;
		min1 = minX; max1 = maxX;
		min2 = minZ; max2 = maxZ;
	} else {
		coord1 = x; coord2 = y;
		min1 = minX; max1 = maxX;
		min2 = minY; max2 = maxY;
	}
	
	float width = max1 - min1;
	float height = max2 - min2;
	
	if (width > 0.0f && height > 0.0f) {
		u = (coord1 - min1) / width;
		v = 1.0f - (coord2 - min2) / height;
	} else {
		u = 0.5f; v = 0.5f;
	}
	u = fmax(0.0f, fmin(1.0f, u));
	v = fmax(0.0f, fmin(1.0f, v));
}