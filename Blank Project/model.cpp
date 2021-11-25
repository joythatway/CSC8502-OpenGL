#include "model.h"

model::model(Mesh* mod) {


	SceneNode* body = new SceneNode(mod, Vector4(1, 0, 0, 1));
	body->SetModelScale(Vector3(100, 100, 100));
	body->SetTransform(Matrix4::Translation(Vector3(100, 500, 100)));//!!position according the map
	AddChild(body);

	
}
void model::Update(float dt) {
	transform = transform * Matrix4::Rotation(0.0f * dt, Vector3(0, 1, 0));//change 30.0f to 0.0f
	
	SceneNode::Update(dt);
}