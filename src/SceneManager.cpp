#include "SceneManager.h"

void SceneManager::Init() {

    mModelManager.Init();

    mModel = Model(mDevice, mCommandList);
    mModel.LoadModel("assets\\cottage_fbx.fbx");

}

void SceneManager::Update(WindowManager& pWindowManager) {

    //回転用ラジアン
    static float radian = 0;
    radian += 0.01f;
    //ワールドマトリックス
    DirectX::XMMATRIX world = DirectX::XMMatrixRotationY(radian);
    //ビューマトリックス
    DirectX::XMVECTOR eye = { 15.f, 0, 0.0f }, focus = { 0, 0, 0 }, up = { 0, 1, 0 };
    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(eye, focus, up);
    //プロジェクションマトリックス
    DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, pWindowManager.GetAspect(), 1.0f, 100.0f);
    //コンスタントバッファ０更新
    DirectX::XMMATRIX worldViewProj = world * view * proj;

    mModel.SetMat(worldViewProj);
}