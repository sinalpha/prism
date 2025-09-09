#include "Scene.h"

#include "Prism.h"

//리펙토링 필요
void Scene::Init(Prism* pPrism) {

	mPrism = pPrism; 

    mModel = Model(mPrism->GetD3DController().GetDevicePtr(), mPrism->GetD3DController().GetCommandListPtr());
    mModel.LoadModel("assets\\cottage_fbx.fbx");

}

//리펙토링 필요
void Scene::Update() {

    //回転用ラジアン
    static float radian = 0;
    radian += 0.01f;
    //ワールドマトリックス
    DirectX::XMMATRIX world = DirectX::XMMatrixRotationY(radian);
    //ビューマトリックス
    DirectX::XMVECTOR eye = { 15.f, 0, 0.0f }, focus = { 0, 0, 0 }, up = { 0, 1, 0 };
    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(eye, focus, up);
    //プロジェクションマトリックス
    DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, mPrism->GetWindowController().GetAspect(), 1.0f, 100.0f);
    //コンスタントバッファ０更新
    DirectX::XMMATRIX worldViewProj = world * view * proj;

    mModel.SetMat(worldViewProj);


}