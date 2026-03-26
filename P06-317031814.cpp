/*
Práctica 6: Texturizado
*/
//para cargar imagen
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
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture dadoTexture;
Texture logofiTexture;
Texture octaedroTexture;

Model Kitt_M;
Model Llanta_M;
Model Dado_M;

// --- MODELOS DEL COCHE ---
Model Cuerpo_M;
Model Cofre_M;
Model LlantaD1_M; // Delantera Derecha
Model LlantaD2_M; // Delantera Izquierda
Model LlantaT1_M; // Trasera Derecha
Model LlantaT2_M; // Trasera Izquierda


Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_texture.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_texture.frag";




//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}



void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);



	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void CrearDado()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,

		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,

		// right
		4, 5, 6,
		6, 7, 4,

	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
	// average normals
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.26f,  0.34f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.49f,	0.34f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.49f,	0.66f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.26f,	0.66f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.5f,   0.34f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.74f,	0.34f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.74f,	0.66,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.5f,	0.66,		-1.0f,	0.0f,	0.0f,
		// back 
		// x      y      z      S      T        NX    NY    NZ
		-0.5f, -0.5f, -0.5f,   1.0f,  0.34f,    0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,   0.75f, 0.34f,    0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,   0.75f, 0.66f,    0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,   1.0f,  0.66f,    0.0f,  0.0f,  1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,   0.34f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.25f,	0.34f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.25f,	0.66f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	0.66f,		1.0f,	0.0f,	0.0f,

		// bottom
		// x      y      z      S      T       
		-0.5f, -0.5f,  0.5f,   0.5f,  0.0f,			0.0f,	-1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,   0.5f,  0.34f,		0.0f,	-1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,   0.74f, 0.34f,		0.0f,	-1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,   0.74f, 0.0f,			0.0f,	-1.0f,  0.0f,

		// up
		// x      y      z      S      T       
		-0.5f,  0.5f,  0.5f,   0.5f,  1.0f,			0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,   0.5f,  0.66f,		0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,   0.74f, 0.66f,		0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,   0.74f, 1.0f,			0.0f,  1.0f,  0.0f,

	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(dado);

}
void CrearOctaedro()
{
	unsigned int octa_indices[] = {
		0, 1, 2,    // Cara Superior Frontal Derecha
		3, 4, 5,    // Cara Superior Trasera Derecha
		6, 7, 8,    // Cara Superior Trasera Izquierda
		9, 10, 11,  // Cara Superior Frontal Izquierda
		12, 13, 14, // Cara Inferior Frontal Derecha
		15, 16, 17, // Cara Inferior Trasera Derecha
		18, 19, 20, // Cara Inferior Trasera Izquierda
		21, 22, 23  // Cara Inferior Frontal Izquierda
	};

	GLfloat octa_vertices[] = {
		// --- PIRÁMIDE SUPERIOR ---
		// Cara 1: Superior Frontal Derecha (Número 1 - Coordenadas corregidas)
		// x      y      z          S       T          NX     NY     NZ
		 0.0f,  1.0f,  0.0f,      0.34f,  0.20f,      0.5f,  0.5f,  0.5f,  // Punta superior
		 0.5f,  0.0f,  0.5f,      0.01f,  0.40f,      0.5f,  0.5f,  0.5f,  // Esquina Izq
		 0.5f,  0.0f, -0.5f,      0.01f,  0.01f,      0.5f,  0.5f,  0.5f,  // Esquina Der

		 // Cara 2: Superior Trasera Derecha (Mapeando el número 2)
		// x      y      z          S       T          NX     NY     NZ
		 0.0f,  1.0f,  0.0f,      0.34f,  0.20f,      0.5f,  0.5f, -0.5f,  // Punta
		 0.5f,  0.0f, -0.5f,      0.34f,  0.60f,      0.5f,  0.5f, -0.5f,  // Base Der
		-0.5f,  0.0f, -0.5f,      0.01f,  0.40f,      0.5f,  0.5f, -0.5f,  // Base Izq

		// Cara 3: Superior Trasera Izquierda (Mapeando el número 3)
		// x      y      z          S       T          NX     NY     NZ
		 0.0f,  1.0f,  0.0f,      0.34f,  0.20f,     -0.5f,  0.5f, -0.5f,  // Punta
		-0.5f,  0.0f, -0.5f,      0.67f,  0.40f,     -0.5f,  0.5f, -0.5f,  // Base Atrás
		-0.5f,  0.0f,  0.5f,      0.34f,  0.59f,     -0.5f,  0.5f, -0.5f,  // Base Frente

		// Cara 4: Superior Frontal Izquierda (Mapeando el número 4)
		// x      y      z          S       T          NX     NY     NZ
		 0.0f,  1.0f,  0.0f,      0.34f,  0.20f,     -0.5f,  0.5f,  0.5f,  // Punta
		-0.5f,  0.0f,  0.5f,      0.67f,  0.00f,     -0.5f,  0.5f,  0.5f,  // Base Izq
		 0.5f,  0.0f,  0.5f,      0.67f,  0.39f,     -0.5f,  0.5f,  0.5f,  // Base Der

		 // --- PIRÁMIDE INFERIOR ---
		 // Cara 5: Inferior Frontal Derecha (Mapeando el número 5) 
		// x      y      z          S       T          NX     NY     NZ
		 0.0f, -1.0f,  0.0f,      0.66f,  0.79f,      0.5f, -0.5f,  0.5f,  // Punta Abajo
		 0.5f,  0.0f,  0.5f,      0.34f,  0.60f,      0.5f, -0.5f,  0.5f,  // Base Der 
		 0.5f,  0.0f, -0.5f,      0.34f,  1.00f,      0.5f, -0.5f,  0.5f,  // Base Izq

		 // Cara 6: Inferior Trasera Derecha (Mapeando el número 6) 
		// x      y      z          S       T          NX     NY     NZ
		 0.0f, -1.0f,  0.0f,      0.67f,  0.80f,      0.5f, -0.5f, -0.5f,  // Punta Abajo 
		 0.5f,  0.0f, -0.5f,      0.67f,  0.40f,      0.5f, -0.5f, -0.5f,  // Base Der
		-0.5f,  0.0f, -0.5f,      0.34f,  0.60f,      0.5f, -0.5f, -0.5f,  // Base Izq 

		// Cara 7: Inferior Trasera Izquierda (Mapeando el número 7)
		// x      y      z          S       T          NX     NY     NZ
		 0.0f, -1.0f,  0.0f,      0.67f,  0.79f,     -0.5f, -0.5f, -0.5f,  // Punta Abajo
		-0.5f,  0.0f, -0.5f,      1.00f,  0.60f,     -0.5f, -0.5f, -0.5f,  // Base Atrás 
		-0.5f,  0.0f,  0.5f,      0.67f,  0.40f,     -0.5f, -0.5f, -0.5f,  // Base Frente

		// Cara 8: Inferior Frontal Izquierda (Mapeando el número 8)
		// x      y      z          S       T          NX     NY     NZ
		 0.0f, -1.0f,  0.0f,      0.67f,  0.79f,     -0.5f, -0.5f,  0.5f,  // Punta Abajo
		-0.5f,  0.0f,  0.5f,      1.00f,  1.00f,     -0.5f, -0.5f,  0.5f,  // Base Izq
		 0.5f,  0.0f,  0.5f,      1.00f,  0.60f,     -0.5f, -0.5f,  0.5f,  // Base Der
	};

	Mesh* octa = new Mesh();
	octa->CreateMesh(octa_vertices, octa_indices, 192, 24);
	meshList.push_back(octa);
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearDado();
	CreateShaders();
	//Creando octaedro
	CrearOctaedro();

	
	camera = Camera(glm::vec3(0.0f, 2.0f, 6.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -15.0f, 0.5f, 0.5f);

	// --- CARGA DE PIEZAS TEXTURIZADAS ---
	Cuerpo_M.LoadModel("Models/cuerpo_auto_texturizado.obj");
	Cofre_M.LoadModel("Models/cofre_auto_fi_logo.obj");
	LlantaD1_M.LoadModel("Models/llantaD1_texturizada.obj");
	LlantaD2_M.LoadModel("Models/llantaD2_texturizada.obj");
	LlantaT1_M.LoadModel("Models/llantaT1_texturizada.obj");
	LlantaT2_M.LoadModel("Models/llantaT2_texturizada.obj");

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dado_animales.tga");
	dadoTexture.LoadTextureA();
	logofiTexture = Texture("Textures/escudo_fi_color.tga");
	logofiTexture.LoadTextureA();


	octaedroTexture = Texture("Textures/octaedro_imagen.tga"); // <--- Tu archivo de GIMP
	octaedroTexture.LoadTextureA();


	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Dado_M = Model();
	Dado_M.LoadModel("Models/Dado_Animales.obj");




	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
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
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		color = glm::vec3(1.0f, 1.0f, 1.0f);//color blanco, multiplica a la información de color de la textura

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.15f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		meshList[2]->RenderMesh();


		/*
		Reporte de práctica :
		Ejercicio 1: Crear un dado de 8 caras y texturizarlo por medio de código
		*/
		// --- Dibujando el octaedro ---
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(2.0f, 1.5f, 2.0f));
		// Rotación base para mirar a la cámara
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		// --- APLICACIÓN DE LOS 3 GETTERS ---
		// B -> Rotación en X (respecto a la camara)
		model = glm::rotate(model, mainWindow.getrotacion1() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		// N -> Rotación en Y (respecto a la camara)
		model = glm::rotate(model, mainWindow.getrotacion2() * toRadians, glm::vec3(0.0f, -1.0f, 0.0f));
		// M -> Rotación en Z (respecto a la camara)
		model = glm::rotate(model, mainWindow.getrotacion3() * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		octaedroTexture.UseTexture();
		meshList[5]->RenderMesh();
		glDisable(GL_BLEND);
		
		
		/*
		Ejercicio 2: Importar el modelo de su coche con sus 4 llantas acomodadas
		y tener texturizadas las 4 llantas (diferenciar caucho y rin)  y
		texturizar el logo de la Facultad de ingeniería en el cofre de su propio modelo de coche
		*/
		
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
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
		Cuerpo_M.RenderModel();

		// 2. COFRE (Hijo)
		modelaux = model; // Hereda traslación y rotación del cuerpo
		modelaux = glm::translate(modelaux, glm::vec3(0.0f, 0.75f, 1.0f));
		// Rotación en Z para abrir/cerrar (Teclas F / G)
		modelaux = glm::rotate(modelaux, mainWindow.getcofre() * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
		Cofre_M.RenderModel();

		// 3. LLANTA DELANTERA DERECHA (Hijo - D1)
		modelaux = model; // Hereda del cuerpo
		modelaux = glm::translate(modelaux, glm::vec3(-0.5f, 0.3f, 1.4f));
		// Rotación sincronizada con H / J
		modelaux = glm::rotate(modelaux, mainWindow.getrotallanta() * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
		LlantaD1_M.RenderModel();

		// 4. LLANTA DELANTERA IZQUIERDA (Hijo - D2)
		modelaux = model; // Hereda del cuerpo
		modelaux = glm::translate(modelaux, glm::vec3(0.5f, 0.3f, 1.4f));
		// Rotación sincronizada con H / J
		modelaux = glm::rotate(modelaux, mainWindow.getrotallanta() * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
		LlantaD2_M.RenderModel();

		// 5. LLANTA TRASERA DERECHA (Hijo - T1)
		modelaux = model; // Hereda del cuerpo
		modelaux = glm::translate(modelaux, glm::vec3(-0.5f, 0.3f, -1.5f));
		// Rotación sincronizada con H / J
		modelaux = glm::rotate(modelaux, mainWindow.getrotallanta() * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
		LlantaT1_M.RenderModel();

		// 6. LLANTA TRASERA IZQUIERDA (Hijo - T2)
		modelaux = model; // Hereda del cuerpo
		modelaux = glm::translate(modelaux, glm::vec3(0.5f, 0.3f, -1.5f));
		// Rotación sincronizada con H / J
		modelaux = glm::rotate(modelaux, mainWindow.getrotallanta() * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f))); 
		LlantaT2_M.RenderModel();
		
		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}
