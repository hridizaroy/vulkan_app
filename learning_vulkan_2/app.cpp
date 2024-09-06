#include "app.h"

App::App(int width, int height, bool debug)
{
	build_glfw_window(width, height, debug);

	graphicsEngine = new Engine(width, height, window, appName, debug);
}


void App::build_glfw_window(int width, int height, bool debugMode)
{
	// initialize glfw
	glfwInit();

	// no default rendering client
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// Disable resizing for now
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// width, height, title, monitor, another window that want to share resources with
	if (window = glfwCreateWindow(width, height, appName, nullptr, nullptr))
	{
		if (debugMode)
		{
			std::cout << "Successfully created a GLFW Window with width " << width << " and height " << height << "\n";
		}
	}
	else
	{
		if (debugMode)
		{
			std::cout << "GLFW Window creation failed :/\n";
		}
	}
}


void App::run()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		calculateFrameRate();
	}
}


void App::calculateFrameRate()
{
	currentTime = glfwGetTime();
	double delta = currentTime - lastTime;

	if (delta >= 1)
	{
		int framerate{ std::max(1, int(numFrames / delta)) };

		std::stringstream title;
		title << "Running at " << framerate << " fps.";
		glfwSetWindowTitle(window, title.str().c_str());

		lastTime = currentTime;
		numFrames = -1;
		frameTime = float(1000.0 / framerate); // seconds per frame * 1000 to get ms
	}

	numFrames++;
}


App::~App()
{
	delete graphicsEngine;
}