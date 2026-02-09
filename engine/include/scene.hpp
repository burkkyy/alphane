#pragma once

#include "device.hpp"
#include "frame_info.hpp"
#include "model.hpp"
#include "pipeline.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <vulkan/vulkan.hpp>

namespace alp {

/**
 * Class for storing a camera and a registry of entitys for rendering
 *
 * @note ASSUMING RENDER TARGET IS MAIN VIEWPORT (DISPLAYED WITH swapchain khr)
 * @note Camera not implemented at all yet
 */
class Scene {
 public:
  Scene(const Scene&) = delete;
  Scene& operator=(const Scene&) = delete;

  struct PushConstantData {
    glm::mat4 transform;
    glm::vec4 color;
  };

  Scene() = default;
  virtual ~Scene();

  virtual void onAttach() {}
  virtual void onUpdate() {}
  virtual void onImGuiRender() {}
  virtual void onDetach();

  void setDevice(Device* device);

  void initialize(vk::RenderPass renderPass);

  void onRender(vk::CommandBuffer commandBuffer, FrameInfo frameInfo);

  const entt::registry& getRegistry() const { return this->registry; }

  void createQuad(glm::vec2 position, float width, float height);

 private:
  void createPipelineLayout();
  void createPipeline(vk::RenderPass renderPass);

  PushConstantData pushConstant;

  Device* device = nullptr;
  std::unique_ptr<Pipeline> pipeline;
  vk::PipelineLayout pipelineLayout;

  entt::registry registry;

  std::unique_ptr<Model> quadModel;  // TEMP
};

}  // namespace alp
