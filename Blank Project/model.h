#pragma once
#include "../nclgl/SceneNode.h"

class model :public SceneNode {
public:
	model(Mesh* mod);
	~model(void) {};
	void Update(float dt) override;

protected:
	
};