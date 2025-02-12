
#ifndef TITLESCENE_H
#define TITLESCENE_H

#include "Transform1.h"

class TitleScene : public SceneState {
public:
    void Initialize() override;
    void Update(Transform1& transform) override;
    void Draw() override;
    void Finalize() override;
};

#endif // TITLESCENE_H
