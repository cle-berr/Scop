#include "../include/scop.hpp"

Scop::Scop(): _running(false), _window(nullptr) {}

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

		gluLookAt(0.0, 0.0, 5.0,
				  0.0, 0.0, 0.0,
				  0.0, 1.0, 0.0);

		glTranslatef(_render.objectX, _render.objectY, _render.objectZ);
		_render.rend();

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
	(void)filename;
	std::cout << "Texture loading not implemented yet." << std::endl;
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