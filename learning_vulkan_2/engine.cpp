// learning_vulkan_2.cpp : Defines the entry point for the application.
//

#include "engine.h"
#include "instance.h"
#include "logging.h"
#include "device.h"

Engine::Engine()
{
	if (debugMode)
	{
		std::cout << "Creating our Graphics Engine\n";
	}

	appName = "Hridiza's Vulkan app";

	build_glfw_window();
	make_instance();
	make_device();
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
	// Create Vulkan instance
	instance = vkInit::make_instance(debugMode, appName);
	
	// Create dispatch loader to assist with debug messenger
	dldi = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);

	// Create Debug messenger
	if (debugMode)
	{
		debugMessenger = vkInit::make_debug_messenger(instance, dldi);
	}

	// Create surface
	VkSurfaceKHR c_style_surface;
	if (glfwCreateWindowSurface(instance, window, nullptr, &c_style_surface) != VK_SUCCESS)
	{
		if (debugMode)
		{
			std::cout << "Failed to abstract the glfw surface for Vulkan.\n";
		}
	}
	else if (debugMode)
	{
		std::cout << "Successfully abstracted the glfw surface for Vulkan.\n";
	}

	surface = c_style_surface;
}

void Engine::make_device()
{
	physicalDevice = vkInit::choose_physical_device(instance, debugMode);
	device = vkInit::create_logical_device(physicalDevice, surface, debugMode);
	std::array<vk::Queue, 2> queues = vkInit::get_queue(physicalDevice, device, surface, debugMode);
	graphicsQueue = queues[0];
	presentQueue = queues[1];
}

Engine::~Engine()
{
	if (debugMode)
	{
		std::cout << "Bye!\n";
	}

	device.destroy();

	instance.destroySurfaceKHR(surface);
	if (debugMode)
	{
		instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
	}

	instance.destroy();

	glfwTerminate();
}
