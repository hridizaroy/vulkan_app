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

	// Debug Callback
	vk::DebugUtilsMessengerEXT debugMessenger{ nullptr };

	// Dynamic Instance dispatcher
	// Loads in "instance procedure address"
	// basically, we can create a function pointer to getInstanceProcAddr using this
	// getInstanceProcAddr is used to load in the function to create or destroy the debug messenger
	// We can create this at any point after the creation of the vulkan instance
	vk::DispatchLoaderDynamic dldi;

	// general
	const char *appName;

	// glfw setup
	void build_glfw_window();

	// instance setup
	void make_instance();

	// Debug messenger
	void make_debug_messenger();
};