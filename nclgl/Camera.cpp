#include "Camera.h"
#include "Window.h"
#include <algorithm>
#include "../nclgl/HeightMap.h"

float speed = 0;
//int mode = 0;
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
		speed = 200.0f * dt;
		//firstposition = GetPosition();
		position += auto_forward * speed;//here is  movement
		//SetPosition(Vector3(100,50,100));
		//position -= auto_right * speed;
		
	}
	else if (!cameramode) {
		//free camera

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
