#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include "obj_parser.hpp"
#include "render_obj.hpp"
#include "texture.hpp"
#include "material.hpp"

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

		void positionCameraForObject();  // Positionne la caméra en fonction de la taille de l'objet
		void positionSunForObject();     // Positionne la lumière (sun) selon la taille de l'objet
		bool _furEnabled;

	private:
		void handleInput();
		void updateCamera(double xpos, double ypos);
		void updateSunPosition();
		
		bool _running;
		GLFWwindow *_window;
		renderObj _render;
		Texture _texture;
		Material _material;
		objParser _parser;
		
		double _lastMouseX;
		double _lastMouseY;
		bool _firstMouse;
		float _yaw;
		float _pitch;
		bool _mousePressed;
		
		float _cameraX, _cameraY, _cameraZ;
		float _frontX, _frontY, _frontZ;
		float _rightX, _rightY, _rightZ;
		float _length;
		float _worldUpX = 0.0f, _worldUpY = 1.0f, _worldUpZ = 0.0f;
		
		float _sunX, _sunY, _sunZ;
		float _sunOrbitRadius;
		float _sunOrbitAngleH;
		float _sunOrbitAngleV;
		
		bool _textureEnabled;
		bool _materialEnabled;
		bool _hasmaterial;
		bool _sunEnabled;
		bool _tKeyPressed;
		bool _fKeyPressed;
		bool _mKeyPressed;
		bool _lKeyPressed;
		bool _pKeyPressed;
		bool _1KeyPressed;
		bool _2KeyPressed;
		bool _3KeyPressed;
		bool _viewSun;
		bool _vKeyPressed;

};