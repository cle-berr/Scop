#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include "obj_parser.hpp"
#include "render_obj.hpp"

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

	private:
		void handleInput();
		
		bool _running;
		GLFWwindow *_window;
		renderObj _render;
};