#include "../include/scop.hpp"

void Scop::handleInput()
{
	float cameraSpeed = 0.1f;
	float objectSpeed = 0.1f;

	//usefull key
	{
		if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			_running = false;// close the window proprely
		}
		if (glfwGetKey(_window, GLFW_KEY_R) == GLFW_PRESS) {
			_cameraX = 0.0f;
			_cameraY = 0.0f;
			_cameraZ = 5.0f;
			_frontX = 0.0f;
			_frontY = 0.0f;
			_frontZ = -1.0f;
			_textureEnabled = false;
			_materialEnabled = false;
			_sunEnabled = false;
			_yaw = -90.0f;
			_pitch = 0.0f;
			_render.objectX = 0.0f;
			_render.objectY = 0.0f;
			_render.objectZ = 0.0f;
			_sunOrbitAngleH = 45.0f;
			_sunOrbitAngleV = 35.26f;
		}
	}

	// Texture
	{
		if (glfwGetKey(_window, GLFW_KEY_T) == GLFW_PRESS) {
			if (!_tKeyPressed) {
				_textureEnabled = !_textureEnabled;// activate/desactivate the texture
				_tKeyPressed = true;
			}
		} else {
			_tKeyPressed = false;
		}

		if (glfwGetKey(_window, GLFW_KEY_F) == GLFW_PRESS) {
			if (!_fKeyPressed) {
				_render.nextTextureFace();// change texture face
				_fKeyPressed = true;

				int currentFace = _render.getTextureFace();// set texture face
				const char* faceNames[] = {"Toutes les faces", "Face X+", "Face X-", "Face Y+", "Face Y-", "Face Z+", "Face Z-"};
				std::cout << "Face texture: " << faceNames[currentFace] << std::endl;
			}
		} else {
			_fKeyPressed = false;
		}
	}

	// Material
	{
		if (glfwGetKey(_window, GLFW_KEY_M) == GLFW_PRESS) {
			if (!_mKeyPressed) {
				_materialEnabled = !_materialEnabled;
				_render.setUseMaterial(_materialEnabled);
				_mKeyPressed = true;
			}
			} else {
				_mKeyPressed = false;
		}
	}

	// Camera movement
	if (_mousePressed) {
		if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			cameraSpeed += 1.0f;// camera speed X10
		} else {
			cameraSpeed = 0.1f;// camera speed X1
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

		if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			_cameraY += cameraSpeed;
		}
		if (glfwGetKey(_window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
			_cameraY -= cameraSpeed;
		}
	}

	// Object movement
	if (!_mousePressed) {
		if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			objectSpeed += 1.0f;// camera speed X10
		} else {
			objectSpeed = 0.3f;// camera speed X1
		}
		if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) {
			_render.objectZ -= objectSpeed; // front
		}
		if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS) {
			_render.objectZ += objectSpeed; // back
		}
		if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) {
			_render.objectX -= objectSpeed; // left
		}
		if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) {
			_render.objectX += objectSpeed;// right
		}
		if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			_render.objectY += objectSpeed;// up
		}
		if (glfwGetKey(_window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
			_render.objectY -= objectSpeed;// down
		}
	}

	// Sun (light point)
	if (glfwGetKey(_window, GLFW_KEY_L) == GLFW_PRESS) {
		if (!_lKeyPressed) {
			_sunEnabled = !_sunEnabled;
			_lKeyPressed = true;
		}
	} else {
			_lKeyPressed = false;
	}
	if (_sunEnabled) {
		float angleSpeed = 2.0f;
		
		if (glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
			angleSpeed = 5.0f;
		}
		
		if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			_sunOrbitAngleH -= angleSpeed;
		}
		if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			_sunOrbitAngleH += angleSpeed;
		}
		if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) {
			_sunOrbitAngleV += angleSpeed;
		}
		if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			_sunOrbitAngleV -= angleSpeed;
		}
		
		updateSunPosition();
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

void Scop::updateSunPosition()
{
	float angleH_rad = _sunOrbitAngleH * M_PI / 180.0f;
	float angleV_rad = _sunOrbitAngleV * M_PI / 180.0f;

	_sunX = _sunOrbitRadius * cos(angleV_rad) * cos(angleH_rad);
	_sunY = _sunOrbitRadius * sin(angleV_rad);
	_sunZ = _sunOrbitRadius * cos(angleV_rad) * sin(angleH_rad);
}
