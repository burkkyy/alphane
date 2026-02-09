#include "scene_manager.hpp"

namespace alp {

SceneManager::SceneManager() = default;

SceneManager::~SceneManager() = default;

void SceneManager::registerScene(std::unique_ptr<Scene> scene) {
  if (this->activeScene) { this->activeScene.reset(); }

  this->activeScene = std::move(scene);
}

void SceneManager::onAttachActiveScene() {
  if (this->activeScene) { this->activeScene->onAttach(); }
}

void SceneManager::onUpdateActiveScene() {
  if (this->activeScene) { this->activeScene->onUpdate(); }
}

void SceneManager::onRenderActiveScene() {
  if (this->activeScene) { this->activeScene->onRender(); }
}

void SceneManager::onImGuiRenderActiveScene() {
  if (this->activeScene) { this->activeScene->onImGuiRender(); }
}

void SceneManager::onDetachActiveScene() {
  if (this->activeScene) { this->activeScene->onDetach(); }
}

}  // namespace alp
