// learning_vulkan_2.cpp : Defines the entry point for the application.
//

#include "engine.h"
#include "instance.h"
#include "logging.h"

Engine::Engine()
{
	if (debugMode)
	{
		std::cout << "Creating our Graphics Engine\n";
	}

	appName = "Hridiza's Vulkan app";

	build_glfw_window();
	make_instance();
	make_debug_messenger();
}

void Engine::build_glfw_window()
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

void Engine::make_instance()
{
	instance = vkInit::make_instance(debugMode, appName);
	
	dldi = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
}

void Engine::make_debug_messenger()
{
	debugMessenger = vkInit::make_debug_messenger(instance, dldi);
}

Engine::~Engine()
{
	if (debugMode)
	{
		std::cout << "Bye!\n";
	}

	instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);

	instance.destroy();

	glfwTerminate();
}
