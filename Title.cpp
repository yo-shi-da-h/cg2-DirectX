
#include "Title.h"
#include "Transform1.h"

void TitleScene::Initialize() {
    // タイトルシーンの初期化処理
}

void TitleScene::Update(Transform1& transform) {
    // タイトルシーンの更新処理
    transform.rotate.y += 0.03f;
}

void TitleScene::Draw() {
    // タイトルシーンの描画処理
}

void TitleScene::Finalize() {
    // タイトルシーンの終了処理
}
