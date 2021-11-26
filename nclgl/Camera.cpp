#include "Camera.h"
#include "Window.h"
#include <algorithm>
#include "../nclgl/HeightMap.h"

float speed = 0;
int mode = 6000;
float camtime = 0;
void Camera::UpdateCamera(float dt) {
	
	
	Matrix4 auto_rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	Vector3 auto_forward = auto_rotation * Vector3(0, 0, -1);
	Vector3 auto_right = auto_rotation * Vector3(1, 0, 0);

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_1)){
		//mode=0;//open auto camera;
		cameramode = true;
	}
	else if (Window::GetKeyboard()->KeyDown(KEYBOARD_2)){
		//mode=1;// open free camera;
		cameramode = false;
	}
	
	if (cameramode) {
		//autoCamera();//
		speed = 200.0f * dt;////////////////////////////
		/*
		//firstposition = GetPosition();
		//position += auto_forward * speed;//here is  movement/////////////////////////
		//SetPosition(Vector3(100,50,100));
		//position -= auto_right * speed;////////////////////////////////
		//yaw += -2*dt;////////////////////////////
		//SetPosition(hSize * Vector3(0.5, 0.5, 0.5));
		if (mode <= 6000 && mode>=4000) {
			mode -= dt;
			position += auto_forward * speed;
		}if (mode <= 5000 && mode >= 3000) {
			mode -= dt;
			position -= auto_right * speed;
			yaw += -3 * dt;
		}if (mode <= 3000 && mode >= 2000) {
			SetPosition(Vector3(100, 1050, 100));
			//pitch = -2*dt;
			yaw = GetYaw() + 2 * dt;

		}if (mode <= 0) {
			mode = 6000;
		}
		*/
		camtime = camtime + dt;
		pitch = std::min(pitch, 90.0f);
		pitch = std::max(pitch, -90.0f);
		if (camtime < 50) {
			position += auto_forward * speed;
		}if (camtime >= 50 && camtime < 70) {
			yaw -= 10.0f * dt;
		}if (camtime >= 70 && camtime < 90) {
			yaw += 10.0f * dt;
		}if (camtime >= 90 && camtime < 100) {
			SetPosition(Vector3(3800, 2500, 3800));
		}if (camtime >= 100 && camtime < 110) {
			position += auto_forward * speed*1.5;
		}if (camtime >= 110 && camtime < 160) {
			yaw -= 20.0f * dt;
			position.y -= speed*0.5f;
		}if (camtime >= 160 && camtime < 165) {
			position -= auto_forward * speed*5;
		}if (camtime >= 165 && camtime < 170) {
			position -= auto_forward * speed;
			pitch -=5.0f * dt;
		}if (camtime >= 170 && camtime < 175) {
			pitch += 5.0f * dt;
		}



	}
	else if (!cameramode) {
		//free camera
		mode = 6000;
		if (speedup) {
			speed = 900.0f * dt;
		}
		else if (!speedup) {
			speed = 200.0f * dt;
		}

		pitch -= (Window::GetMouse()->GetRelativePosition().y);
		yaw -= (Window::GetMouse()->GetRelativePosition().x);

		pitch = std::min(pitch, 90.0f);
		pitch = std::max(pitch, -90.0f);

		if (yaw < 0) {
			yaw += 360.0f;
		}
		if (yaw > 360.0f) {
			yaw -= 360.0f;
		}

		Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
		Vector3 forward = rotation * Vector3(0, 0, -1);
		Vector3 right = rotation * Vector3(1, 0, 0);

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
			position += forward * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
			position -= forward * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
			position -= right * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
			position += right * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
			position.y += speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
			position.y -= speed;
		}
		if (Window::GetKeyboard()->KeyHeld(KEYBOARD_Q)) {
			speedup = true;
		}
		else {
			speedup = false;
		}
	}

}
Matrix4 Camera::BuildViewMatrix() {
		return	Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
				Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
				Matrix4::Translation(-position);
	}
