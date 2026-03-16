/* Práctica 5: Coche Jerárquico - Solo Cuerpo */
#define STB_IMAGE_IMPLEMENTATION
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

// --- MODELOS DEL COCHE ---
Model Cuerpo_M;
Model Cofre_M;
Model LlantaD1_M; // Delantera Derecha
Model LlantaD2_M; // Delantera Izquierda
Model LlantaT1_M; // Trasera Derecha
Model LlantaT2_M; // Trasera Izquierda

Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
static const char* vShader = "shaders/shader_m.vert";
static const char* fShader = "shaders/shader_m.frag";

void CreateObjects()
{
	unsigned int floorIndices[] = { 0, 2, 1, 1, 2, 3 };
	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	// Cámara posicionada para ver el coche de lado/frente
	camera = Camera(glm::vec3(0.0f, 1.0f, 6.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);

	// --- CARGA DE TODAS LAS PIEZAS ---
	Cuerpo_M.LoadModel("Models/cuerpo_auto_obj.obj");
	Cofre_M.LoadModel("Models/cofre_auto_obj.obj");
	LlantaD1_M.LoadModel("Models/llanta_D_1_obj.obj");
	LlantaD2_M.LoadModel("Models/llanta_D_2_obj.obj");
	LlantaT1_M.LoadModel("Models/llanta_T_1_obj.obj");
	LlantaT2_M.LoadModel("Models/llanta_T_2_obj.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");
	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// DIBUJO DEL PISO
		glm::mat4 model(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.5f, 0.5f, 0.5f)));
		meshList[0]->RenderMesh();

		// --- DIBUJO DEL AUTO ---
		glm::mat4 modelaux(1.0);

		// 1. CUERPO (Padre - Todo depende de este model)
		model = glm::mat4(1.0);
		// Traslación independiente en X con teclas K / L
		model = glm::translate(model, glm::vec3(mainWindow.getmuevex(), 0.0f, 0.0f));
		// --- ROTACIÓN DE AJUSTE ---
		// Rotamos 90 grados en Y para que el frente del carro vea a la cámara
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.0f))); // Cuerpo Amarillo
		Cuerpo_M.RenderModel();

		// 2. COFRE (Hijo - Color Negro)
		modelaux = model; // Hereda traslación y rotación del cuerpo
		modelaux = glm::translate(modelaux, glm::vec3(0.0f, 0.75f, 1.0f));
		// Rotación en Z para abrir/cerrar (Teclas F / G)
		modelaux = glm::rotate(modelaux, mainWindow.getcofre() * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f))); // Negro
		Cofre_M.RenderModel();

		// 3. LLANTA DELANTERA DERECHA (Hijo - D1)
		modelaux = model; // Hereda del cuerpo
		modelaux = glm::translate(modelaux, glm::vec3(-0.5f, 0.3f, 1.4f));
		// Rotación sincronizada con H / J
		modelaux = glm::rotate(modelaux, mainWindow.getrotallanta() * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f))); // Negra
		LlantaD1_M.RenderModel();

		// 4. LLANTA DELANTERA IZQUIERDA (Hijo - D2)
		modelaux = model; // Hereda del cuerpo
		modelaux = glm::translate(modelaux, glm::vec3(0.5f, 0.3f, 1.4f));
		// Rotación sincronizada con H / J
		modelaux = glm::rotate(modelaux, mainWindow.getrotallanta() * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f))); // Negra
		LlantaD2_M.RenderModel();

		// 5. LLANTA TRASERA DERECHA (Hijo - T1)
		modelaux = model; // Hereda del cuerpo
		modelaux = glm::translate(modelaux, glm::vec3(-0.5f, 0.3f, -1.5f));
		// Rotación sincronizada con H / J
		modelaux = glm::rotate(modelaux, mainWindow.getrotallanta() * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f))); // Negra
		LlantaT1_M.RenderModel();

		// 6. LLANTA TRASERA IZQUIERDA (Hijo - T2)
		modelaux = model; // Hereda del cuerpo
		modelaux = glm::translate(modelaux, glm::vec3(0.5f, 0.3f, -1.5f));
		// Rotación sincronizada con H / J
		modelaux = glm::rotate(modelaux, mainWindow.getrotallanta() * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f))); // Negra
		LlantaT2_M.RenderModel();


		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}