// learning_vulkan_2.cpp : Defines the entry point for the application.
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
	renderPass = output.renderpass;
	pipeline = output.pipeline;
}

void Engine::finalize_setup()
{
	vkInit::framebufferInput framebufferInput;
	framebufferInput.device = device;
	framebufferInput.renderpass = renderPass;
	framebufferInput.swapchainExtent = swapchainExtent;
	
	vkInit::make_framebuffers(framebufferInput, swapchainFrames, debugMode);

	commandPool = vkInit::make_command_pool(device, physicalDevice, surface, debugMode);

	vkInit::commandBufferInputChunk commandBufferInput = { device, commandPool, swapchainFrames };
	mainCommandBuffer = vkInit::make_command_buffers(commandBufferInput, debugMode);

	imageAvailable = vkInit::make_semaphore(device, debugMode);
	renderFinished = vkInit::make_semaphore(device, debugMode);
	inFlightFence = vkInit::make_fence(device, debugMode);
}

void Engine::record_draw_commands(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
	vk::CommandBufferBeginInfo beginInfo = {};

	try
	{
		commandBuffer.begin(beginInfo);
	}
	catch (vk::SystemError err)
	{
		if (debugMode)
		{
			std::cout << "Failed to begin recording command buffer :/" << std::endl;
		}
	}

	vk::RenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapchainFrames[imageIndex].frameBuffer;
	renderPassInfo.renderArea.offset.x = 0;
	renderPassInfo.renderArea.offset.y = 0;
	renderPassInfo.renderArea.extent = swapchainExtent;

	vk::ClearValue clearColor = { std::array<float, 4>{0.2f, 0.1f, 0.9f, 1.0f} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

	commandBuffer.draw(3, 1, 0, 0);

	commandBuffer.endRenderPass();

	try
	{
		commandBuffer.end();
	}
	catch (vk::SystemError err)
	{
		if (debugMode)
		{
			std::cout << "Failed to finish recording command buffer :/" << std::endl;
		}
	}
}

void Engine::render()
{
	device.waitForFences(1, &inFlightFence, VK_TRUE, UINT64_MAX);
	device.resetFences(1, &inFlightFence);

	// Acquire next image
	uint32_t imageIndex{ device.acquireNextImageKHR(swapchain, UINT64_MAX, imageAvailable, nullptr).value };

	vk::CommandBuffer commandBuffer = swapchainFrames[imageIndex].commandBuffer;

	commandBuffer.reset();

	record_draw_commands(commandBuffer, imageIndex);

	vk::SubmitInfo submitInfo = {};

	vk::Semaphore waitSemaphores[] = { imageAvailable };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vk::Semaphore signalSemaphores[] = { renderFinished };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	try
	{
		graphicsQueue.submit(submitInfo, inFlightFence);
	}
	catch (vk::SystemError err)
	{
		if (debugMode)
		{
			std::cout << "Failed to submit draw command buffer :/" << std::endl;
		}
	}

	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	vk::SwapchainKHR swapchains[] = { swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;

	presentQueue.presentKHR(presentInfo);
}

Engine::~Engine()
{
	device.waitIdle(); // wait until device is idle

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
	device.destroyRenderPass(renderPass);


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
