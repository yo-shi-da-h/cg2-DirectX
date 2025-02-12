#ifndef TRANSFORM1_H
#define TRANSFORM1_H

#include "Vector3.h"

struct Transform1 {
    Vector3 scale;
    Vector3 rotate;
    Vector3 translate;
};

class SceneState {
public:
    virtual ~SceneState() = default;
    virtual void Initialize() = 0;
    virtual void Update(Transform1& transform) = 0;
    virtual void Draw() = 0;
    virtual void Finalize() = 0;
};

#endif // TRANSFORM1_H