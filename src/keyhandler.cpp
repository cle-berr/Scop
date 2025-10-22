#include "../include/scop.hpp"

void Scop::handleInput()
{
	float cameraSpeed = 0.1f;
	float objectSpeed = 0.1f;

	//usefull key
	{
		// ESC: quit the application gracefully by stopping the main loop
		if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			_running = false;// close the window proprely
		}
		// R: full reset of camera pose, object position, lighting/material/texture toggles
		// Also recenters yaw/pitch and resets the sun orbit angles and auto-rotation speed
		if (glfwGetKey(_window, GLFW_KEY_R) == GLFW_PRESS) {
			_cameraX = 0.0f;
			_cameraY = 0.0f;
			_cameraZ = 5.0f;
			_frontX = 0.0f;
			_frontY = 0.0f;
			_frontZ = -1.0f;
			_textureEnabled = false;
			_materialEnabled = false;
			_render.setUseMaterial(_materialEnabled);
			_sunEnabled = false;
			_furEnabled = false;
			_yaw = -90.0f;
			_pitch = 0.0f;
			_render.objectX = 0.0f;
			_render.objectY = 0.0f;
			_render.objectZ = 0.0f;
			_sunOrbitAngleH = 45.0f;
			_sunOrbitAngleV = 35.26f;
			_render.setRotationSpeed(0.03f);

			// Recompute camera and sun distances based on current object size
			positionCameraForObject();
			positionSunForObject();
		}
		// P: toggle auto-rotation of the object (pause/resume). Uses a one-shot lock to avoid repeat.
		if (glfwGetKey(_window, GLFW_KEY_P) == GLFW_PRESS) {
			if (!_pKeyPressed) {
				if (_render.getRotationSpeed() == 0.0f)
					_render.setRotationSpeed(0.03f);
				else
					_render.setRotationSpeed(0.0f);
				_pKeyPressed = true;
			}
		} else {
			_pKeyPressed = false;
		}
		// 1: toggle rotation around X axis
		if (glfwGetKey(_window, GLFW_KEY_1) == GLFW_PRESS) {
			if (!_1KeyPressed) {
				_render._rotateX = !_render._rotateX;
				_1KeyPressed = true;
			}
		} else {
			_1KeyPressed = false;
		} 
		// 2: toggle rotation around Y axis
		if (glfwGetKey(_window, GLFW_KEY_2) == GLFW_PRESS) {
			if (!_2KeyPressed) {
				_render._rotateY = !_render._rotateY;
				_2KeyPressed = true;
			}
		} else {
			_2KeyPressed = false;
		}
		// 3: toggle rotation around Z axis
		if (glfwGetKey(_window, GLFW_KEY_3) == GLFW_PRESS) {
			if (!_3KeyPressed) {
				_render._rotateZ = !_render._rotateZ;
				_3KeyPressed = true;
			}
		} else {
			_3KeyPressed = false;
		}

		if (glfwGetKey(_window, GLFW_KEY_V) == GLFW_PRESS) {
			if (!_vKeyPressed) {
				_viewSun = !_viewSun;
				_vKeyPressed = true;
			}
		} else {
			_vKeyPressed = false;
		}
	}

	// Texture
	{
		// T: toggle texture usage (only if a texture was loaded). One-shot lock avoids key repeat.
		if (glfwGetKey(_window, GLFW_KEY_T) == GLFW_PRESS) {
			if (_texture.hasTexture())
				if (!_tKeyPressed) {
					_textureEnabled = !_textureEnabled;// activate/desactivate the texture
					_tKeyPressed = true;
				}
		} else {
			_tKeyPressed = false;
		}
	}

	// Material
	// Material toggle – requires the model to have a material name in the OBJ/MTL.
	// M: toggles material usage; we auto-enable lighting if it was off to make material visible.
	if (_hasmaterial) {
		if (glfwGetKey(_window, GLFW_KEY_M) == GLFW_PRESS) {
			if (!_mKeyPressed) {
				if (!_sunEnabled)
					_sunEnabled = !_sunEnabled;
				_materialEnabled = !_materialEnabled;
				_render.setUseMaterial(_materialEnabled);
				_mKeyPressed = true;
			}
		} else {
			_mKeyPressed = false;
		}

		if (glfwGetKey(_window, GLFW_KEY_F) == GLFW_PRESS) {
			if (!_fKeyPressed) {
				_furEnabled = !_furEnabled;
				_fKeyPressed = true;
			}
		} else {
			_fKeyPressed = false;
		}
	}

	// Camera movement
	if (_mousePressed) {
		// When mouse is captured (right click), WASD moves the CAMERA.
		// Holding Shift boosts the movement speed for fast navigation.
		if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			cameraSpeed += 1.0f; // speed x10
		} else {
			cameraSpeed = 0.1f;  // default speed
		}

		// Move forward/backward along the camera's forward vector ("_front").
		// This keeps the motion aligned with where the camera is looking.
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

		// Strafe left/right using the camera's RIGHT vector.
		// right = normalize(cross(front, worldUp)) with worldUp = (0,1,0)
		// For the XZ components, this reduces to: right = (-frontZ, 0, frontX).
		if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) {
			_rightX = _frontY * 0.0f - _frontZ * 1.0f; // = -frontZ
			_rightZ = _frontX * 1.0f - _frontY * 0.0f; // =  frontX
			// Normalize the right vector to keep a constant speed.
			_length = sqrt(_rightX * _rightX + _rightZ * _rightZ);
			if (_length > 0) {
				_rightX /= _length;
				_rightZ /= _length;
			}
			_cameraX -= _rightX * cameraSpeed; // move left
			_cameraZ -= _rightZ * cameraSpeed;
		}
		if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) {
			_rightX = _frontY * 0.0f - _frontZ * 1.0f; // = -frontZ
			_rightZ = _frontX * 1.0f - _frontY * 0.0f; // =  frontX
			_length = sqrt(_rightX * _rightX + _rightZ * _rightZ);
			if (_length > 0) {
				_rightX /= _length;
				_rightZ /= _length;
			}
			_cameraX += _rightX * cameraSpeed; // move right
			_cameraZ += _rightZ * cameraSpeed;
		}

		// Move up/down along the world up axis (0,1,0), independent of camera pitch.
		if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			_cameraY += cameraSpeed; // up
		}
		if (glfwGetKey(_window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
			_cameraY -= cameraSpeed; // down
		}
	}

	// Object movement
	if (!_mousePressed) {
		// When mouse is NOT captured, WASD moves the OBJECT instead of the camera.
		// Shift boosts the translation speed of the object.
		objectSpeed = 0.3f;  // Base speed
		if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			objectSpeed = 1.3f;  // Increased speed with Shift
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
	// Lighting controls
	// L: toggle the point light ("sun"). One-shot lock avoids repeat.
	if (glfwGetKey(_window, GLFW_KEY_L) == GLFW_PRESS) {
		if (!_lKeyPressed) {
			_sunEnabled = !_sunEnabled; // toggle sun
			_lKeyPressed = true;
		}
	} else {
			_lKeyPressed = false;
	}
	if (_sunEnabled) {
		float angleSpeed = 2.0f; // degrees per frame
		
		// Hold Left Ctrl to accelerate the orbit speed for faster positioning
		if (glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
			angleSpeed = 5.0f; // faster movement with Ctrl
		}
		
		// Arrow keys adjust the spherical orbit angles
		// LEFT/RIGHT change the horizontal angle, UP/DOWN change the vertical angle
		if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			_sunOrbitAngleH -= angleSpeed; // rotate left
		}
		if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			_sunOrbitAngleH += angleSpeed; // rotate right
		}
		if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) {
			_sunOrbitAngleV += angleSpeed; // rotate up
		}
		if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			_sunOrbitAngleV -= angleSpeed; // rotate down
		}
		
		updateSunPosition(); // recalculate sun position
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
