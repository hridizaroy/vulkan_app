#pragma once

#include "config.h"

namespace vkUtil
{
	struct SwapchainFrame
	{
		vk::Image image;
		vk::ImageView imageView;
	};
}