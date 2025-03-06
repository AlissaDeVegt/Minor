#pragma once
#include "../Base.h"
#include "../Logger.h"
#include "Vertex.h"

#include <string>
#include <iostream>
#include <optional>
#include <set>
#include <fstream>

#define VK_USE_PLATFORM_WIN64_KHR
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"


namespace Card {
	class CARD_API GraphicsPipeline
	{
	public:
		GraphicsPipeline();
		GraphicsPipeline(std::string filepathVertex, std::string filepathFragment, VkDevice device, VkRenderPass renderPass, VkDescriptorSetLayout *descriptorSetLayout);
		~GraphicsPipeline();

		void createGraphicsPipeline(std::string filepathVertex, std::string filepathFragment, VkDevice device, VkRenderPass renderPass, VkDescriptorSetLayout *descriptorSetLayout);
		void destroyPipeline(VkDevice device);

		static std::vector<char> readFile(std::string filename);

		VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice device);
		VkPipelineLayout getpipelineLayout();
		VkPipeline getgraphicsPipeline();

	private:
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;
	};


}


