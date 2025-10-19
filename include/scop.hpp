#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include "obj_parser.hpp"
#include "render_obj.hpp"
#include "texture.hpp"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

class Scop 
{
	public:
		Scop();
		~Scop();
		bool init();
		void cleanup();
		void run();
		bool loadObjFile(const std::string &filename);
		bool loadTexture(const std::string &filename);
		
		static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
		static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	private:
		void handleInput();
		void updateCamera(double xpos, double ypos);
		
		bool _running;
		GLFWwindow *_window;
		renderObj _render;
		Texture _texture;
		
		double _lastMouseX;
		double _lastMouseY;
		bool _firstMouse;
		float _yaw;
		float _pitch;
		bool _mousePressed;
		
		float _cameraX, _cameraY, _cameraZ;
		float _frontX, _frontY, _frontZ;
		
		bool _textureEnabled;
		bool _tKeyPressed;
		bool _fKeyPressed;
};