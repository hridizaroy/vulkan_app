#pragma once

#include <GLFW/glfw3.h>
#include "config.h"

class Engine
{
public:
	Engine();

	~Engine();

private:
	bool debugMode = true;

	// glfw window params
	int width{ 800 };
	int height{ 600 };
	GLFWwindow* window{ nullptr };

	// vulkan instance
	vk::Instance instance{ nullptr };

	// general
	const char *appName;

	// glfw setup
	void build_glfw_window();

	// instance setup
	void make_instance();
};