/*
 * Copyright (c) 2023 Valve Corporation
 * Copyright (c) 2023 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 */

#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <map>
#include "spirv_reflect.h"

struct VulkanInstance;

class Hopper {
  public:
    Hopper(VulkanInstance& vk, size_t file_size, const void* spirv_data);
    ~Hopper();

    bool Run();

  private:
    VulkanInstance& vk;

    size_t file_size;
    const void* spirv_data;

    bool Reflect();
    bool CreateShaderStage(size_t code_size, const void* code, VkShaderStageFlagBits stage, const char* name = "main");
    bool CreatePipelineLayout();
    bool CreateVertexAttributeDescriptions(const SpvReflectInterfaceVariable& variable);
    bool CreateGraphicsPipeline();
    bool CreateGraphicsMeshPipeline();
    bool CreateComputePipeline();

    // For Pass Through shaders
    bool IsBuiltinType(const SpvReflectInterfaceVariable& variable);
    std::string GetTypeDescription(SpvReflectTypeDescription& description);
    std::string DefineCustomStruct(const SpvReflectTypeDescription& description);
    std::string GetMatchingGeometryBuiltins(const SpvReflectTypeDescription& description);
    void DefineAllStruct(const SpvReflectTypeDescription& description);
    bool BuildPassThroughShader(std::string& source, VkShaderStageFlagBits stage);
    bool CreatePassThroughVertex();
    bool CreatePassThroughVertexNoInterface();
    bool CreatePassThroughTessellationEval();
    bool CreatePassThroughTessellationControl();
    bool CreatePassThroughMesh();

    SpvReflectShaderModule module;

    SpvReflectEntryPoint entry_point;
    VkShaderStageFlagBits shader_stage;

    std::vector<SpvReflectInterfaceVariable*> input_variables;
    std::vector<SpvReflectInterfaceVariable*> output_variables;
    std::vector<SpvReflectDescriptorSet*> descriptor_sets;
    std::vector<SpvReflectBlockVariable*> push_constants;
    std::vector<const SpvReflectDescriptorBinding*> input_attachments;

    // For passthrough shaders who need to build this up recursively
    std::map<uint32_t, const SpvReflectTypeDescription*> struct_ordered_map;

    std::vector<VkAttachmentReference> color_attachment_references;
    std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachments;
    std::vector<VkVertexInputAttributeDescription> vertex_input_attributes;

    std::vector<VkShaderModule> shader_modules;
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages_info;

    VkRenderPass render_pass = VK_NULL_HANDLE;
    VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;

    uint32_t block_location;

    // TileImageColorReadAccessEXT, TileImageDepthReadAccessEXT, or TileImageStencilReadAccessEXT
    bool has_shader_tile_image = false;
};
