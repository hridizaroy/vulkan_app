#pragma once

#include "config.h"
#include "logging.h"

namespace vkInit
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete()
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapchainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	struct SwapchainBundle
	{
		vk::SwapchainKHR swapchain;
		std::vector<vk::Image> images;
		vk::Format format;
		vk::Extent2D extent;
	};


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

		vk::PhysicalDevice chosenDevice = nullptr;

		// Check if any device is suitable
		for (vk::PhysicalDevice device : availableDevices)
		{
			if (debug)
			{
				log_device_properties(device);
			}

			if (isSuitable(device, debug))
			{
				chosenDevice = device;
			}
		}

		return chosenDevice;
	}


	QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface, bool debug)
	{
		QueueFamilyIndices indices;

		// Get queue families for device
		std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

		if (debug)
		{
			std::cout << "System can support " << queueFamilies.size() << " queue families.\n";
		}


		// Go through each device queue family and add indices accordingly
		int idx = 0;
		for (vk::QueueFamilyProperties queueFamily : queueFamilies)
		{
			// check if this is a graphics queue family
			if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
			{
				indices.graphicsFamily = idx;

				if (debug)
				{
					std::cout << "Queue Family " << idx << " is suitable for graphics.\n";
				}
			}

			if (device.getSurfaceSupportKHR(idx, surface))
			{
				indices.presentFamily = idx;

				if (debug)
				{
					std::cout << "Queue Family " << idx << " is suitable for presenting.\n";
				}
			}

			
			// if we found all needed queue family indices, break
			if (indices.isComplete())
			{
				break;
			}

			idx++;
		}


		return indices;
	}


	vk::Device create_logical_device(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, bool debug)
	{
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface, debug);

		// Get unique indices for queue families
		std::vector<uint32_t> uniqueIndices;
		uniqueIndices.push_back(indices.graphicsFamily.value());

		if (indices.graphicsFamily.value() != indices.presentFamily.value())
		{
			uniqueIndices.push_back(indices.presentFamily.value());
		}

		// Queue priority determines how GPU allocates its resources towards different queues
		// in the same queue family
		// 0.0 = lowest, 1.0 = highest
		float queuePriority = 1.0f;

		// Queue info
		// flags, queue family index, queue count, pQueuePriorities
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;

		for (uint32_t queueFamilyIdx : uniqueIndices)
		{
			queueCreateInfo.push_back(
				vk::DeviceQueueCreateInfo(
					vk::DeviceQueueCreateFlags(),
					queueFamilyIdx,
					1,
					&queuePriority
				)
			);
		}

		// Request swapchain extension
		std::vector<const char*> deviceExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};



		// Device features
		// We can enable features in this if we want
		// e.g., deviceFeatures.samplerAnisotropy = true
		vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();


		// Enabled layers
		std::vector<const char*> enabledLayers;

		if (debug)
		{
			enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
		}


		// Device Create info
		// flags, queueCreateInfoCount, pQueueCreateInfos, enabledLayerCount, enabled layers,
		// enabledExtensionCount, enabled extensions, pEnabledFeatures
		vk::DeviceCreateInfo deviceInfo = vk::DeviceCreateInfo(
			vk::DeviceCreateFlags(),
			queueCreateInfo.size(), queueCreateInfo.data(),
			enabledLayers.size(),
			enabledLayers.data(),
			deviceExtensions.size(), deviceExtensions.data(),
			&deviceFeatures
		);


		// Create the device
		try
		{
			vk::Device device = physicalDevice.createDevice(deviceInfo);

			if (debug)
			{
				std::cout << "Logical device created!\n";
			}
			
			return device;
		}
		catch (vk::SystemError err)
		{
			if (debug)
			{
				std::cout << "Logical device creation failed.\n";

				return nullptr;
			}
		}

		return nullptr;
	}


	std::array<vk::Queue, 2> get_queue(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface, bool debug)
	{
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface, debug);

		// queue family index, queue index
		return
		{
			device.getQueue(indices.graphicsFamily.value(), 0),
			device.getQueue(indices.presentFamily.value(), 0)
		};
	}


	SwapchainSupportDetails query_swapchain_support(vk::PhysicalDevice device, vk::SurfaceKHR surface, bool debug)
	{
		SwapchainSupportDetails support;

		// Capabilities
		support.capabilities = device.getSurfaceCapabilitiesKHR(surface);

		if (debug)
		{
			std::cout << "Swapchain can support the following surface capabilities:\n";

			std::cout << "\tMinimum image count: " << support.capabilities.minImageCount << "\n";
			std::cout << "\tMaximum image count: " << support.capabilities.maxImageCount << "\n";

			std::cout << "\tCurrent extent:\n";

			std::cout << "\t\tWidth: " << support.capabilities.currentExtent.width << "\n";
			std::cout << "\t\tHeight: " << support.capabilities.currentExtent.height << "\n";

			std::cout << "\tMinimum supported extent:\n";
			std::cout << "\t\tWidth: " << support.capabilities.minImageExtent.width << "\n";
			std::cout << "\t\tHeight: " << support.capabilities.minImageExtent.height << "\n";

			std::cout << "\tMaximum supported extent:\n";
			std::cout << "\t\tWidth: " << support.capabilities.maxImageExtent.width << "\n";
			std::cout << "\t\tHeight: " << support.capabilities.maxImageExtent.height << "\n";

			std::cout << "\tMaximum image array layers: " << support.capabilities.maxImageArrayLayers << "\n";

			std::cout << "\tSupported transforms:\n";
			std::vector<std::string> stringList = log_transform_bits(support.capabilities.supportedTransforms);
			for (std::string line : stringList)
			{
				std::cout << "\t\t" << line << "\n";
			}

			std::cout << "\tCurrent transforms:\n";
			stringList = log_transform_bits(support.capabilities.currentTransform);
			for (std::string line : stringList)
			{
				std::cout << "\t\t" << line << "\n";
			}

			std::cout << "\tSupported alpha operations:\n";
			stringList = log_alpha_composite_bits(support.capabilities.supportedCompositeAlpha);
			for (std::string line : stringList)
			{
				std::cout << "\t\t" << line << "\n";
			}

			std::cout << "\tSupported image usage:\n";
			stringList = log_image_usage_bits(support.capabilities.supportedUsageFlags);
			for (std::string line : stringList)
			{
				std::cout << "\t\t" << line << "\n";
			}
		}


		// Formats
		support.formats = device.getSurfaceFormatsKHR(surface);

		if (debug)
		{
			for (vk::SurfaceFormatKHR supportedFormat : support.formats)
			{
				std::cout << "Supported pixel format: " << vk::to_string(supportedFormat.format) << "\n";
				std::cout << "Supported color space: " << vk::to_string(supportedFormat.colorSpace) << "\n";
			}
		}


		// Present modes
		support.presentModes = device.getSurfacePresentModesKHR(surface);

		for (vk::PresentModeKHR presentMode : support.presentModes)
		{
			std::cout << "\t" << log_present_mode(presentMode) << "\n";
		}


		return support;
	}


	vk::SurfaceFormatKHR choose_swapchain_surface_format(std::vector<vk::SurfaceFormatKHR> formats)
	{
		// Check if our preferred format is available
		for (vk::SurfaceFormatKHR format : formats)
		{
			if (format.format == vk::Format::eB8G8R8A8Unorm
				&& format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			{
				return format;
			}
		}

		// Otherwise, just return any format
		return formats[0];
	}


	vk::PresentModeKHR choose_swapchain_present_mode(std::vector<vk::PresentModeKHR> presentModes)
	{
		// Check if our preferred presentMode is available
		for (vk::PresentModeKHR presentMode : presentModes)
		{
			if (presentMode == vk::PresentModeKHR::eMailbox)
			{
				return presentMode;
			}
		}

		// Otherwise, return fifo since it's guaranteed to exist
		return vk::PresentModeKHR::eFifo;
	}


	vk::Extent2D choose_swapchain_extent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilities)
	{
		// UINT32_MAX => You're allowed to have the image extent differ from the window extent
		if (capabilities.currentExtent.width != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}
		else
		{
			vk::Extent2D extent = { width, height };

			// if image extent < min extent, choose min extent
			// if image extent > max extent, choose max extent

			extent.width = std::min(
				capabilities.maxImageExtent.width,
				std::max(capabilities.minImageExtent.width, width)
			);

			extent.height = std::min(
				capabilities.maxImageExtent.height,
				std::max(capabilities.minImageExtent.height, height)
			);
		}
	}


	SwapchainBundle create_swapchain(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, int width, int height, bool debug)
	{
		if (debug)
		{
			std::cout << "Creating Swapchain...\n";
		}

		SwapchainSupportDetails support = query_swapchain_support(physicalDevice, surface, debug);

		vk::SurfaceFormatKHR format = choose_swapchain_surface_format(support.formats);

		vk::PresentModeKHR presentMode = choose_swapchain_present_mode(support.presentModes);

		vk::Extent2D extent = choose_swapchain_extent(width, height, support.capabilities);

		// Increase frame rate by requesting 1 additional image
		uint32_t imageCount = std::min(
			support.capabilities.maxImageCount,
			support.capabilities.minImageCount + 1
		);


		// flags, surface, minImageCount, imageFormat, imageColorSpace, imageExtent
		// imageArrayLayers, imageUsage
		vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR(
			vk::SwapchainCreateFlagsKHR(), surface, imageCount, format.format,
			format.colorSpace, extent, 1, vk::ImageUsageFlagBits::eColorAttachment
		);

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface, debug);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (queueFamilyIndices[0] != queueFamilyIndices[1])
		{
			createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		}
		
		createInfo.preTransform = support.capabilities.currentTransform;
		createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);


		SwapchainBundle bundle{};

		try
		{
			bundle.swapchain = logicalDevice.createSwapchainKHR(createInfo);

			if (debug)
			{
				std::cout << "Successfully created swapchain!\n";
			}
		}
		catch (vk::SystemError err)
		{
			throw std::runtime_error("Failed to create swapchain :/\n");
		}

		bundle.images = logicalDevice.getSwapchainImagesKHR(bundle.swapchain);
		bundle.format = format.format;
		bundle.extent = extent;


		return bundle;
	}
}