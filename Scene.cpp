#include "Scene.h"


Scene::Scene() : currentState(nullptr) {}

void Scene::SetState(SceneState* state) {
    if (currentState) {
        currentState->Finalize();
    }
    currentState = state;
    if (currentState) {
        currentState->Initialize();
    }
}

void Scene::Update(Transform1& transform1) {
    if (currentState) {
        currentState->Update(transform1);
    }
}

void Scene::Draw() {
    if (currentState) {
        currentState->Draw();
    }
}