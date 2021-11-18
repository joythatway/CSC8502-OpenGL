#include "teapot.h"

Teapot::Teapot(Mesh* teapot) {

	SceneNode* body = new SceneNode(teapot, Vector4(1, 0, 0, 1));
	body->SetModelScale(Vector3(10, 15, 5));//!!
	body->SetTransform(Matrix4::Translation(Vector3(0, 10, 0)));//!!
	AddChild(body);

}

void Teapot::Update(float dt) {
	transform = transform;//change 30.0f to 0.0f

	//body->SetTransform(transform);

	

	SceneNode::Update(dt);
}