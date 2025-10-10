#include "../include/scop.hpp"

void Scop::handleInput()
{
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		_running = false;
	}
	if (glfwGetKey(_window, GLFW_KEY_M) == GLFW_PRESS) {
		// Toggle material usage
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