#include "GraphicsPipeline.h"

namespace Card {
    GraphicsPipeline::GraphicsPipeline()
    {

    }
    GraphicsPipeline::GraphicsPipeline(std::string filepathVertex, std::string filepathFragment, VkDevice device, VkRenderPass renderPass, VkDescriptorSetLayout* descriptorSetLayout)
    {
        createGraphicsPipeline(filepathVertex, filepathFragment,device,renderPass,descriptorSetLayout);
    }

    GraphicsPipeline::~GraphicsPipeline()
    {
       
    }

    void GraphicsPipeline::destroyPipeline(VkDevice device)
    {
        vkDestroyPipeline(device, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    }

    void GraphicsPipeline::createGraphicsPipeline(std::string filepathVertex, std::string filepathFragment, VkDevice device, VkRenderPass renderPass, VkDescriptorSetLayout* descriptorSetLayout)
    {
        //creating/getting shaders
        auto vertShaderCode = readFile(filepathVertex);
        CARD_ENGINE_INFO(vertShaderCode.size());
        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode,device);
        auto fragShaderCode = readFile(filepathFragment);
        CARD_ENGINE_INFO(fragShaderCode.size());
        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode,device);

        //adding shader to specific pipeline stage

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";
        //vertShaderStageInfo.pSpecializationInfo  
        //'You can use a single shader module where its behavior can be configured at pipeline creation by specifying different values for the constants used in it.'

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

        //fixed functionshaders 
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{}; //The format of the vertex data that will be passed to the vertex shader.
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();


        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};//Kind of geometry will be drawn from the vertices and if primitive restart should be enabled.
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE; //if true certain gpu feature should be enabled. usefull for shadow maps
        rasterizer.rasterizerDiscardEnable = VK_FALSE; //if true geometry never passes through the rasterize phase
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL; //Using any mode other than fill requires enabling a GPU feature.
        rasterizer.lineWidth = 1.0f; //anything other than 1.0f requires enabling GPU feature
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;


        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        //allow for dynamic viewports and scissors
        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayout; 
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;


        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to create pipeline layout!");
        }
        else {
            CARD_ENGINE_INFO("succesfully created pipeline layout");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;

        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;

        pipelineInfo.layout = pipelineLayout;

        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to create graphics pipeline!");
        }
        else {
            CARD_ENGINE_INFO("succesfully created graphics pipeline");
        }

        //removing shader modules
        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        vkDestroyShaderModule(device, vertShaderModule, nullptr);
    }

    /// <summary>
    /// read a file
    /// </summary>
    /// <param name="filepath"></param>
    /// <returns></returns>
    std::vector<char> GraphicsPipeline::readFile(std::string filepath)
    {
        //read the file starting from the end as a binary file
        std::ifstream file(filepath, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            CARD_ENGINE_ERROR("failed to open file!");
            return std::vector<char>();
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
        
    }

    VkShaderModule GraphicsPipeline::createShaderModule(const std::vector<char>& code, VkDevice device)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to create shadermodule!");
        }
        else
        {
            CARD_ENGINE_INFO("succes in creating shadermodule!");
        }

        return shaderModule;
    }
    VkPipelineLayout GraphicsPipeline::getpipelineLayout()
    {
        return pipelineLayout;
    }
    VkPipeline GraphicsPipeline::getgraphicsPipeline()
    {
        return graphicsPipeline;
    }
}