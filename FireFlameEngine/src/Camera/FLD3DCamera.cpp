#include "FLD3DCamera.h"
#include "..\MathHelper\FLMathHelper.h"

namespace FireFlame {
D3DCamera::D3DCamera()
{
	SetLens(0.25f*MathHelper::FL_PI, 1.0f, 1.0f, 1000.0f);
}

D3DCamera::~D3DCamera() {}

DirectX::XMVECTOR D3DCamera::GetPosition() const
{
	return XMLoadFloat3(&mPosition);
}

DirectX::XMFLOAT3 D3DCamera::GetPosition3f() const
{
	return mPosition;
}

void D3DCamera::SetPosition(float x, float y, float z)
{
	mPosition = DirectX::XMFLOAT3(x, y, z);
	mViewDirty = true;
}

void D3DCamera::SetPosition(const DirectX::XMFLOAT3& v)
{
	mPosition = v;
	mViewDirty = true;
}

DirectX::XMVECTOR D3DCamera::GetRight() const
{
	return XMLoadFloat3(&mRight);
}

DirectX::XMFLOAT3 D3DCamera::GetRight3f() const
{
	return mRight;
}

DirectX::XMVECTOR D3DCamera::GetUp() const
{
	return XMLoadFloat3(&mUp);
}

DirectX::XMFLOAT3 D3DCamera::GetUp3f() const
{
	return mUp;
}

DirectX::XMVECTOR D3DCamera::GetLook() const
{
	return XMLoadFloat3(&mLook);
}

DirectX::XMFLOAT3 D3DCamera::GetLook3f() const
{
	return mLook;
}

float D3DCamera::GetNearZ() const
{
	return mNearZ;
}

float D3DCamera::GetFarZ() const
{
	return mFarZ;
}

float D3DCamera::GetAspect() const
{
	return mAspect;
}

float D3DCamera::GetFovY() const
{
	return mFovY;
}

float D3DCamera::GetFovX() const
{
	float halfWidth = 0.5f*GetNearWindowWidth();
	return 2.0f*atan(halfWidth / mNearZ);
}

float D3DCamera::GetNearWindowWidth() const
{
	return mAspect * mNearWindowHeight;
}

float D3DCamera::GetNearWindowHeight() const
{
	return mNearWindowHeight;
}

float D3DCamera::GetFarWindowWidth() const
{
	return mAspect * mFarWindowHeight;
}

float D3DCamera::GetFarWindowHeight() const
{
	return mFarWindowHeight;
}

void D3DCamera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf( 0.5f*mFovY );
	mFarWindowHeight  = 2.0f * mFarZ * tanf( 0.5f*mFovY );

    DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProj, P);
}

void D3DCamera::LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp)
{
    DirectX::XMVECTOR L = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(target, pos));
    DirectX::XMVECTOR R = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(worldUp, L));
    DirectX::XMVECTOR U = DirectX::XMVector3Cross(L, R);

	XMStoreFloat3(&mPosition, pos);
	XMStoreFloat3(&mLook, L);
	XMStoreFloat3(&mRight, R);
	XMStoreFloat3(&mUp, U);

	mViewDirty = true;
}

void D3DCamera::LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
{
    DirectX::XMVECTOR P = XMLoadFloat3(&pos);
    DirectX::XMVECTOR T = XMLoadFloat3(&target);
    DirectX::XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);

	mViewDirty = true;
}

DirectX::XMMATRIX D3DCamera::GetView()const
{
	assert(!mViewDirty);
	return XMLoadFloat4x4(&mView);
}

DirectX::XMMATRIX D3DCamera::GetProj() const
{
	return XMLoadFloat4x4(&mProj);
}

DirectX::XMFLOAT4X4 D3DCamera::GetView4x4f() const
{
	assert(!mViewDirty);
	return mView;
}

DirectX::XMFLOAT4X4 D3DCamera::GetProj4x4f() const
{
	return mProj;
}

void D3DCamera::Strafe(float d)
{
	// mPosition += d*mRight
    DirectX::XMVECTOR s = DirectX::XMVectorReplicate(d);
    DirectX::XMVECTOR r = DirectX::XMLoadFloat3(&mRight);
    DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, DirectX::XMVectorMultiplyAdd(s, r, p));

	mViewDirty = true;
}

void D3DCamera::Walk(float d)
{
	// mPosition += d*mLook
    DirectX::XMVECTOR s = DirectX::XMVectorReplicate(d);
    DirectX::XMVECTOR l = DirectX::XMLoadFloat3(&mLook);
    DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, DirectX::XMVectorMultiplyAdd(s, l, p));

	mViewDirty = true;
}

void D3DCamera::Ascend(float d)
{
    DirectX::XMVECTOR s = DirectX::XMVectorReplicate(d);
    DirectX::XMVECTOR u = DirectX::XMLoadFloat3(&mUp);
    DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&mPosition);
    XMStoreFloat3(&mPosition, DirectX::XMVectorMultiplyAdd(s, u, p));
    mViewDirty = true;
}

void D3DCamera::Descend(float d)
{
    Ascend(-d);
}

void D3DCamera::Pitch(float angle)
{
	// Rotate up and look vector about the right vector.
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);
	XMStoreFloat3(&mUp, DirectX::XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, DirectX::XMVector3TransformNormal(XMLoadFloat3(&mLook), R));
	mViewDirty = true;
}

void D3DCamera::Roll(float angle)
{
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&mLook), angle);
    XMStoreFloat3(&mUp, DirectX::XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
    XMStoreFloat3(&mRight, DirectX::XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
    mViewDirty = true;
}

void D3DCamera::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.

    DirectX::XMMATRIX R = DirectX::XMMatrixRotationY(angle);

	XMStoreFloat3(&mRight,   XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	mViewDirty = true;
}

void D3DCamera::UpdateViewMatrix()
{
	if(mViewDirty)
	{
        DirectX::XMVECTOR R = XMLoadFloat3(&mRight);
        DirectX::XMVECTOR U = XMLoadFloat3(&mUp);
        DirectX::XMVECTOR L = XMLoadFloat3(&mLook);
        DirectX::XMVECTOR P = XMLoadFloat3(&mPosition);

		// Keep camera's axes orthogonal to each other and of unit length.
		L = DirectX::XMVector3Normalize(L);
		U = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(L, R));

		// U, L already ortho-normal, so no need to normalize cross product.
		R = DirectX::XMVector3Cross(U, L);

		// Fill in the view matrix entries.
		float x = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, R));
		float y = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, U));
		float z = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, L));

		XMStoreFloat3(&mRight, R);
		XMStoreFloat3(&mUp, U);
		XMStoreFloat3(&mLook, L);

		mView(0, 0) = mRight.x;
		mView(1, 0) = mRight.y;
		mView(2, 0) = mRight.z;
		mView(3, 0) = x;

		mView(0, 1) = mUp.x;
		mView(1, 1) = mUp.y;
		mView(2, 1) = mUp.z;
		mView(3, 1) = y;

		mView(0, 2) = mLook.x;
		mView(1, 2) = mLook.y;
		mView(2, 2) = mLook.z;
		mView(3, 2) = z;

		mView(0, 3) = 0.0f;
		mView(1, 3) = 0.0f;
		mView(2, 3) = 0.0f;
		mView(3, 3) = 1.0f;

		mViewDirty = false;
	}
}
}




