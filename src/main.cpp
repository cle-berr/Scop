#include "../include/scop.hpp"

Scop::Scop():  _furEnabled(false), _running(false), _window(nullptr), 
			   _lastMouseX(WINDOW_WIDTH/2.0), _lastMouseY(WINDOW_HEIGHT/2.0),
			   _firstMouse(true), _yaw(-90.0f), _pitch(0.0f), 
			   _mousePressed(false),
			   _cameraX(0.0f), _cameraY(0.0f), _cameraZ(5.0f),
			   _frontX(0.0f), _frontY(0.0f), _frontZ(-1.0f),
               _rightX(0.0f), _rightY(0.0f), _rightZ(0.0f), _length(0.0f),
			   _sunX(5.0f), _sunY(5.0f), _sunZ(5.0f),
			   _sunOrbitRadius(8.66f), _sunOrbitAngleH(45.0f), _sunOrbitAngleV(35.26f),
		       _textureEnabled(false), _materialEnabled(false), _hasmaterial(false), _sunEnabled(false),
			   _tKeyPressed(false), _fKeyPressed(false), _mKeyPressed(false),
			   _lKeyPressed(false), _pKeyPressed(false), _1KeyPressed(false),
			   _2KeyPressed(false), _3KeyPressed(false), _viewSun(false), _vKeyPressed(false)
{
	updateSunPosition();
}

Scop::~Scop() { cleanup(); }

void Scop::cleanup()
{
	if (_window) {
		glfwDestroyWindow(_window);
		_window = nullptr;
	}
	glfwTerminate();
}

bool Scop::init()
{
	if (!glfwInit())
		return false;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "SCOP", NULL, NULL);
	if (!_window) {
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(_window);
	glfwSetWindowUserPointer(_window, this);
	
	glfwSetCursorPosCallback(_window, mouseCallback);
	glfwSetMouseButtonCallback(_window, mouseButtonCallback);

	if (glewInit() != GLEW_OK)
		return false;
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)WINDOW_WIDTH/(double)WINDOW_HEIGHT, 0.1, 1024.0);
	glMatrixMode(GL_MODELVIEW);
	
	_running = true;
	return true;
}

void Scop::run()
{
	while (!glfwWindowShouldClose(_window) && _running) {
		glfwPollEvents();

		handleInput();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		float yawRad = _yaw * M_PI / 180.0f;
		float pitchRad = _pitch * M_PI / 180.0f;
		
		_frontX = cos(yawRad) * cos(pitchRad);
		_frontY = sin(pitchRad);
		_frontZ = sin(yawRad) * cos(pitchRad);
		
		float targetX = _cameraX + _frontX;
		float targetY = _cameraY + _frontY;
		float targetZ = _cameraZ + _frontZ;

		gluLookAt(_cameraX, _cameraY, _cameraZ,
				  targetX, targetY, targetZ,
				  0.0, 1.0, 0.0);

		// Configuration de la lumière
		if (_sunEnabled || _materialEnabled) {
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			
			GLfloat lightPos[] = { _sunX, _sunY, _sunZ, 1.0f };
			GLfloat lightAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
			GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			
			glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
			glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
			glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

			// Dessiner le soleil seulement si _sunEnabled
			if (_sunEnabled) {
				glDisable(GL_LIGHTING);
				glColor3f(1.0f, 1.0f, 0.0f);
				glPushMatrix();
				glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
				if (_viewSun) {
					GLUquadric* quad = gluNewQuadric();
					gluSphere(quad, 0.3, 20, 20);
					gluDeleteQuadric(quad);
				}
				glPopMatrix();
				glEnable(GL_LIGHTING);
			}
		} else {
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
		}

		glTranslatef(_render.objectX, _render.objectY, _render.objectZ);
		_render.rend(&_texture, _textureEnabled, _furEnabled);

		glfwSwapBuffers(_window);
	}
}

bool Scop::loadObjFile(const std::string &filename)
{
	objParser parser;
	if (!parser.parse(filename)) {
		std::cerr << "Failed to parse OBJ file: " << filename << std::endl;
		return false;
	}

	if (parser.hasMaterials()) {
		std::string mtlFilename = parser.getFileMaterialName();
		std::string materialName = parser.getCurrentMaterial();
		
		std::string objPath = filename;
		size_t lastSlash = objPath.find_last_of("/\\");
		std::string mtlPath;
		
		if (lastSlash != std::string::npos) {
			mtlPath = objPath.substr(0, lastSlash + 1) + mtlFilename;
		} else {
			mtlPath = mtlFilename;
		}
		
		if (_material.loadFromMTL(mtlPath, materialName)) {
			_hasmaterial = true;
			_material.setvalue();
			_render.setMaterial(&_material);
		} else {
			_hasmaterial = false;
			std::cerr << "Warning: Failed to load material from MTL file" << std::endl;
		}
	}

	if (!_render.setupFromParser(parser)) {
		std::cerr << "Failed to setup renderer from parser" << std::endl;
		return false;
	}
	
	positionCameraForObject();
	
	return true;
}

void Scop::positionCameraForObject()
{
	float radius = _render.getBoundingRadius();
	
	if (radius <= 0.0f) {
		radius = 1.0f;
	}
	
	float fovRadians = 45.0f * M_PI / 180.0f;

	float distance = radius / tan(fovRadians / 2.0f);
	
	_cameraX = 0.0f;
	_cameraY = 0.0f;
	_cameraZ = distance;
	
	_yaw = -90.0f;
	_pitch = 0.0f;
	
	_frontX = 0.0f;
	_frontY = 0.0f;
	_frontZ = -1.0f;
    
	// Ajuster aussi la distance de la lumière pour être proportionnelle à l'objet
	positionSunForObject();
}

void Scop::positionSunForObject()
{
	float radius = _render.getBoundingRadius();
	if (radius <= 0.0f) radius = 1.0f;

	// Place the light on a sphere around the object
	_sunOrbitRadius = radius * 2.5f;
	// Keep pleasant default angles
	if (_sunOrbitAngleH == 0.0f && _sunOrbitAngleV == 0.0f) {
		_sunOrbitAngleH = 45.0f;
		_sunOrbitAngleV = 35.26f;
	}
	updateSunPosition();
}

bool Scop::loadTexture(const std::string &filename)
{
	if (!_texture.loadFromFile(filename)) {
		std::cerr << "Failed to load texture file: " << filename << std::endl;
		return false;
	}
	return true;
}

int main(int argc, char* argv[])
{
	if (argc != 2 && argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <obj_file> [tex_file]" << std::endl;
		return -1;
	}

	Scop app;

	if (!app.init()) {
		std::cerr << "Failed to initialize" << std::endl;
		return -1;
	}

	if (!app.loadObjFile(argv[1])) {
		std::cerr << "Failed to load OBJ file" << std::endl;
		return -1;
	}

	if (argc == 3) {
		if (!app.loadTexture(argv[2])) {
			std::cerr << "Failed to load texture file" << std::endl;
			return -1;
		}
	}

	app.run();
	return 0;
}