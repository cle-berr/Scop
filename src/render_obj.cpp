#include "render_obj.hpp"
#include "texture.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

renderObj::renderObj() : objectX(0.0f), objectY(0.0f), objectZ(0.0f),
                         _faceData(), _centerX(0.0f), _centerY(0.0f), _centerZ(0.0f),
                         _useMaterial(false), _rotationAngle(0.0f), _rotationSpeed(0.03f), 
                         _currentTextureFace(0), _textureRepeating(2.0f) {}

renderObj::~renderObj() {}

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
				_faceData.indices.push_back(face.vertexIndices[0]);     // Premier vertex
				_faceData.indices.push_back(face.vertexIndices[j]);     // Vertex courant
				_faceData.indices.push_back(face.vertexIndices[j + 1]); // Vertex suivant
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
	
	glPushMatrix();

	_rotationAngle += _rotationSpeed;
	if (_rotationAngle > 360.0f)
		_rotationAngle -= 360.0f;

	glTranslatef(0.0f, 0.0f, -5.0f);
	glRotatef(_rotationAngle * 57.3f, 0.0f, 1.0f, 0.0f);
	glTranslatef(-_centerX, -_centerY, -_centerZ);
	glScalef(1.0f, 1.0f, 1.0f);

	if (useTexture && texture) {
		glEnable(GL_TEXTURE_2D);
		texture->bind();
		glColor3f(1.0f, 1.0f, 1.0f);
	} else {
		glDisable(GL_TEXTURE_2D);
	}

	glBegin(GL_TRIANGLES);
	for (size_t i = 0; i < _faceData.indices.size(); i += 3) {
		if (!useTexture || !texture) {
			if ((i/3) % 3 == 0)
				glColor3f(0.1f, 0.1f, 0.1f);
			else if ((i/3) % 3 == 1)
				glColor3f(0.2f, 0.2f, 0.2f);
			else
				glColor3f(0.3f, 0.3f, 0.3f); 
		}

		float v1x = _faceData.vertices[_faceData.indices[i] * 3] - _centerX;
		float v1y = _faceData.vertices[_faceData.indices[i] * 3 + 1] - _centerY;
		float v1z = _faceData.vertices[_faceData.indices[i] * 3 + 2] - _centerZ;
		
		float v2x = _faceData.vertices[_faceData.indices[i + 1] * 3] - _centerX;
		float v2y = _faceData.vertices[_faceData.indices[i + 1] * 3 + 1] - _centerY;
		float v2z = _faceData.vertices[_faceData.indices[i + 1] * 3 + 2] - _centerZ;
		
		float v3x = _faceData.vertices[_faceData.indices[i + 2] * 3] - _centerX;
		float v3y = _faceData.vertices[_faceData.indices[i + 2] * 3 + 1] - _centerY;
		float v3z = _faceData.vertices[_faceData.indices[i + 2] * 3 + 2] - _centerZ;
		
		float edge1x = v2x - v1x, edge1y = v2y - v1y, edge1z = v2z - v1z;
		float edge2x = v3x - v1x, edge2y = v3y - v1y, edge2z = v3z - v1z;
		
		float nx = edge1y * edge2z - edge1z * edge2y;
		float ny = edge1z * edge2x - edge1x * edge2z;
		float nz = edge1x * edge2y - edge1y * edge2x;
		
		// Normaliser
		float nLength = sqrt(nx*nx + ny*ny + nz*nz);
		if (nLength > 0.0f) {
			nx /= nLength;
			ny /= nLength;
			nz /= nLength;
		}
		
		int triangleFace = 0;
		float absNx = fabs(nx), absNy = fabs(ny), absNz = fabs(nz);
		
		if (absNx > absNy && absNx > absNz) {
			triangleFace = (nx > 0) ? 1 : 2;
		} else if (absNy > absNx && absNy > absNz) {
			triangleFace = (ny > 0) ? 3 : 4;
		} else if (absNz > 0) {
			triangleFace = (nz > 0) ? 5 : 6;
		}
		
		bool applyTexture = useTexture && texture && (_currentTextureFace == 0 || _currentTextureFace == triangleFace);
		
		for (int j = 0; j < 3; ++j) {
			const unsigned int index = _faceData.indices[i + j];
			
			if (applyTexture) {
				float x = _faceData.vertices[index * 3 + 0] - _centerX;
				float y = _faceData.vertices[index * 3 + 1] - _centerY;
				float z = _faceData.vertices[index * 3 + 2] - _centerZ;
				
				float u = 0.5f, v = 0.5f;
				
				if (triangleFace == 1 || triangleFace == 2) {
					u = (z * _textureRepeating);
					v = (y * _textureRepeating);
				} else if (triangleFace == 3 || triangleFace == 4) {
					u = (x * _textureRepeating);
					v = (z * _textureRepeating);
				} else if (triangleFace == 5 || triangleFace == 6) {
					u = (x * _textureRepeating);
					v = (y * _textureRepeating);
				}
				
				glTexCoord2f(u, v);
			}
			
			glVertex3f(
				_faceData.vertices[index * 3 + 0],
				_faceData.vertices[index * 3 + 1],
				_faceData.vertices[index * 3 + 2]
			);
		}
	}

	glEnd();
	
	if (useTexture && texture) {
		texture->unbind();
		glDisable(GL_TEXTURE_2D);
	}
	
	glPopMatrix();
}