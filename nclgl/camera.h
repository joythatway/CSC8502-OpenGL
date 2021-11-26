#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "HeightMap.h"

class Camera {
public:
	Camera(void) {
		yaw = 0.0f;
		pitch = 0.0f;
	};

	Camera(float pitch, float yaw, Vector3 position) {
		this->pitch = pitch;
		this->yaw = yaw;
		this->position = position;
	}

	~Camera(void) {};

	void UpdateCamera(float dt = 1.0f);

	void autoCamera(float pitch, float yaw, Vector3 position) {
		this->pitch = pitch;
		this->yaw = yaw;
		this->position = position;
	}// the temp func in 15/11/2021

	bool autocamera = false;
	bool speedup = false;
	//int mode = 0;
	HeightMap* camheightMap;
	bool cameramode = true;
	Vector3 firstposition;
	Vector3 secondposition;


	Matrix4 BuildViewMatrix();

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetYaw() const { return yaw; }
	void SetYaw(float y) { yaw = y; }

	float GetPitch()const { return pitch; }
	void SetPitch(float p) { pitch = p; }

protected:
	float yaw;
	float pitch;
	Vector3 position;
};