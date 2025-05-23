#pragma once

#include "tree_set.hpp"
#include "renderer.hpp"
#include "vulkan/render_resources.hpp"
#include <vulkan/vulkan.h>
#include <optional>

enum class RenderNodeType
{
  DefaultModel,
  InstancedModel,
  Skybox
};

struct RenderNode
{
  RenderNode(RenderNodeType type)
    : type(type) {}

  RenderNodeType type;
  MeshHandle mesh;
  MaterialHandle material;

  virtual ~RenderNode() = default;
};

using RenderGraphKey = long;
using RenderNodePtr = std::unique_ptr<RenderNode>;
using RenderGraph = TreeSet<RenderGraphKey, RenderNodePtr>;

struct PipelineKey
{
  RenderPass renderPass;
  std::optional<MeshFeatureSet> meshFeatures;
  std::optional<MaterialFeatureSet> materialFeatures;

  bool operator==(const PipelineKey& rhs) const = default;
};

template<>
struct std::hash<PipelineKey>
{
  std::size_t operator()(const PipelineKey& key) const noexcept
  {
    return hashAll(key.renderPass, key.meshFeatures, key.materialFeatures);
  }
};

struct DefaultModelNode : public RenderNode
{
  DefaultModelNode()
    : RenderNode(RenderNodeType::DefaultModel)
  {}

  Mat4x4f modelMatrix;
};

struct InstancedModelNode : public RenderNode
{
  InstancedModelNode()
    : RenderNode(RenderNodeType::InstancedModel)
  {}

  std::vector<MeshInstance> instances;
};

struct SkyboxNode : public RenderNode
{
  SkyboxNode()
    : RenderNode(RenderNodeType::Skybox)
  {}
};

struct BindState
{
  VkPipeline pipeline;
  std::vector<VkDescriptorSet> descriptorSets;
};

class FileSystem;

class Pipeline
{
  public:
    virtual void onViewportResize(VkExtent2D swapchainExtent) = 0;

    virtual void recordCommandBuffer(VkCommandBuffer commandBuffer, const RenderNode& node,
      BindState& bindState, size_t currentFrame) = 0;

    virtual ~Pipeline() {}
};

using PipelinePtr = std::unique_ptr<Pipeline>;

class Logger;

PipelinePtr createPipeline(RenderPass renderPass, const MeshFeatureSet& meshFeatures,
  const MaterialFeatureSet& materialFeatures, const FileSystem& fileSystem,
  const RenderResources& renderResources, Logger& logger, VkDevice device,
  VkExtent2D swapchainExtent, VkFormat swapchainImageFormat, VkFormat depthFormat);
