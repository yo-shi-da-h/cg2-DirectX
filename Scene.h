#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Transform1.h"

class Scene {
public:
    Scene();

    void SetState(SceneState* state);
    void Update(Transform1& transform);
    void Draw();

private:

    SceneState* currentState;
};

#endif // SCENEMANAGER_H