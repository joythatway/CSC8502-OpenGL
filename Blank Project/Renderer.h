#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "..//nclgl/SceneNode.h"
#include "..//nclgl/CubeRobot.h"
#include "..//nclgl/teapot.h"

class Camera;
class Shader;
class HeightMap;
class Mesh;
class MeshAnimation;
class MeshMaterial;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window& parent);
	~Renderer(void);

	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	//cube Mapping
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	void DrawModel1();

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;

	HeightMap* heightMap;
	Mesh* quad;

	Light* light;
	Camera* camera;
	GLuint cubeMap;
	GLuint waterTex;
	GLuint earthTex;
	GLuint earthBump;
	GLuint waterBump;

	float waterRotate;
	float waterCycle;

	//shadow mapping
	void DrawShadowScene();
	void DrawMainScene();

	GLuint shadowTex;
	GLuint shadowFBO;

	GLuint sceneDiffuse;
	GLuint sceneBump;
	float sceneTime;

	Shader* sceneShader;
	Shader* shadowShader;

	vector<Mesh*> sceneMeshes;
	vector<Matrix4> sceneTransform;
	Light* light_shadow;

	//tu 9
	
	Mesh* model1mesh;
	Shader* model1shader;
	MeshAnimation* model1anim;
	MeshMaterial* model1material;
	vector<GLuint> model1matTextures;

	int currentFrame;
	float frameTime;


	//tu 6

	void DrawNode(SceneNode* n);

	SceneNode* root;
	Mesh* cube;
	Mesh* teapot;
	Shader* shaderforcube;

};
