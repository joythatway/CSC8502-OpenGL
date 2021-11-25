#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "..//nclgl/SceneNode.h"
#include "..//nclgl/CubeRobot.h"
#include "..//nclgl/teapot.h"
#include "ParticleEmitter.h"

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

	void loadshader();
	void loadtexture();
	//void loadpostprocessing();
	void loadMutiLight();
	void loadmodel();
	void drawtree();
	void updatetree(float dt);
	bool changecamera = false;
	bool deferredrendering = true;

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
	Mesh* post_quad;

	Light* light;
	Camera* camera;
	Camera* camera01;
	Camera* camera02;
	Camera* camera03;

	GLuint cubeMap;
	GLuint waterTex;
	GLuint earthTex;
	GLuint earthBump;
	GLuint waterBump;
	GLuint sphereTex;
	GLuint treeTex;

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
	Mesh* cube2;
	Mesh* teapot;
	Shader* shaderforcube;


	//tu 10 post processing
	
	//void PresentScene();
	//void DrawPostProcess();
	//void DrawScene();

	//Shader* post_sceneShader;
	//Shader* post_processShader;
	//GLuint bufferFBO;
	//GLuint processFBO;
	//GLuint bufferColourTex[2];
	//GLuint bufferDepthTex;
	
	//tu 15 begin
	void FillBuffers();
	void DrawPointLights();
	void CombineBuffers();
	void GenerateScreenTexture(GLuint& into, bool depth = false);
	//Shader* sceneShader;
	Shader* pointlightShader;
	Shader* combineShader;
	GLuint bufferFBO;
	GLuint bufferColourTex;
	GLuint bufferNormalTex;
	GLuint bufferDepthTex;
	GLuint pointLightFBO;
	GLuint lightDiffuseTex;
	GLuint lightSpecularTex;
	Light* pointLights;
	Mesh* sphere;
	//tu 15 end

	SceneNode* model_teapot;
	MeshAnimation* model_teapot_anim;
	MeshMaterial* model_teapot_material;
	vector<GLuint> model_teapot_texture;

	Mesh* soldier_mesh;
	Shader* soldier_shader;
	Shader* modelshader;
	MeshAnimation* soldier_anim;
	MeshMaterial* soldier_material;
	vector<GLuint> soldier_matTextures;
	SceneNode* model_soldier;

	SceneNode* mod_tea;
	Mesh* tea;
	MeshMaterial* tea_mat;
	Mesh* tree;
	Shader* treeshader;

	SceneNode* mod_tree;
	Mesh* tree1;
	SceneNode* mod_tree2;
	SceneNode* mod_tree3;
	SceneNode* mod_tree4;
	SceneNode* mod_tree5;

	float wavetime;
	float wavespeed;


	//particle
	public:
		Shader* particleShader;
		void drawparticle();
	protected:
		void SetShaderParticleSize(float f);
		ParticleEmitter* emitter;

	//particle
};
