#pragma once

#include "config.h"

#include "frame.h"

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

	// Instance related variables
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
	vk::SurfaceKHR surface;


	// Device related variables
	vk::PhysicalDevice physicalDevice{ nullptr };
	vk::Device device{ nullptr };
	vk::Queue graphicsQueue{ nullptr };
	vk::Queue presentQueue{ nullptr };
	vk::SwapchainKHR swapchain;
	std::vector<vkUtil::SwapchainFrame> swapchainFrames;
	vk::Format swapchainFormat;
	vk::Extent2D swapchainExtent;


	// general
	const char *appName;

	// pipeline-related variables
	vk::PipelineLayout layout;
	vk::RenderPass renderpass;
	vk::Pipeline pipeline;


	// glfw setup
	void build_glfw_window();

	// instance setup
	void make_instance();

	// device setup
	void make_device();

	// pipeline setup
	void make_pipeline();
};