#pragma once
#include "../nclgl/SceneNode.h"

class Teapot :public SceneNode {
public:
	Teapot(Mesh* teapot);
	~Teapot(void) {};
	void Update(float dt) override;

protected:
	SceneNode* body;
	
};