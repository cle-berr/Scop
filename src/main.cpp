#include "../include/scop.hpp"

Scop::Scop(): _running(false), _window(nullptr), 
			   _lastMouseX(WINDOW_WIDTH/2.0), _lastMouseY(WINDOW_HEIGHT/2.0),
			   _firstMouse(true), _yaw(-90.0f), _pitch(0.0f), 
			   _mousePressed(false),
			   _cameraX(0.0f), _cameraY(0.0f), _cameraZ(5.0f),
			   _frontX(0.0f), _frontY(0.0f), _frontZ(-1.0f),
			   _textureEnabled(true), _tKeyPressed(false), _fKeyPressed(false),
			   _plusKeyPressed(false), _minusKeyPressed(false) {}

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
	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)WINDOW_WIDTH/(double)WINDOW_HEIGHT, 0.1, 100.0);
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

		glTranslatef(_render.objectX, _render.objectY, _render.objectZ);
		_render.rend(&_texture, _textureEnabled);

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

	if (!_render.setupFromParser(parser)) {
		std::cerr << "Failed to setup renderer from parser" << std::endl;
		return false;
	}
	return true;
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