#include "../include/scop.hpp"

void Scop::handleInput()
{
	if (glfwGetKey(_window, GLFW_KEY_T) == GLFW_PRESS) {
		if (!_tKeyPressed) {
			_textureEnabled = !_textureEnabled;
			_tKeyPressed = true;
		}
	} else {
		_tKeyPressed = false;
	}

	if (glfwGetKey(_window, GLFW_KEY_F) == GLFW_PRESS) {
		if (!_fKeyPressed) {
			_render.nextTextureFace();
			_fKeyPressed = true;
			
			int currentFace = _render.getTextureFace();
			const char* faceNames[] = {"Toutes les faces", "Face X+", "Face X-", "Face Y+", "Face Y-", "Face Z+", "Face Z-"};
			std::cout << "Face texture: " << faceNames[currentFace] << std::endl;
		}
	} else {
		_fKeyPressed = false;
	}

	float cameraSpeed = 0.1f;
	if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		cameraSpeed += 1.0f;
	}
	else {
		cameraSpeed = 0.1f;
	}
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		_running = false;
	}
	if (glfwGetKey(_window, GLFW_KEY_M) == GLFW_PRESS) {
		// Toggle material usage
	}
	if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) {
		_cameraX += _frontX * cameraSpeed;
		_cameraY += _frontY * cameraSpeed;
		_cameraZ += _frontZ * cameraSpeed;
	}
	if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS) {
		_cameraX -= _frontX * cameraSpeed;
		_cameraY -= _frontY * cameraSpeed;
		_cameraZ -= _frontZ * cameraSpeed;
	}
	if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) {
		float rightX = _frontY * 0.0f - _frontZ * 1.0f;
		float rightZ = _frontX * 1.0f - _frontY * 0.0f;
		float length = sqrt(rightX * rightX + rightZ * rightZ);
		if (length > 0) {
			rightX /= length;
			rightZ /= length;
		}
		_cameraX -= rightX * cameraSpeed;
		_cameraZ -= rightZ * cameraSpeed;
	}
	if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) {
		float rightX = _frontY * 0.0f - _frontZ * 1.0f;
		float rightZ = _frontX * 1.0f - _frontY * 0.0f;
		float length = sqrt(rightX * rightX + rightZ * rightZ);
		if (length > 0) {
			rightX /= length;
			rightZ /= length;
		}
		_cameraX += rightX * cameraSpeed;
		_cameraZ += rightZ * cameraSpeed;
	}
	
	// Contrôles verticaux
	if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		_cameraY += cameraSpeed;
	}
	if (glfwGetKey(_window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
		_cameraY -= cameraSpeed;
	}

	if (glfwGetKey(_window, GLFW_KEY_I) == GLFW_PRESS) {
		_render.objectZ -= 0.5f;
	}
	if (glfwGetKey(_window, GLFW_KEY_K) == GLFW_PRESS) {
		_render.objectZ += 0.5f;
	}
	if (glfwGetKey(_window, GLFW_KEY_J) == GLFW_PRESS) {
		_render.objectX -= 0.5f;
	}
	if (glfwGetKey(_window, GLFW_KEY_L) == GLFW_PRESS) {
		_render.objectX += 0.5f;
	}
	if (glfwGetKey(_window, GLFW_KEY_U) == GLFW_PRESS) {
		_render.objectY += 0.5f;
	}
	if (glfwGetKey(_window, GLFW_KEY_O) == GLFW_PRESS) {
		_render.objectY -= 0.5f;
	}
}

void Scop::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	Scop* app = static_cast<Scop*>(glfwGetWindowUserPointer(window));
	if (app) {
		app->updateCamera(xpos, ypos);
	}
}

void Scop::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	(void)mods;
	Scop* app = static_cast<Scop*>(glfwGetWindowUserPointer(window));
	if (app && button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			app->_mousePressed = true;
			app->_firstMouse = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		} else if (action == GLFW_RELEASE) {
			app->_mousePressed = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

void Scop::updateCamera(double xpos, double ypos)
{
	if (!_mousePressed) {
		_lastMouseX = xpos;
		_lastMouseY = ypos;
		return;
	}

	if (_firstMouse) {
		_lastMouseX = xpos;
		_lastMouseY = ypos;
		_firstMouse = false;
	}

	double xoffset = xpos - _lastMouseX;
	double yoffset = _lastMouseY - ypos;
	_lastMouseX = xpos;
	_lastMouseY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	_yaw += xoffset;
	_pitch += yoffset;

	if (_pitch > 89.0f)
		_pitch = 89.0f;
	if (_pitch < -89.0f)
		_pitch = -89.0f;
}