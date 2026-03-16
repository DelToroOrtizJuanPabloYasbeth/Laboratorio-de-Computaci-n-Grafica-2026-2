/*
Práctica 5: Optimización y Carga de Modelos - Goddard Jerárquico
*/
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
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

// --- MODELOS INDIVIDUALES DE GODDARD ---
Model Cuerpo_M;
Model Mandibula_M;
Model PataDD_M;
Model PataDI_M;
Model PataTD_M;
Model PataTI_M;

Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

static const char* vShader = "shaders/shader_m.vert";
static const char* fShader = "shaders/shader_m.frag";

void CreateObjects()
{
	unsigned int indices[] = { 0, 3, 1, 1, 3, 2, 2, 3, 0, 0, 1, 2 };
	GLfloat vertices[] = {
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

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

	// Cambiamos 0.5f por 10.0f para mayor velocidad de traslación
	camera = Camera(glm::vec3(0.0f, 0.5f, 7.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 10.0f, 1.0f);

	// --- CARGA DE LAS PIEZAS NUEVAS ---
	Cuerpo_M.LoadModel("Models/godart_cuerpo_obj.obj");
	Mandibula_M.LoadModel("Models/godart_mandibula_obj.obj");
	PataDD_M.LoadModel("Models/godart_pata_delantera_derecha_obj.obj");
	PataDI_M.LoadModel("Models/godart_pata_delantera_izquierda_obj.obj");
	PataTD_M.LoadModel("Models/godart_pata_trasera_derecha_obj.obj");
	PataTI_M.LoadModel("Models/godart_pata_trasera_izquierda_obj.obj");

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
		glm::vec3 colorPiso = glm::vec3(0.5f, 0.5f, 0.5f);
		glm::mat4 model(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.7f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorPiso));
		meshList[0]->RenderMesh();

		
		// --- DIBUJO DE CUERPO, MANDÍBULA Y PATA DD ---
		glm::mat4 modelaux(1.0);

		// 1. CUERPO (Padre - Turqueza)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(mainWindow.getmuevex(), 0.0f, -1.5f));
		model = glm::rotate(model, mainWindow.getrotay() * toRadians, glm::vec3(0, 1, 0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 1.0f))); // Turqueza
		Cuerpo_M.RenderModel();

		// 2. MANDÍBULA (Hijo - Amarillo)
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(2.5f, 1.0f, 0.0f));
		// Rotación en Z usando la articulacion2 (Teclas F / G)
		modelaux = glm::rotate(modelaux, mainWindow.getmandibula() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.0f))); // Verde
		Mandibula_M.RenderModel();

		// 3. PATA DELANTERA DERECHA (Hijo - Azul Brillante)
		modelaux = model; // Hereda del cuerpo
		modelaux = glm::translate(modelaux, glm::vec3(0.5f, -0.5f, 0.75));
		// Rotación en Z usando la articulacion1 (Teclas I / O)
		modelaux = glm::rotate(modelaux, mainWindow.getarticulacion1() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f))); // Azul Brillante
		PataDD_M.RenderModel();

		
		// 4. PATA DELANTERA IZQUIERDA (Hijo - Verde Brillante)
		modelaux = model; // Hereda del cuerpo
		modelaux = glm::translate(modelaux, glm::vec3(0.5f, -0.5f, -0.75f));
		// Rotación en Z usando la articulacion2 (Teclas K / L)
		modelaux = glm::rotate(modelaux, mainWindow.getarticulacion2() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 1.0f))); //Purpura Brillante
		PataDI_M.RenderModel();

		// 5. PATA TRASERA DERECHA (Hijo - Rojo Brillante)
		modelaux = model; // Hereda del cuerpo amarillo
		modelaux = glm::translate(modelaux, glm::vec3(-1.0f, -1.25f, 0.75f));
		// Rotación en Z usando articulacion3 (Teclas N / M)
		modelaux = glm::rotate(modelaux, mainWindow.getarticulacion3() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f))); // Rojo Brillante
		PataTD_M.RenderModel();
		
		// 6. PATA TRASERA IZQUIERDA (Hijo - Verde)
		modelaux = model; // Hereda del cuerpo
		modelaux = glm::translate(modelaux, glm::vec3(-1.0f, -1.25f, -0.75f));
		// Rotación en Z usando la articulacion4 (Teclas V / B)
		modelaux = glm::rotate(modelaux, mainWindow.getarticulacion4() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f))); // Verde
		PataTI_M.RenderModel();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}