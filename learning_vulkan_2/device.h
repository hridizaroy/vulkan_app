#pragma once

#include "config.h"

namespace vkInit
{
	void log_device_properties(vk::PhysicalDevice& device)
	{
		// Get device properties
		vk::PhysicalDeviceProperties properties = device.getProperties();

		// log info about the device
		std::cout << "Device name: " << properties.deviceName << "\n";

		std::cout << "Device type: ";

		switch (properties.deviceType)
		{
		case (vk::PhysicalDeviceType::eCpu):
			std::cout << "CPU\n";
			break;

		case (vk::PhysicalDeviceType::eDiscreteGpu):
			std::cout << "Discrete GPU\n";
			break;

		case (vk::PhysicalDeviceType::eIntegratedGpu):
			std::cout << "Integrated GPU\n";
			break;

		case (vk::PhysicalDeviceType::eVirtualGpu):
			std::cout << "Virtual GPU\n";
			break;

		default:
			std::cout << "Other\n";
		}
	}


	bool checkDeviceExtensionSupport(
		const vk::PhysicalDevice& device,
		const std::vector<const char*>& requestedExtensions,
		const bool& debug
	)
	{
		// Create a set to get unique values of requested extensions
		std::set<std::string> requestedExtensionsSet(requestedExtensions.begin(), requestedExtensions.end());

		if (debug)
		{
			std::cout << "Device can support extensions:\n";
		}


		// Check which extensions the device can support
		for (vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties())
		{
			if (debug)
			{
				std::cout << "\t\"" << extension.extensionName << "\"\n";
			}

			// "Check off" this extension from the requested extensions list
			requestedExtensionsSet.erase(extension.extensionName);
		}

		// if all requested extensions were found, this set would be empty
		return requestedExtensionsSet.empty();
	}


	bool isSuitable(const vk::PhysicalDevice& device, const bool debug)
	{
		if (debug)
		{
			std::cout << "Checking if device is suitable...\n";
		}

		// For now, we consider a device suitable if it can present to the screen
		// i.e., Support the swapchain extension
		const std::vector<const char*> requestedExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		if (debug)
		{
			std::cout << "We are requesting device extensions:\n";

			for (const char *extension : requestedExtensions)
			{
				std::cout << "\t\"" << extension << "\"\n";
			}
		}


		// Check if device can support our requested extensions
		if (checkDeviceExtensionSupport(device, requestedExtensions, debug))
		{
			if (debug)
			{
				std::cout << "Device can support the requested extensions!\n";
			}
		}
		else
		{
			if (debug)
			{
				std::cout << "Device cannot support the requested extensions!\n";
			}

			return false;
		}

		return true;
	}


	vk::PhysicalDevice choose_physical_device(vk::Instance& instance, bool debug)
	{
		// Physical devices are neither created nor destroyed. Merely chosen.
		
		if (debug)
		{
			std::cout << "Choosing Physical Device...\n";
		}


		// Get available devices
		std::vector<vk::PhysicalDevice> availableDevices = instance.enumeratePhysicalDevices();
		
		if (debug)
		{
			std::cout << "There are " << availableDevices.size() << " physical device(s) available on this system\n";
		}


		// Check if any device is suitable
		for (vk::PhysicalDevice device : availableDevices)
		{
			if (debug)
			{
				log_device_properties(device);
			}

			if (isSuitable(device, debug))
			{
				return device;
			}
		}

		return nullptr;
	}
}