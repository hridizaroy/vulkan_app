#pragma once

#include "config.h"
#include "queue_families.h"

namespace vkInit
{
	vk::CommandPool make_command_pool(vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, bool debug)
	{
		vkUtil::QueueFamilyIndices queueFamilyIndices = vkUtil::findQueueFamilies(physicalDevice, surface, debug);

		vk::CommandPoolCreateInfo poolInfo = {};
		poolInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		try
		{
			return device.createCommandPool(poolInfo);
		}
		catch (vk::SystemError err)
		{
			if (debug)
			{
				std::cout << "Failed to create Command Pool :/" << std::endl;
			}

			return nullptr;
		}
	}
}