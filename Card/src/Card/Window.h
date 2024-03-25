#pragma once

#include "glfw3.h"

class Window
{
public:

private:
	void InitWindow();
	const int width;
	const int height;
	GLFWwindow* window;

};

