#pragma once
#include "Vector4.h"
#include "Vector3.h"

class Light {
public:
	Light() {};
	Light(const Vector3& position, const Vector4& colour, float radius) {
		this->position = position;
		this->colour = colour;
		this->radius = radius;
	}

	~Light(void) {};

	Vector3 GetPosition()const { return position; }
	void SetPosition(const Vector3& val) { position = val; }

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

	Vector4 GetColour()const { return colour; }
	void SetColour(const Vector4& val) { colour = val; }

	void Update(float dt) { 
		position = Matrix4::Rotation(60*dt, Vector3(1, 0, 1))*position ;
		//position = Matrix4::Translation(Vector3(10, 0, 10)) * position;
		/*position.x = 1.0f + std::sin(dt) * 20.0f;
		position.z = std::cos(dt / 2.0f) * 1.0f;
		position.y = std::cos(dt / 2.0f) * 1.0f;*/
	}

	//lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
	//lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;


protected:
	Vector3 position;
	float radius;
	Vector4 colour;
};