#pragma once

#include "DirectXMath.h"

class Camera {
public:

	DirectX::XMMATRIX GetLookAtMatrix() const {
		return DirectX::XMMatrixLookAtLH(mEye, mFocus, mUp);
	}

private:


	DirectX::XMVECTOR  mEye{ 10.f, 0.f, 0. };
	DirectX::XMVECTOR  mFocus{ 0.f, 0.f, 0.f };
	DirectX::XMVECTOR  mUp{ 0.f, 1.f, 0.f };


};