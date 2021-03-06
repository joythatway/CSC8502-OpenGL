#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Shader.h"
#include "../nclgl/camera.h"

#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"

#include "model1.h"
#include "model.h"

#define SHADOWSIZE 2048
const int LIGHT_NUM = 32;
//const int POST_PASSES = 10;//post processing
float treetime = 0;
int treesecond = 0;
Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{

	cube = Mesh::LoadFromMeshFile("OffsetCubeY.msh");//tu6
	cube2 = Mesh::LoadFromMeshFile("OffsetCubeY.msh");
	teapot = Mesh::LoadFromMeshFile("Teapot001.msh");
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");

	wavespeed = 0.005;
	wavetime = 0;

	quad = Mesh::GenertateQuad();
	post_quad = Mesh::GenertateQuad();
	
	emitter = new ParticleEmitter();

	loadtexture();

	loadshader();
	loadMutiLight();
	//draw tree begin
	//draw tree end
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
	camera01 = new Camera(-15.0f, 135.0f, heightmapSize * Vector3(1.0f, 6.0f, 0.0f));
	camera02 = new Camera(-15.0f, -45.0f, heightmapSize * Vector3(0.0f, 7.0f, 1.0f));
	camera03 = new Camera(-15.0f, 45.0f, heightmapSize * Vector3(1.0f, 8.0f, 1.0f));




	//light = new Light(heightmapSize * Vector3(0.5f, 10.5f, 0.5f), Vector4(1, 1, 1, 1), 1.5f*heightmapSize.x);
	light = new Light(heightmapSize * Vector3(0.5f, 20.0f, 0.5f), Vector4(1, 1, 1, 1), 2.5f*heightmapSize.x);//1.5f->1.0f
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);



	root = new  SceneNode();//tu6
	root->AddChild(new  CubeRobot(cube));//tu6
	//root->AddChild(new CubeRobot(cube2));//tu6
	root->AddChild(new model(cube2));
	loadmodel();
	//root->AddChild(new CubeRobot(model1mesh));

	//loadpostprocessing();

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
	delete camera01;
	delete camera02;
	delete camera03;

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
	delete  cube2;
	delete teapot;
	//tu6 end
	
	//post processing begin
	//delete post_sceneShader;
	//delete post_processShader;
	//glDeleteTextures(2, bufferColourTex);
	//glDeleteTextures(1, &bufferDepthTex);
	//glDeleteFramebuffers(1, &bufferFBO);
	//glDeleteFramebuffers(1, &processFBO);
	//post processing end
	//tu 15 begin
	delete sphere;
	delete[] pointLights;
	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	//tu 15 wnd
	
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
void Renderer::loadmodel() {

	sphereTex = SOIL_load_OGL_texture(TEXTUREDIR"tree1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (!sphereTex)return;

	model_teapot = new SceneNode();
	tree1 = Mesh::LoadFromMeshFile("tree1.msh");
	model_teapot->SetMesh(tree1);
	model_teapot->SetColour(Vector4(0,0.6,0,1));
	model_teapot->SetModelScale(Vector3(100, 100, 100));
	model_teapot->SetTransform(Matrix4::Translation(Vector3(4200, 250, 3600)));
	model_teapot->SetTexture(sphereTex);
	//SetTextureRepeating(sphereTex, true);
	root->AddChild(model_teapot);
	//model:sphere end

	mod_tree = new SceneNode();
	tea = Mesh::LoadFromMeshFile("tree1.msh");
	//tea_mat = new MeshMaterial("tree01.mat");
	mod_tree->SetMesh(tea);
	mod_tree->SetModelScale(Vector3(100, 100, 100));
	mod_tree->SetTransform(Matrix4::Translation(Vector3(3650, 250, 4900)));
	mod_tree->SetTransform(mod_tree->GetTransform() * Matrix4::Rotation(-8.0f * 1, Vector3(1, 0, 0)));
	mod_tree->SetTexture(sphereTex);
	root->AddChild(mod_tree);

	mod_tree2 = new SceneNode();
	tea = Mesh::LoadFromMeshFile("tree1.msh");
	//tea_mat = new MeshMaterial("tree01.mat");
	mod_tree2->SetMesh(tea);
	mod_tree2->SetModelScale(Vector3(100, 100, 100));
	mod_tree2->SetTransform(Matrix4::Translation(Vector3(5250, 250, 2400)));
	mod_tree2->SetTransform(mod_tree2->GetTransform()*Matrix4::Rotation(-8.0f*1,Vector3(1,0,0)));
	mod_tree2->SetTexture(sphereTex);
	root->AddChild(mod_tree2);

	mod_tree3 = new SceneNode();
	tea = Mesh::LoadFromMeshFile("tree1.msh");
	//tea_mat = new MeshMaterial("tree01.mat");
	mod_tree3->SetMesh(tea);
	mod_tree3->SetModelScale(Vector3(100, 100, 100));
	mod_tree3->SetTransform(Matrix4::Translation(Vector3(5450, 250, 4300)));
	mod_tree3->SetTransform(mod_tree3->GetTransform() * Matrix4::Rotation(-8.0f * 1, Vector3(1, 0, 0)));
	mod_tree3->SetTexture(sphereTex);
	root->AddChild(mod_tree3);

	mod_tree4 = new SceneNode();
	tea = Mesh::LoadFromMeshFile("tree1.msh");
	//tea_mat = new MeshMaterial("tree01.mat");
	mod_tree4->SetMesh(tea);
	mod_tree4->SetModelScale(Vector3(100, 100, 100));
	mod_tree4->SetTransform(Matrix4::Translation(Vector3(3550, 250, 3200)));
	mod_tree4->SetTransform(mod_tree4->GetTransform() * Matrix4::Rotation(-8.0f * 1, Vector3(1, 0, 0)));
	mod_tree4->SetTexture(sphereTex);
	root->AddChild(mod_tree4);

	mod_tree5 = new SceneNode();
	tea = Mesh::LoadFromMeshFile("tree1.msh");
	//tea_mat = new MeshMaterial("tree01.mat");
	mod_tree5->SetMesh(tea);
	mod_tree5->SetModelScale(Vector3(100, 100, 100));
	mod_tree5->SetTransform(Matrix4::Translation(Vector3(4050, 250, 2600)));
	mod_tree5->SetTransform(mod_tree5->GetTransform() * Matrix4::Rotation(-8.0f * 1, Vector3(1, 0, 0)));
	mod_tree5->SetTexture(sphereTex);
	root->AddChild(mod_tree5);


	//teapot begin
	mod_tea = new SceneNode();
	tea = Mesh::LoadFromMeshFile("tree1.msh");
	//tea_mat = new MeshMaterial("tree01.mat");
	mod_tea->SetMesh(tea);
	mod_tea->SetModelScale(Vector3(100, 100, 100));
	mod_tea->SetTransform(Matrix4::Translation(Vector3(3850, 250, 4500)));
	mod_tea->SetTransform(mod_tea->GetTransform() * Matrix4::Rotation(-8.0f * 1, Vector3(1, 0, 0)));
	mod_tea->SetTexture(sphereTex);
	root->AddChild(mod_tea);
	//teapot end

	//soldier begin
	soldier_mesh = Mesh::LoadFromMeshFile("Role_T.msh");
	soldier_anim = new  MeshAnimation("Role_T.anm");
	soldier_material = new  MeshMaterial("Role_T.mat");
	soldier_shader = new  Shader("SkinningVertex.glsl", "texturedFragment.glsl");
	if (!soldier_shader->LoadSuccess()) {
		return;
	}
	model_soldier = new SceneNode(soldier_mesh);
	model_soldier->SetModelScale(Vector3(100, 100, 100));
	model_soldier->SetTransform(Matrix4::Translation(Vector3(3900, 300, 4000)));
	model_soldier->SetTexture(earthTex);
	root->AddChild(model_soldier);

	for (int i = 0; i < soldier_mesh->GetSubMeshCount(); ++i) {
		const  MeshMaterialEntry* matEntry = soldier_material->GetMaterialForLayer(i);

		const  string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string  path = TEXTUREDIR + *filename;
		GLuint  texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		soldier_matTextures.emplace_back(texID);
	}
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(soldier_shader);
	glUniform1i(glGetUniformLocation(soldier_shader->GetProgram(), "diffuseTex"), 0);

	UpdateShaderMatrices();

	vector <Matrix4 > frameMatrices;

	const  Matrix4* invBindPose = soldier_mesh->GetInverseBindPose();
	const  Matrix4* frameData = soldier_anim->GetJointData(currentFrame);

	for (unsigned int i = 0; i < soldier_mesh->GetJointCount(); ++i) {
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}


	int j = glGetUniformLocation(soldier_shader->GetProgram(), "joints");
	glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());

	for (int i = 0; i < soldier_mesh->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, soldier_matTextures[i]);
		soldier_mesh->DrawSubMesh(i);
	}

	//soldier end
}
void Renderer::loadshader() {

	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	//lightShader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");
	lightShader = new Shader("bumpvertex.glsl", "bumpfragment.glsl");
	model1shader = new  Shader("SkinningVertex.glsl", "TexturedFragment.glsl");
	shaderforcube = new  Shader("SceneVertex.glsl", "SceneFragment.glsl");//tu6
	modelshader = new Shader("modelshaderVertex.glsl", "modelshaderFrag.glsl");
	treeshader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");
	particleShader = new Shader("particleVertex.glsl","particleFragment.glsl","particleGeometry.glsl");

	//post_sceneShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");//tu 10
	//post_processShader = new Shader("TexturedVertex.glsl", "processfrag.glsl");//tu 10
	//if (!post_sceneShader) {
	//	return;
	//}
	//if (!post_processShader) {
	//	return;
	//}
	if (!modelshader) { return; }
	if (!shaderforcube) {return;}
	if (!reflectShader->LoadSuccess()) {return;}
	if (!skyboxShader->LoadSuccess()) {return;}
	if (!lightShader->LoadSuccess()) {return;}
	if (!model1shader->LoadSuccess()) {return;}
}

void Renderer::loadtexture() {

	//heightMap = new HeightMap(TEXTUREDIR"noise3pppp.png");
	heightMap = new HeightMap(TEXTUREDIR"terrain001.png");
	earthTex = SOIL_load_OGL_texture(TEXTUREDIR"terrainTex001.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthBump = SOIL_load_OGL_texture(TEXTUREDIR"terrainbump002.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	//earthTex = SOIL_load_OGL_texture(TEXTUREDIR"terrain02.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	//earthBump = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterBump = SOIL_load_OGL_texture(TEXTUREDIR"waterbump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	/*
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	*/
	//
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"right.jpg", TEXTUREDIR"left.jpg",
		TEXTUREDIR"top.jpg", TEXTUREDIR"bottom.jpg",
		TEXTUREDIR"front.jpg", TEXTUREDIR"back.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	//
	/*
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"sright.jpg", TEXTUREDIR"sleft.jpg",
		TEXTUREDIR"stop.jpg", TEXTUREDIR"sbottom.jpg",
		TEXTUREDIR"sfront.jpg", TEXTUREDIR"sback.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	*/
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
}
/*
void Renderer::loadpostprocessing() {
	//post processing begin
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	}

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &processFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0]) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//post processing end

}*/
void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	camera01->UpdateCamera(dt);
	camera02->UpdateCamera(dt);
	camera03->UpdateCamera(dt);
	updatetree(dt);
	light->Update(dt);

	emitter->Update(1000 * dt);

	viewMatrix = camera->BuildViewMatrix();
	waterRotate += dt * 2.0f;
	waterCycle += dt * 1.25f;//0.25
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
		//
		currentFrame = (currentFrame + 1) % soldier_anim->GetFrameCount();
		frameTime += 1.0f / soldier_anim->GetFrameRate();
	}

}

void Renderer::RenderScene() {
	if (!changecamera) {
		if (deferredrendering) {
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			glViewport(0, 0, width, height);
			viewMatrix = camera->BuildViewMatrix();
			DrawSkybox();
			DrawHeightmap();
			DrawWater();
			DrawModel1();

			//drawparticle();

			//drawtree();
			//deferred rendering===========================================================================================
			//FillBuffers();
			//DrawPointLights();
			//CombineBuffers();
			//deferred rendering===========================================================================================
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_3)) {
				//DrawScene();
				//DrawPostProcess();
				//PresentScene();
			}
			projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
			viewMatrix = camera->BuildViewMatrix();
			//tu6 begin
			BindShader(shaderforcube);
			UpdateShaderMatrices();
			glUniform1i(glGetUniformLocation(shaderforcube->GetProgram(), "diffuseTex"), 1);
			DrawNode(root);
			//DrawNode(model_soldier);
			//tu6 end
			//DrawShadowScene();
			//DrawMainScene();
		}
		if (!deferredrendering) {
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			glViewport(0, 0, width, height);
			viewMatrix = camera->BuildViewMatrix();
			DrawSkybox();
			DrawHeightmap();
			DrawWater();
			DrawModel1();
			//drawtree();
			//deferred rendering===========================================================================================
			FillBuffers();
			DrawPointLights();
			CombineBuffers();
			//deferred rendering===========================================================================================
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_3)) {
				//DrawScene();
				//DrawPostProcess();
				//PresentScene();
			}
			projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
			viewMatrix = camera->BuildViewMatrix();
			//tu6 begin
			BindShader(shaderforcube);
			UpdateShaderMatrices();
			glUniform1i(glGetUniformLocation(shaderforcube->GetProgram(), "diffuseTex"), 1);
			DrawNode(root);
			//DrawNode(model_soldier);
			//tu6 end
			//DrawShadowScene();
			//DrawMainScene();
		}
		/*
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, width, height);
		viewMatrix = camera->BuildViewMatrix();
		DrawSkybox();
		DrawHeightmap();
		DrawWater();
		DrawModel1();
		//drawtree();
		//deferred rendering===========================================================================================
		//FillBuffers();
		//DrawPointLights();
		//CombineBuffers();
		//deferred rendering===========================================================================================
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_3)) {
		//DrawScene();
		//DrawPostProcess();
		//PresentScene();
		}
		projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
		viewMatrix = camera->BuildViewMatrix();
		//tu6 begin
		BindShader(shaderforcube);
		UpdateShaderMatrices();
		glUniform1i(glGetUniformLocation(shaderforcube->GetProgram(), "diffuseTex"), 1);
		DrawNode(root);
		//DrawNode(model_soldier);
		//tu6 end
		//DrawShadowScene();
		//DrawMainScene();
		*/
	}
	

	if (changecamera) {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		//view position 1------------------------------------------------------------------------
		glViewport(width / 2, 0, width / 2, height / 2);
		viewMatrix = camera->BuildViewMatrix();
		DrawSkybox();
		DrawHeightmap();
		DrawWater();
		DrawModel1();
		//drawtree();
		//FillBuffers();
		DrawPointLights();
		//CombineBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_3)) {
			//DrawScene();
			//DrawPostProcess();
			//PresentScene();
		}
		//projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
		
		//tu6 begin
		BindShader(shaderforcube);
		UpdateShaderMatrices();
		glUniform1i(glGetUniformLocation(shaderforcube->GetProgram(), "diffuseTex"), 1);
		DrawNode(root);
		//DrawNode(model_soldier);
		//tu6 end
		//DrawShadowScene();
		//DrawMainScene();

		//view position 2------------------------------------------------------------------------
		glViewport(0, 0, width / 2, height / 2);
		viewMatrix = camera01->BuildViewMatrix();
		DrawSkybox();
		DrawHeightmap();
		DrawWater();
		DrawModel1();
		//drawtree();
		//FillBuffers();
		DrawPointLights();
		//CombineBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_3)) {
			//DrawScene();
			//DrawPostProcess();
			//PresentScene();
		}
		
		//tu6 begin
		BindShader(shaderforcube);
		UpdateShaderMatrices();
		glUniform1i(glGetUniformLocation(shaderforcube->GetProgram(), "diffuseTex"), 1);
		DrawNode(root);
		//DrawNode(model_soldier);
		//tu6 end
		//DrawShadowScene();
		//DrawMainScene();

		//view position 3------------------------------------------------------------------------
		glViewport(0, height/2, width / 2, height / 2);
		viewMatrix = camera02->BuildViewMatrix();
		DrawSkybox();
		DrawHeightmap();
		DrawWater();
		DrawModel1();
		//drawtree();
		//FillBuffers();
		DrawPointLights();
		//CombineBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_3)) {
			//DrawScene();
			//DrawPostProcess();
			//PresentScene();
		}
		//tu6 begin
		BindShader(shaderforcube);
		UpdateShaderMatrices();
		glUniform1i(glGetUniformLocation(shaderforcube->GetProgram(), "diffuseTex"), 1);
		DrawNode(root);
		//DrawNode(model_soldier);
		//tu6 end
		//DrawShadowScene();
		//DrawMainScene();

		//view position 4------------------------------------------------------------------------
		glViewport(width / 2, height/2, width / 2, height / 2);
		viewMatrix = camera03->BuildViewMatrix();
		DrawSkybox();
		DrawHeightmap();
		DrawWater();
		DrawModel1();
		//drawtree();
		//FillBuffers();
		DrawPointLights();
		//CombineBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_3)) {
			//DrawScene();
			//DrawPostProcess();
			//PresentScene();
		}
		//tu6 begin
		BindShader(shaderforcube);
		UpdateShaderMatrices();
		glUniform1i(glGetUniformLocation(shaderforcube->GetProgram(), "diffuseTex"), 1);
		DrawNode(root);
		//DrawNode(model_soldier);
		//tu6 end
		//DrawShadowScene();
		//DrawMainScene();
	}


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
	
	//make water has wave 
	glUniform1f(glGetUniformLocation(reflectShader->GetProgram(), "time"), wavetime);
	//make water has wave 

	Vector3 hSize = heightMap->GetHeightmapSize();
	hSize = hSize * Vector3(1, 1, 1);//control water level by change the y values
	//hSize = hSize - Vector3(0, 10, 0);//control water level by change the y values
	modelMatrix = Matrix4::Translation(Vector3(hSize.x*0.5f,hSize.y*0.7f,hSize.z*0.5f)) * Matrix4::Scale(hSize * 0.5f) * Matrix4::Rotation(90, Vector3(1, 0, 0));

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
	//
	
	//glUniform3fv(glGetUniformLocation(model1shader->GetProgram(), "position"), Vector3(100, 100, 100));
	Vector3 hSize = heightMap->GetHeightmapSize();//change the soldiers
	hSize = hSize * Vector3(1, 26, 1);// control water level by change the y values
	modelMatrix = Matrix4::Translation((hSize * 0.5f)-Vector3(-300,3100,-300)) * Matrix4::Scale(hSize * 0.06f) * Matrix4::Rotation(45, Vector3(0, 1, 0));//change!!!

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
		BindShader(shaderforcube);
		glUniform1i(glGetUniformLocation(shaderforcube->GetProgram(), "diffuseTex"), 0);

		Matrix4  model = n->GetWorldTransform() *
			Matrix4::Scale(n->GetModelScale());

		glUniformMatrix4fv(glGetUniformLocation(shaderforcube->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(shaderforcube->GetProgram(), "nodeColour"), 1, (float*)& n->GetColour());
		//glUniform1i(glGetUniformLocation(shaderforcube->GetProgram(),"useTexture"),1)); 
		glUniform1i(glGetUniformLocation(shaderforcube->GetProgram(), "useTexture"), 1);

		//begin
		//glUniform1i(glGetUniformLocation(shaderforcube->GetProgram(), "diffuseTex"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sphereTex);
		//end

		n->Draw(*this);
	}

	for (vector <SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}
}
void   Renderer::BuildNodeLists(SceneNode* from) {

	if (frameFrustum.InsideFrustum(*from)) {
		Vector3  dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));
		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}
	for (vector <SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}

void   Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);

}
//post processing begin
/*
void Renderer::DrawScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	BindShader(post_sceneShader);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(post_sceneShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);
	heightMap->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(post_processShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(post_processShader->GetProgram(), "sceneTex"), 0);
	for (int i = 0; i < POST_PASSES; ++i) {

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(post_processShader->GetProgram(), "isVertical"), 0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
		post_quad->Draw();

		glUniform1i(glGetUniformLocation(post_processShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
		post_quad->Draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(sceneShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
	quad->Draw();
}
*/
//post processing end

//Deferred Rendering begin
void Renderer::loadMutiLight() {
	Vector3 heightmapSize = heightMap->GetHeightmapSize();

	pointLights = new Light[LIGHT_NUM];

	for (int i = 0; i < LIGHT_NUM; ++i) {
		Light& l = pointLights[i];
		l.SetPosition(Vector3(rand() % (int)heightmapSize.x, 150.0f, rand() % (int)heightmapSize.z));
		l.SetColour(Vector4(0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			1));

		l.SetRadius(800.0f + (rand() % 250));
	}

	sceneShader = new Shader("BumpVertex.glsl", "bufferFragment.glsl");
	pointlightShader = new Shader("pointlightvertex.glsl", "pointlightfrag.glsl");
	combineShader = new Shader("combinevert.glsl", "combinefrag.glsl");

	if (!sceneShader->LoadSuccess() || !pointlightShader->LoadSuccess() || !combineShader->LoadSuccess()) {
		return;
	}
	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);

	GLenum buffers[2] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1
	};


	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glEnable(GL_CULL_FACE);
}
void Renderer::GenerateScreenTexture(GLuint& into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}
void Renderer::FillBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(sceneShader);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "bumpTex"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);
	modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	UpdateShaderMatrices();
	heightMap->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPointLights() {
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	BindShader(pointlightShader);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	glUniform1i(glGetUniformLocation(pointlightShader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glUniform1i(glGetUniformLocation(pointlightShader->GetProgram(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(pointlightShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	glUniform2f(glGetUniformLocation(pointlightShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);


	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(pointlightShader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);

	UpdateShaderMatrices();
	for (int i = 0; i < LIGHT_NUM; ++i) {
		Light& l = pointLights[i];
		SetShaderLight(l);
		sphere->Draw();
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::CombineBuffers() {
	BindShader(combineShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	quad->Draw();
}

//Deferred Rendering end

void Renderer::drawtree() {
	tree = Mesh::LoadFromMeshFile("tree.msh");
	treeTex = SOIL_load_OGL_texture(TEXTUREDIR"tree.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (!treeTex)return;
	BindShader(model1shader);
	glUniform1i(glGetUniformLocation(model1shader->GetProgram(), "diffuseTex"), 1);

	Vector3 hSize = heightMap->GetHeightmapSize();
	hSize = hSize * Vector3(1, 1, 1);//control water level by change the y values
	//hSize = hSize - Vector3(0, 10, 0);//control water level by change the y values
	modelMatrix = Matrix4::Translation(hSize * 0.5f) * Matrix4::Scale(hSize * 0.5f) * Matrix4::Rotation(90, Vector3(1, 0, 0));

	textureMatrix = Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) * Matrix4::Scale(Vector3(10, 10, 10)) * Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

	UpdateShaderMatrices();
	tree->Draw();
}
void Renderer::updatetree(float dt) {
	treetime += dt;
	treesecond = treetime / 2;
	if (treesecond%2) {
		mod_tree2->SetTransform(mod_tree2->GetTransform() * Matrix4::Rotation(2.5f * dt, Vector3(1, 0, 1)));
		mod_tree3->SetTransform(mod_tree3->GetTransform() * Matrix4::Rotation(2.5f * dt, Vector3(1, 0, 1)));
		mod_tree4->SetTransform(mod_tree4->GetTransform() * Matrix4::Rotation(2.5f * dt, Vector3(1, 0, 1)));
		mod_tree5->SetTransform(mod_tree5->GetTransform() * Matrix4::Rotation(2.5f * dt, Vector3(1, 0, 0)));
		mod_tea->SetTransform(mod_tea->GetTransform() * Matrix4::Rotation(2.5f * dt, Vector3(1, 0, 0)));
		mod_tree->SetTransform(mod_tree->GetTransform() * Matrix4::Rotation(2.5f * dt, Vector3(1, 0, 1)));
	}
	else {
		mod_tree2->SetTransform(mod_tree2->GetTransform() * Matrix4::Rotation(-2.5f * dt, Vector3(1, 0, 1)));
		mod_tree3->SetTransform(mod_tree3->GetTransform() * Matrix4::Rotation(-2.5f * dt, Vector3(1, 0, 1)));
		mod_tree4->SetTransform(mod_tree4->GetTransform() * Matrix4::Rotation(-2.5f * dt, Vector3(1, 0, 1)));
		mod_tree5->SetTransform(mod_tree5->GetTransform() * Matrix4::Rotation(-2.5f * dt, Vector3(1, 0, 0)));
		mod_tea->SetTransform(mod_tea->GetTransform() * Matrix4::Rotation(-2.5f * dt, Vector3(1, 0, 0)));
		mod_tree->SetTransform(mod_tree->GetTransform() * Matrix4::Rotation(-2.5f * dt, Vector3(1, 0, 1)));
	}
	
}
void Renderer::drawparticle() {
	//glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(particleShader);
	glUseProgram(particleShader->GetProgram());
	glUniform1i(glGetUniformLocation(particleShader->GetProgram(), "diffuseTex"), 1);

	SetShaderParticleSize(emitter->GetParticleSize());
	emitter->SetParticleSize(8.0f);
	emitter->SetParticleVariance(1.0f);
	emitter->SetLaunchParticles(16.0f);
	emitter->SetParticleLifetime(2000.0f);
	emitter->SetParticleSpeed(0.1f);
	UpdateShaderMatrices();

	emitter->Draw();

	//glUseProgram(0);

	//SwapBuffers();
}
void Renderer::SetShaderParticleSize(float f) {
	glUniform1f(glGetUniformLocation(particleShader->GetProgram(), "particleSize"), f);
}