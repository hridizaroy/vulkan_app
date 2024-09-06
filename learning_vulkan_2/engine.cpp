﻿// learning_vulkan_2.cpp : Defines the entry point for the application.
//

#include "engine.h"
#include "instance.h"
#include "logging.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"
#include "framebuffer.h"
#include "commands.h"
#include "sync.h"


Engine::Engine(int width, int height, GLFWwindow* window, const char* appName, bool debugMode)
{
	this->width = width;
	this->height = height;
	this->window = window;
	this->debugMode = debugMode;
	this->appName = appName;

	if (debugMode)
	{
		std::cout << "Creating our Graphics Engine\n";
	}

	make_instance();
	make_device();
	make_pipeline();
	finalize_setup();
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
	// physical device
	physicalDevice = vkInit::choose_physical_device(instance, debugMode);

	// logical device
	device = vkInit::create_logical_device(physicalDevice, surface, debugMode);

	// Queues
	std::array<vk::Queue, 2> queues = vkInit::get_queue(physicalDevice, device, surface, debugMode);
	graphicsQueue = queues[0];
	presentQueue = queues[1];
	
	// Swapchain
	vkInit::SwapchainBundle bundle = vkInit::create_swapchain(device, physicalDevice, surface, width, height, debugMode);
	swapchain = bundle.swapchain;
	swapchainFrames = bundle.frames;
	swapchainFormat = bundle.format;
	swapchainExtent = bundle.extent;
}

void Engine::make_pipeline()
{
	vkInit::GraphicsPipelineInBundle specification = {};
	specification.device = device;
	specification.vertexFilepath = "../../../../learning_vulkan_2/shaders/vertex.spv";
	specification.fragmentFilepath = "../../../../learning_vulkan_2/shaders/fragment.spv";
	specification.swapchainExtent = swapchainExtent;
	specification.swapchainImageFormat = swapchainFormat;

	vkInit::GraphicsPipelineOutBundle output = vkInit::make_graphics_pipeline(specification, debugMode);
	layout = output.layout;
	renderpass = output.renderpass;
	pipeline = output.pipeline;
}

void Engine::finalize_setup()
{
	vkInit::framebufferInput framebufferInput;
	framebufferInput.device = device;
	framebufferInput.renderpass = renderpass;
	framebufferInput.swapchainExtent = swapchainExtent;
	
	vkInit::make_framebuffers(framebufferInput, swapchainFrames, debugMode);

	commandPool = vkInit::make_command_pool(device, physicalDevice, surface, debugMode);

	vkInit::commandBufferInputChunk commandBufferInput = { device, commandPool, swapchainFrames };
	mainCommandBuffer = vkInit::make_command_buffers(commandBufferInput, debugMode);

	imageAvailable = vkInit::make_semaphore(device, debugMode);
	renderFinished = vkInit::make_semaphore(device, debugMode);
	inFlightFence = vkInit::make_fence(device, debugMode);
}

Engine::~Engine()
{
	if (debugMode)
	{
		std::cout << "Bye!\n";
	}

	device.destroySemaphore(imageAvailable);
	device.destroySemaphore(renderFinished);
	device.destroyFence(inFlightFence);

	device.destroyCommandPool(commandPool);

	device.destroyPipeline(pipeline);
	device.destroyPipelineLayout(layout);
	device.destroyRenderPass(renderpass);


	for (const auto& frame : swapchainFrames)
	{
		device.destroyImageView(frame.imageView);
		device.destroyFramebuffer(frame.frameBuffer);
	}

	device.destroySwapchainKHR(swapchain);
	device.destroy();

	instance.destroySurfaceKHR(surface);
	if (debugMode)
	{
		instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
	}

	instance.destroy();

	glfwTerminate();
}
