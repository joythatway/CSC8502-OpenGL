#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Shader.h"
#include "../nclgl/camera.h"

#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"

#include "model1.h"

#define SHADOWSIZE 2048

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	cube = Mesh::LoadFromMeshFile("OffsetCubeY.msh");//tu6
	teapot = Mesh::LoadFromMeshFile("Teapot001.msh");

	quad = Mesh::GenertateQuad();
	//heightMap = new HeightMap(TEXTUREDIR"terrain03.png");
	heightMap = new HeightMap(TEXTUREDIR"noise3pppp.png");
	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	//earthTex = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthTex = SOIL_load_OGL_texture(TEXTUREDIR"terrain02.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthBump = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	
	//earthBump = SOIL_load_OGL_texture(TEXTUREDIR"NormalMapfire.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterBump= SOIL_load_OGL_texture(TEXTUREDIR"waterbump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	/*
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	*/
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"sright.jpg", TEXTUREDIR"sleft.jpg",
		TEXTUREDIR"stop.jpg", TEXTUREDIR"sbottom.jpg",
		TEXTUREDIR"sfront.jpg", TEXTUREDIR"sback.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if (!earthTex) {
		return;
	}
	if (!earthBump) {
		return;
	}
	if (!cubeMap) {
		return;
	}
	if (!waterTex) {
		return;
	}
	if (!waterBump) {
		return;
	}

	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(earthBump, true);
	SetTextureRepeating(waterTex, true);
	SetTextureRepeating(waterBump, true);

	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	//lightShader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");
	lightShader = new Shader("bumpvertex.glsl", "bumpfragment.glsl");
	model1shader = new  Shader("SkinningVertex.glsl", "texturedFragment.glsl");
	shaderforcube = new  Shader("SceneVertex.glsl", "SceneFragment.glsl");//tu6
	if (!shaderforcube) {
		return;
	}
	if (!reflectShader->LoadSuccess()) {
		return;
	}
	if (!skyboxShader->LoadSuccess()) {
		return;
	}
	if (!lightShader->LoadSuccess()) {
		return;
	}
	if (!model1shader->LoadSuccess()) {
		return;
	}
	model1mesh = Mesh::LoadFromMeshFile("Role_T.msh");
	model1anim = new  MeshAnimation("Role_T.anm");
	model1material = new  MeshMaterial("Role_T.mat");
	for (int i = 0; i < model1mesh->GetSubMeshCount(); ++i) {
		const  MeshMaterialEntry* matEntry = model1material->GetMaterialForLayer(i);

		const  string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string  path = TEXTUREDIR + *filename;
		GLuint  texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		model1matTextures.emplace_back(texID);
	}
	currentFrame = 0;
	frameTime = 0.0f;

	Vector3 heightmapSize = heightMap->GetHeightmapSize();

	//camera = new Camera(-45.0f, 0.0f, heightmapSize * Vector3(0.5f, 5.0f, 0.5f));
	camera = new Camera(-15.0f, -135.0f, heightmapSize * Vector3(0.0f, 5.0f, 0.0f));




	//light = new Light(heightmapSize * Vector3(0.5f, 10.5f, 0.5f), Vector4(1, 1, 1, 1), 1.5f*heightmapSize.x);
	light = new Light(heightmapSize * Vector3(0.5f, 3.5f, 0.5f), Vector4(1, 1, 1, 1), 1.5f*heightmapSize.x);//1.5f->1.0f
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);



	root = new  SceneNode();//tu6
	root->AddChild(new  CubeRobot(cube));//tu6
	root->AddChild(new Teapot(teapot));
	root->AddChild(new CubeRobot(model1mesh));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	waterRotate = 0.0f;
	waterCycle = 0.0f;
	init = true;
}
Renderer::~Renderer(void)	{
	delete camera;
	delete heightMap;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete light;


	
	delete model1mesh;
	delete model1anim;
	delete model1material;
	delete model1shader;

	//tu6 begin
	delete  root;
	delete  shaderforcube;
	delete  cube;
	delete teapot;
	//tu6 end
	/*
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);

	for (auto& i : sceneMeshes) {
		delete i;
	}
	delete sceneShader;
	delete shadowShader;
	*/
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	light->Update(dt);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += dt * 2.0f;
	waterCycle += dt * 0.25f;
	//sceneTime += dt;
	//for (int i = 1; i < 4; ++i) {
	//	Vector3 t = Vector3(-10 + (5 * i), 2.0f + sin(sceneTime * i), 0);
	//	sceneTransform[i] = Matrix4::Translation(t) * Matrix4::Rotation(sceneTime * 10 * i, Vector3(1, 0, 0));
	//}
	root->Update(dt);//tu6

	frameTime -= dt;
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % model1anim->GetFrameCount();
		frameTime += 1.0f / model1anim->GetFrameRate();
	}

}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawSkybox();
	DrawHeightmap();
	DrawWater();
	DrawModel1();

	//tu6 begin
	BindShader(shaderforcube);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(shaderforcube->GetProgram(), "diffuseTex"), 1);
	DrawNode(root);
	//tu6 end

	//DrawShadowScene();
	//DrawMainScene();
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();

	glDepthMask(GL_TRUE);

}

void Renderer::DrawHeightmap() {
	BindShader(lightShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();
}

void Renderer::DrawWater() {
	BindShader(reflectShader);
	
	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	//glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "bumpTex"), 1);
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, waterBump);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);//
	

	Vector3 hSize = heightMap->GetHeightmapSize();
	hSize = hSize * Vector3(1, 1, 1);//control water level by change the y values
	//hSize = hSize - Vector3(0, 10, 0);//control water level by change the y values
	modelMatrix = Matrix4::Translation(hSize * 0.5f) * Matrix4::Scale(hSize * 0.5f) * Matrix4::Rotation(90, Vector3(1, 0, 0));

	textureMatrix = Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) * Matrix4::Scale(Vector3(10, 10, 10)) * Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

	UpdateShaderMatrices();
	quad->Draw();

}

void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	BindShader(shadowShader);

	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
	projMatrix = Matrix4::Perspective(1, 100, 1, 45);
	shadowMatrix = projMatrix * viewMatrix;

	for (int i = 0; i < 4; ++i) {
		modelMatrix = sceneTransform[i];
		UpdateShaderMatrices();
		sceneMeshes[i]->Draw();
	}


	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawMainScene() {
	BindShader(sceneShader);
	SetShaderLight(*light);
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "shadowTex"), 2);

	glUniform3fv(glGetUniformLocation(sceneShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneDiffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sceneBump);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	for (int i = 0; i < 4; ++i) {
		modelMatrix = sceneTransform[i];
		UpdateShaderMatrices();
		sceneMeshes[i]->Draw();
	}

}

void Renderer::DrawModel1() {
	BindShader(model1shader);
	glUniform1i(glGetUniformLocation(model1shader->GetProgram(), "diffuseTex"), 0);

	UpdateShaderMatrices();

	vector <Matrix4 > frameMatrices;

	const  Matrix4* invBindPose = model1mesh->GetInverseBindPose();
	const  Matrix4* frameData = model1anim->GetJointData(currentFrame);

	for (unsigned int i = 0; i < model1mesh->GetJointCount(); ++i) {
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}


	int j = glGetUniformLocation(model1shader->GetProgram(), "joints");
	glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());

	for (int i = 0; i < model1mesh->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, model1matTextures[i]);
		model1mesh->DrawSubMesh(i);
	}
}

void   Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		Matrix4  model = n->GetWorldTransform() *
			Matrix4::Scale(n->GetModelScale());

		glUniformMatrix4fv(glGetUniformLocation(shaderforcube->GetProgram(), "modelMatrix"), 1, false, model.values);

		glUniform4fv(glGetUniformLocation(shaderforcube->GetProgram(), "nodeColour"), 1, (float*)& n->GetColour());

		//glUniform1i(glGetUniformLocation(shader->GetProgram(),"useTexture"),0)); 
		glUniform1i(glGetUniformLocation(shaderforcube->GetProgram(), "useTexture"), 0);

		n->Draw(*this);
	}

	for (vector <SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}
}