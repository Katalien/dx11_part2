#pragma once
#include "Dev.h"

class Camera
{
private:
	float fov = 60.0f;
	size_t height = 1;
	size_t width = 1;
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;

	DirectX::XMMATRIX projection;
	bool projectionDirty = true;

	DirectX::XMFLOAT3 position = { 0, 0, 0 };
	DirectX::XMFLOAT3 direction = { 0, 0, 1 };
	DirectX::XMFLOAT3 up = { 0, 1, 0 };

	DirectX::XMMATRIX view;
	bool viewDirty = true;

	DirectX::XMFLOAT4X4 viewProj;

	float MaxUpAngle = 89.0f;
	float currUpAngle = 0.0f;

public:
	Camera() {

	}

	void setPosition(DirectX::XMFLOAT3 pos) {
		position = pos;
		viewDirty = true;
	}

	void reset() {
		fov = 90.0f;
		nearPlane = 0.1f;
		farPlane = 1000.0f;

		projection = {};
		projectionDirty = true;

		position = { 0, 0, 0 };
		direction = { 0, 0, 1 };
		up = { 0, 1, 0 };

		view = {};
		viewDirty = true;

		viewProj = {};
		currUpAngle = 0.0f;
	}

	void translate(DirectX::XMFLOAT3 pos) {
		position.x += pos.x;
		position.y += pos.y;
		position.z += pos.z;

		viewDirty = true;
	}

	void rotateAroundUp(float angleInDegrees) {
		DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&up), angleInDegrees * DirectX::XM_PI / 180.0f);
		DirectX::XMVECTOR newDir = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&direction), rotation);
		DirectX::XMStoreFloat3(&direction, newDir);
		viewDirty = true;
	}

	void rotateAroundRight(float angleInDegrees) {
		if (currUpAngle + angleInDegrees < -MaxUpAngle) {
			angleInDegrees = -MaxUpAngle - currUpAngle;
		}
		else if (currUpAngle + angleInDegrees > MaxUpAngle) {
			angleInDegrees = MaxUpAngle - currUpAngle;
		}
		currUpAngle += angleInDegrees;

		DirectX::XMVECTOR right = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&up), DirectX::XMLoadFloat3(&direction));
		DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationAxis(right, angleInDegrees * DirectX::XM_PI / 180.0f);
		DirectX::XMVECTOR newDir = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&direction), rotation);
		DirectX::XMStoreFloat3(&direction, newDir);
		viewDirty = true;
	}

	void setDirection(DirectX::XMFLOAT3 direct) {
		direction = direct;
		viewDirty = true;
	}

	void setFov(float newFov) {
		fov = newFov;
		projectionDirty = true;
	}

	void setNear(float newNear) {
		nearPlane = newNear;
		projectionDirty = true;
	}

	void setFar(float newFar) {
		farPlane = newFar;
		projectionDirty = true;
	}

	void resize(size_t w, size_t h) {
		width = w;
		height = h;
		projectionDirty = true;
	}

	DirectX::XMFLOAT4X4 getVP() {
		if (!viewDirty && !projectionDirty) {
			return viewProj;
		}

		if (viewDirty) {
			view = DirectX::XMMatrixLookAtLH(
				DirectX::XMLoadFloat3(&position),
				DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position), DirectX::XMLoadFloat3(&direction)),
				DirectX::XMLoadFloat3(&up));
			viewDirty = false;
		}

		if (projectionDirty) {
			projection = DirectX::XMMatrixPerspectiveFovLH(
				fov * DirectX::XM_PI / 180, 1.0 * width / height, nearPlane, farPlane);
			projectionDirty = false;
		}

		DirectX::XMStoreFloat4x4(&viewProj, DirectX::XMMatrixMultiply(view, projection));
		
		return viewProj;
	}

	DirectX::XMFLOAT3 getPosition() { return position; }
	DirectX::XMFLOAT3 getDirection() { return direction; }

};
