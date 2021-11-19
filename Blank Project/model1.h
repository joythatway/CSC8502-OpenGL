#pragma once
#include "../nclgl/OGLRenderer.h"

class Camera;
class Mesh;
class MeshAnimation;
class MeshMaterial;

class Model1 :public OGLRenderer {
public:
	Model1();
	~Model1(void);


protected:
	
	Mesh* mesh;
	Shader* shader;
	MeshAnimation* anim;
	MeshMaterial* material;
	vector<GLuint> matTextures;

	int currentFrame;
	float frameTime;
};