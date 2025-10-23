#include "render_obj.hpp"
#include "texture.hpp"
#include <cmath>
#include <algorithm>

renderObj::renderObj() : objectX(0.0f), objectY(0.0f), objectZ(0.0f),
						 _rotateX(0.0f), _rotateY(1.0f), _rotateZ(0.0f), 
						 _faceData(), _centerX(0.0f), _centerY(0.0f), _centerZ(0.0f),
						 _useMaterial(false), _rotationAngle(0.0f), _rotationSpeed(0.03f),
						 _currentTextureFace(0),
						 _textureMix(0.0f), _textureMixSpeed(0.08f),
						 _minX(1000.0f), _maxX(-1000.0f), _minY(1000.0f), _maxY(-1000.0f),
						 _minZ(1000.0f), _maxZ(-1000.0f), _boundsCalculated(false),
						 _material(nullptr) {}

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
				_faceData.indices.push_back(face.vertexIndices[0]);
				_faceData.indices.push_back(face.vertexIndices[j]);
				_faceData.indices.push_back(face.vertexIndices[j + 1]);
			}
		}
	}

	calculateCenter();

	_boundsCalculated = false;  // Réinitialiser pour recalculer les bounds

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

void renderObj::rend(const Texture* texture, bool useTexture, bool furEnabled)
{
	if (_faceData.indices.empty() || _faceData.vertices.empty())
		return;
    
	setupTransformations();
	setupTexture(texture, useTexture);

	// Gestion du matériau
	if (_useMaterial && _material) {
		glDisable(GL_COLOR_MATERIAL);  // Désactiver pour que le matériau fonctionne
		_material->apply();
	} else {
		glEnable(GL_COLOR_MATERIAL);   // Activer pour les couleurs par défaut
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		
		// Définir un matériau par défaut gris si pas de texture
		if (!useTexture || !texture) {
			GLfloat defaultAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
			GLfloat defaultDiffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
			GLfloat defaultSpecular[] = { 0.3f, 0.3f, 0.3f, 1.0f };
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, defaultAmbient);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, defaultDiffuse);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, defaultSpecular);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);
		}
	}
	
	glBegin(GL_TRIANGLES);
	for (size_t i = 0; i < _faceData.indices.size(); i += 3) {
		renderTriangle(i, texture, useTexture);
	}
	glEnd();

	if (_useMaterial && _material && _material->hasFur() && furEnabled) {
		setupFurLayers();
	}
	
	glEnable(GL_COLOR_MATERIAL);
	
	cleanupTexture(texture, useTexture);
	glPopMatrix();
}

void renderObj::setupFurLayers()
{
	int layers = _material->getFurLayers();
		float length = _material->getFurLength();
		float baseAlpha = _material->getFurOpacity();

		if (layers > 0 && length > 0.0f && baseAlpha > 0.0f) {
			// Le fur ne se texture pas ici, on utilise un shading simple avec alpha
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE); // ne pas écrire dans le depth pour les coques
			glEnable(GL_COLOR_MATERIAL);
			glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

			for (int li = 1; li <= layers; ++li) {
				float t = (float)li / (float)layers;
				float offset = length * t;
				float a = baseAlpha * (1.0f - t * 0.9f);
				glColor4f(_material->getFurColor(0), _material->getFurColor(1), _material->getFurColor(2), a);

				glBegin(GL_TRIANGLES);
				for (size_t i = 0; i < _faceData.indices.size(); i += 3) {
					unsigned int idx0 = _faceData.indices[i + 0];
					unsigned int idx1 = _faceData.indices[i + 1];
					unsigned int idx2 = _faceData.indices[i + 2];

					float v0[3] = {
						_faceData.vertices[idx0 * 3] - _centerX,
						_faceData.vertices[idx0 * 3 + 1] - _centerY,
						_faceData.vertices[idx0 * 3 + 2] - _centerZ
					};
					float v1[3] = {
						_faceData.vertices[idx1 * 3] - _centerX,
						_faceData.vertices[idx1 * 3 + 1] - _centerY,
						_faceData.vertices[idx1 * 3 + 2] - _centerZ
					};
					float v2[3] = {
						_faceData.vertices[idx2 * 3] - _centerX,
						_faceData.vertices[idx2 * 3 + 1] - _centerY,
						_faceData.vertices[idx2 * 3 + 2] - _centerZ
					};

					float e1[3] = { v1[0]-v0[0], v1[1]-v0[1], v1[2]-v0[2] };
					float e2[3] = { v2[0]-v0[0], v2[1]-v0[1], v2[2]-v0[2] };
					float nx = e1[1]*e2[2] - e1[2]*e2[1];
					float ny = e1[2]*e2[0] - e1[0]*e2[2];
					float nz = e1[0]*e2[1] - e1[1]*e2[0];
					float nL = sqrt(nx*nx + ny*ny + nz*nz);
					if (nL > 0.0f) { nx/=nL; ny/=nL; nz/=nL; }
					glNormal3f(nx, ny, nz);

					float addx = nx * offset;
					float addy = ny * offset;
					float addz = nz * offset;

					glVertex3f(_faceData.vertices[idx0 * 3] + addx,
					           _faceData.vertices[idx0 * 3 + 1] + addy,
					           _faceData.vertices[idx0 * 3 + 2] + addz);
					glVertex3f(_faceData.vertices[idx1 * 3] + addx,
					           _faceData.vertices[idx1 * 3 + 1] + addy,
					           _faceData.vertices[idx1 * 3 + 2] + addz);
					glVertex3f(_faceData.vertices[idx2 * 3] + addx,
					           _faceData.vertices[idx2 * 3 + 1] + addy,
					           _faceData.vertices[idx2 * 3 + 2] + addz);
				}
				glEnd();
			}

			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
		}
}

void renderObj::setupTransformations()
{
	glPushMatrix();

	_rotationAngle += _rotationSpeed;
	if (_rotationAngle > 360.0f)
		_rotationAngle -= 360.0f;

	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(_rotationAngle * 30.0f, _rotateX, _rotateY, _rotateZ);
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
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
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
	
	// Calcul des edges pour la normale
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
	
	// Calcul de la normale (produit vectoriel)
	float nx = edge1[1] * edge2[2] - edge1[2] * edge2[1];
	float ny = edge1[2] * edge2[0] - edge1[0] * edge2[2];
	float nz = edge1[0] * edge2[1] - edge1[1] * edge2[0];
	
	// Normalisation
	float nLength = sqrt(nx*nx + ny*ny + nz*nz);
	if (nLength > 0.0f) {
		nx /= nLength;
		ny /= nLength;
		nz /= nLength;
	}
	
	glNormal3f(nx, ny, nz);
	
	// Couleur par défaut si pas de texture ET pas de matériau
	if (!_useMaterial && (!useTexture || !texture)) {
		size_t triangleNum = triangleIndex / 3;
		float grayValue = 0.3f + (triangleNum % 3) * 0.1f;
		glColor3f(grayValue, grayValue, grayValue);
	}
	
	// Déterminer la face du triangle à partir de la normale déjà calculée
	int triangleFace = 0;
	float absNx = fabs(nx), absNy = fabs(ny), absNz = fabs(nz);
	
	if (absNx > absNy && absNx > absNz) {
		triangleFace = (nx > 0) ? 1 : 2;
	} else if (absNy > absNx && absNy > absNz) {
		triangleFace = (ny > 0) ? 3 : 4;
	} else {
		triangleFace = (nz > 0) ? 5 : 6;
	}
	
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

void renderObj::calculateTextureBounds()
{
	if (_boundsCalculated || _faceData.vertices.empty())
		return;
		
	_minX = _maxX = _faceData.vertices[0] - _centerX;
	_minY = _maxY = _faceData.vertices[1] - _centerY;
	_minZ = _maxZ = _faceData.vertices[2] - _centerZ;
	
	for (size_t i = 0; i < _faceData.vertices.size(); i += 3) {
		float vx = _faceData.vertices[i] - _centerX;
		float vy = _faceData.vertices[i + 1] - _centerY;
		float vz = _faceData.vertices[i + 2] - _centerZ;
		
		_minX = fmin(_minX, vx); _maxX = fmax(_maxX, vx);
		_minY = fmin(_minY, vy); _maxY = fmax(_maxY, vy);
		_minZ = fmin(_minZ, vz); _maxZ = fmax(_maxZ, vz);
	}
	_boundsCalculated = true;
}

void renderObj::calculateTextureCoords(float x, float y, float z, int triangleFace, float& u, float& v)
{
	if (!_boundsCalculated) {
		calculateTextureBounds();
	}
	
	float coord1, coord2;
	float min1, max1, min2, max2;
	
	if (triangleFace == 1 || triangleFace == 2) {
		coord1 = z; coord2 = y;
		min1 = _minZ; max1 = _maxZ;
		min2 = _minY; max2 = _maxY;
	} else if (triangleFace == 3 || triangleFace == 4) {
		coord1 = x; coord2 = z;
		min1 = _minX; max1 = _maxX;
		min2 = _minZ; max2 = _maxZ;
	} else {
		coord1 = x; coord2 = y;
		min1 = _minX; max1 = _maxX;
		min2 = _minY; max2 = _maxY;
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

float renderObj::getBoundingRadius()
{
	if (!_boundsCalculated) {
		calculateTextureBounds();
	}
	
	float maxRadius = 0.0f;
	
	for (size_t i = 0; i < _faceData.vertices.size(); i += 3) {
		float dx = _faceData.vertices[i] - _centerX;
		float dy = _faceData.vertices[i + 1] - _centerY;
		float dz = _faceData.vertices[i + 2] - _centerZ;
		
		float radius = sqrt(dx*dx + dy*dy + dz*dz);
		if (radius > maxRadius) {
			maxRadius = radius;
		}
	}
	
	return maxRadius;
}

void renderObj::getBoundingBox(float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ)
{
	if (!_boundsCalculated) {
		calculateTextureBounds();
	}
	
	minX = _minX + _centerX;
	maxX = _maxX + _centerX;
	minY = _minY + _centerY;
	maxY = _maxY + _centerY;
	minZ = _minZ + _centerZ;
	maxZ = _maxZ + _centerZ;
}