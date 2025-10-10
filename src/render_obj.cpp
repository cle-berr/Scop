#include "render_obj.hpp"

renderObj::renderObj() : objectX(0.0f), objectY(0.0f), objectZ(0.0f),
                         _faceData(), _centerX(0.0f), _centerY(0.0f), _centerZ(0.0f),
                         _useMaterial(false), _rotationAngle(0.0f), _rotationSpeed(0.03f) {}

renderObj::~renderObj() {}

void renderObj::rend()
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

	glBegin(GL_TRIANGLES);
	for (size_t i = 0; i < _faceData.indices.size(); i += 3) {
		if (_useMaterial) {
			// Set color or texture based on material
		}
		else {
			if ((i/3) % 3 == 0)
				glColor3f(0.1f, 0.1f, 0.1f);
			else if ((i/3) % 3 == 1)
				glColor3f(0.2f, 0.2f, 0.2f);
			else
				glColor3f(0.3f, 0.3f, 0.3f); 
		}

		for (int j = 0; j < 3; ++j) {
			const unsigned int index = _faceData.indices[i + j];
			glVertex3f(
				_faceData.vertices[index * 3 + 0],
				_faceData.vertices[index * 3 + 1],
				_faceData.vertices[index * 3 + 2]
			);
		}
	}

	glEnd();
	glPopMatrix();
}

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