#include "scene.hpp"

#include <cassert>
#include <iostream>

#include "components.hpp"
#include "shaders/internal_scene_frag.h"
#include "shaders/internal_scene_vert.h"

namespace alp {

Scene::~Scene() {
  log::trace("destroyed vk::PipelineLayout");
  this->device->get()->destroyPipelineLayout(this->pipelineLayout);
}

void Scene::initialize(vk::RenderPass renderPass) {
  assert(this->device != nullptr &&
         "Cannot initialize a Scene with a null device");

  this->pipeline = std::make_unique<Pipeline>(*this->device);

  createPipelineLayout();
  createPipeline(renderPass);

  this->pushConstant.transform = glm::mat4(1.0f);

  // TEMP
  Model::Builder quadBuilder{};
  quadBuilder.vertices = {{{-1.0f, -1.0f}, {1, 0, 0}},
                          {{-1.0f, 1.0f}, {1, 0, 0}},
                          {{1.0f, 1.0f}, {0, 1, 0}},
                          {{1.0f, -1.0f}, {0, 0, 1}}};
  quadBuilder.indices = {0, 1, 2, 0, 2, 3};

  this->quadModel = std::make_unique<Model>(this->device, quadBuilder);

  // auto descriptorPool = DescriptorPool::Builder(this->device)
  //                           .setMaxSets(1)
  //                           .addPoolSize(vk::DescriptorType::eStorageBuffer,
  //                           1) .build();
}

void Scene::onRender(vk::CommandBuffer commandBuffer, FrameInfo frameInfo) {
  this->pipeline->bind(commandBuffer);

  this->quadModel->bind(commandBuffer);

  commandBuffer.pushConstants(
      this->pipelineLayout,
      vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0,
      sizeof(PushConstantData), &this->pushConstant);

  this->quadModel->draw(commandBuffer);
}

void Scene::onDetach() {
  auto view = this->registry.view<Quad2D>();

  std::cout << "Detaching scene - Found " << view.size() << " quads:\n";

  for (auto entity : view) {
    std::cout << "Entity ID: " << static_cast<uint32_t>(entity) << std::endl;
  }
}

void Scene::setDevice(Device* device) { this->device = device; }

void Scene::createQuad(glm::vec2 position, float width, float height) {
  //   Model::Builder quadBuilder{};
  // quadBuilder.vertices = {
  //     {{position.x, position.y}, {1, 0, 0}},
  //     {{position.x + width, position.y}, {1, 0, 0}},
  //     {{position.x, position.y + height}, {0, 1, 0}},
  //     {{position.x + width, position.y + height}, {0, 0, 1}}};
  // quadBuilder.indices = {0, 1, 2, 1, 2, 3};

  // std::unique_ptr<Model> quad =
  //     std::make_unique<Model>(this->device, quadBuilder);

  // this->quads.push_back(std::move(quad));

  entt::entity entity = this->registry.create();
  this->registry.emplace<Quad2D>(entity, width, height,
                                 glm::vec4{0, 0, 1.0, 1.0});
  this->registry.emplace<Transform2D>(entity, position);
}

void Scene::createPipelineLayout() {
  vk::PushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags =
      vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(PushConstantData);

  vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  try {
    this->pipelineLayout =
        this->device->get()->createPipelineLayout(pipelineLayoutInfo);
    log::trace("created vk::PipelineLayout");
  } catch (const vk::SystemError& err) {
    log::fatal("failed to create vk::PipelineLayout");
    throw std::runtime_error("failed to create vk::PipelineLayout");
  }
}

void Scene::createPipeline(vk::RenderPass renderPass) {
  assert(pipelineLayout != nullptr &&
         "Cannot create pipeline before pipeline layout");

  this->pipeline->create(internal_scene_vert_spv, internal_scene_vert_spv_len,
                         internal_scene_frag_spv, internal_scene_frag_spv_len,
                         this->pipelineLayout, renderPass);
}

}  // namespace alp
