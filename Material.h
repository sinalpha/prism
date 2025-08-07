#pragma once

#include <string>

#include <DirectXMath.h>

#include "Texture.h"

class Material {

public:

    std::string m_name;

    DirectX::XMFLOAT4 AmbientColor{ DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) };
    DirectX::XMFLOAT4 DiffuseColor{ DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) };
    DirectX::XMFLOAT4 SpecularColor{ DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) };
    DirectX::XMFLOAT4 BaseColor{ DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) };
    DirectX::XMFLOAT4 EmissiveColor{ DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) };

    Texture* pTextures[9] = { 0 };

    float m_transparencyFactor{ 1.0f };
    float m_alphaTest{ 0.0f };

    ~Material();

};