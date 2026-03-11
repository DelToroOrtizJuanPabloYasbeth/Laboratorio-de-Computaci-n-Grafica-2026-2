/*Práctica 4: Modelado Jerárquico.
Se implementa el uso de matrices adicionales para almacenar información de transformaciones geométricas que se quiere
heredar entre diversas instancias para que estén unidas
Teclas de la F a la K para rotaciones de articulaciones
*/
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases para dar orden y limpieza al còdigo
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z
using std::vector;
//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3

	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);

}
/*
Crear cilindro y cono con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

//función para crear un cono
void CrearCono(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);

	//ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);


	for (i = 0; i < res + 2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4

	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

}


int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList
	CrearCilindro(15, 1.0f);//índice 2 en MeshList  //cambio a 15 para llantas
	CrearCono(25, 2.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índice 4 en MeshList
	CreateShaders();



	/*Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.2f, 0.2f);
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);

	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4
	
	// Matrices de apoyo para la jerarquía del animal
	glm::mat4 modelaux(1.0f);    // Base para las patas
	glm::mat4 colaPivote(1.0f);   // Punto de conexión de la cola
	glm::mat4 cabezaPivote(1.0f); // Punto de conexión del cuello/cabeza

	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f); //inicializar Color para enviar a variable Uniform;

	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		//Cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Limpiar la ventana
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		// Enviamos Proyección y Vista (Cámara)
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		/*
		// =============================================
		// CABINA
		// =============================================

		// Reiniciar la matriz de modelo
		model = glm::mat4(1.0);

		// Traslación inicial para posicionar toda la grúa en el mundo
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, -4.0f));
		// Guardamos esta transformación (posición global) para que los hijos la hereden
		// pero sin la escala que aplicaremos a continuación.
		modelaux = model;

		// Transformaciones propias de la Cabina
		model = glm::scale(model, glm::vec3(4.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f); // Amarillo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh(); // Dibuja el cubo de la cabina

		// =============================================
		// BRAZO 1
		// =============================================
		// Recuperamos la matriz de la cabina (solo posición, sin escala)
		model = modelaux;

		// articulación 1: Posicionamos el brazo respecto a la cabina (ej. en el techo)
		model = glm::translate(model, glm::vec3(-0.5f, 0.2f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));

		// primer brazo que conecta con la cabina
		// Aplicamos el desplazamiento para que rote desde un extremo
		model = glm::translate(model, glm::vec3(-1.0f, 2.0f, 0.0f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model; // Guardamos para la articulación 2

		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		color = glm::vec3(1.0f, 1.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// =============================================
		// BRAZO 2
		// =============================================
		// Para descartar la escala cargamos la matrix auxiliar
		model = modelaux;

		// articulación 2
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;

		// dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		model = modelaux;
		// segundo brazo
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// =============================================
		//  BRAZO 3
		// =============================================
		// Para descartar la escala cargamos la matrix auxiliar del brazo 2
		model = modelaux;

		// articulación 3 (al final del brazo 2)
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		// Rotación de la articulación (controlada por teclado) + los 135 grados de la imagen
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3() + 35.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;

		// dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		model = modelaux;
		// tercer brazo
		// Se traslada en X porque, tras la rotación, el brazo se extiende "hacia adelante"
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Para descartar la escala cargamos la matrix auxiliar del brazo 3
		model = modelaux;

		// articulación 4 (punto final del brazo 3)
		// Nos movemos al extremo del brazo 3 (que mide 5 unidades de largo)
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		// Rotación de la canasta (puedes usar una variable nueva o dejarla fija)
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 1.0f,0.0f));
		modelaux = model;

		// dibujar una pequeña esfera (pivote de la canasta)
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();


		// =============================================
		//  CANASTA
		// =============================================
		model = modelaux;
		// La desplazamos un poco hacia abajo y hacia adelante para que cuelgue del pivote
		model = glm::translate(model, glm::vec3(0.5f, -0.7f, 0.0f));
		modelaux = model;

		// Escalamos para que sea un cubo donde quepa una persona (
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.7f, 0.7f, 0.7f); // Gris claro / Metálico
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh(); // Dibuja el cubo


		// =============================================
		// BASE DE LA CABINA (Pirámide Cuadrangular)
		// =============================================

		// Reiniciamos a la matriz de la cabina (posición global)
		// para no heredar rotaciones ni escalas de los brazos o la canasta.
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, -4.0f)); // Misma posición base que la cabina

		// La movemos hacia abajo para que quede debajo del cubo amarillo
		model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f));

		// Guardamos para las llantas
		modelaux = model;

		// Escalamos la pirámide para que sea una base sólida
		model = glm::scale(model, glm::vec3(4.0f, 2.0f, 3.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.5f, 0.5f, 0.5f); // Gris oscuro para la base
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		// Dibujamos la pirámide cuadrangular (índice 4 según tu código inicial)
		meshList[4]->RenderMesh();

		// =============================================
		// LLANTAS INDEPENDIENTES (COLOR NEGRO)
		// =============================================

		// --- LLANTA 1 ( - Tecla K) ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.5f, -0.8f, 1.8f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.6f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.1f, 0.1f, 0.1f))); // Negro
		meshList[2]->RenderMeshGeometry();

		// --- LLANTA 2 ( - Tecla L) ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.5f, -0.8f, 1.8f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.6f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.1f, 0.1f, 0.1f))); // Negro
		meshList[2]->RenderMeshGeometry();

		// --- LLANTA 3 ( - Tecla N) ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.5f, -0.8f, -1.8f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion7()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.6f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.1f, 0.1f, 0.1f))); // Negro
		meshList[2]->RenderMeshGeometry();

		// --- LLANTA 4 ( - Tecla M) ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.5f, -0.8f, -1.8f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion8()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.6f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.1f, 0.1f, 0.1f))); // Negro
		meshList[2]->RenderMeshGeometry();
		*/
		

		
		
		// =============================================
		// CUERPO DEL ANIMAL 
		// =============================================
		
		// Reiniciar la matriz de modelo
		model = glm::mat4(1.0);
 
		// Posición en el mundo
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -7.0f));

		// --- ALMACENAMIENTO DE MATRICES DE APOYO ---
		modelaux = model; // Para las 4 patas
		// Punto de salida para la cola
		glm::mat4 colaPivote = glm::translate(model, glm::vec3(-2.0f, 0.4f, 0.0f)); 
		// Punto de salida para el cuello/cabeza
		glm::mat4 cabezaPivote = glm::translate(model, glm::vec3(2.0f, 0.4f, 0.0f));
		// -------------------------------------------

		// Escala del Torso
		model = glm::scale(model, glm::vec3(4.0f, 1.5f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		// CAMBIO DE COLOR: Turquesa Brillante (R: 0.0, G: 1.0, B: 1.0)
		color = glm::vec3(0.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		meshList[0]->RenderMesh(); // Dibuja el cubo del torso

		// =============================================
		// 1. ARTICULACIÓN DELANTERA DERECHA
		// =============================================
		model = modelaux; // Reset al cuerpo
		model = glm::translate(model, glm::vec3(1.5f, 0.0f, 1.35f));
		model = glm::rotate(model, glm::radians(-135.0f + mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));

		// Dibujar Esfera Negra
		glm::mat4 model_sph = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_sph));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
		sp.render();

		// =============================================
		// 2. ARTICULACIÓN DELANTERA IZQUIERDA 
		// =============================================
		model = modelaux; // Reset al cuerpo
		model = glm::translate(model, glm::vec3(1.5f, 0.0f, -1.35f)); // Z Negativa
		model = glm::rotate(model, glm::radians(-135.0f + mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));

		// Dibujar Esfera Negra
		model_sph = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_sph));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
		sp.render();

		// =============================================
		// 3. ARTICULACIÓN TRASERA DERECHA 
		// =============================================
		model = modelaux; // Reset al cuerpo
		model = glm::translate(model, glm::vec3(-1.5f, 0.0f, 1.35f)); // X Negativa
		model = glm::rotate(model, glm::radians(-135.0f + mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));

		// Dibujar Esfera Negra
		model_sph = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_sph));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
		sp.render();

		// =============================================
		// 4. ARTICULACIÓN TRASERA IZQUIERDA 
		// =============================================
		model = modelaux; // Reset al cuerpo
		model = glm::translate(model, glm::vec3(-1.5f, 0.0f, -1.35f)); // X y Z Negativas
		model = glm::rotate(model, glm::radians(-135.0f + mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f));

		// Dibujar Esfera Negra
		model_sph = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_sph));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
		sp.render();

		// =============================================
		// 1. MUSLO DELANTERO DERECHO (X: 1.5, Z: 1.35)
		// =============================================
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.5f, 0.0f, 1.35f));
		model = glm::rotate(model, glm::radians(-135.0f + mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 pivote1 = model; // Punto de unión

		// Dibujar Esfera y Muslo Rojo
		glm::mat4 model_temp = glm::scale(pivote1, glm::vec3(0.45f, 0.45f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f))); // Negro
		sp.render();

		model_temp = glm::translate(pivote1, glm::vec3(0.75f, 0.0f, 0.0f));
		model_temp = glm::scale(model_temp, glm::vec3(1.5f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f))); // Rojo
		meshList[0]->RenderMesh();

		// =============================================
		// 2. MUSLO DELANTERO IZQUIERDO (X: 1.5, Z: -1.35)
		// =============================================
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.5f, 0.0f, -1.35f));
		model = glm::rotate(model, glm::radians(-135.0f + mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 pivote2 = model;

		// Dibujar Esfera y Muslo Rojo
		model_temp = glm::scale(pivote2, glm::vec3(0.45f, 0.45f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
		sp.render();

		model_temp = glm::translate(pivote2, glm::vec3(0.75f, 0.0f, 0.0f));
		model_temp = glm::scale(model_temp, glm::vec3(1.5f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
		meshList[0]->RenderMesh();

		// =============================================
		// 3. MUSLO TRASERO DERECHO (X: -1.5, Z: 1.35)
		// =============================================
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.5f, 0.0f, 1.35f));
		model = glm::rotate(model, glm::radians(-135.0f + mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 pivote3 = model;

		// Dibujar Esfera y Muslo Rojo
		model_temp = glm::scale(pivote3, glm::vec3(0.45f, 0.45f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
		sp.render();

		model_temp = glm::translate(pivote3, glm::vec3(0.75f, 0.0f, 0.0f));
		model_temp = glm::scale(model_temp, glm::vec3(1.5f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
		meshList[0]->RenderMesh();

		// =============================================
		// 4. MUSLO TRASERO IZQUIERDO (X: -1.5, Z: -1.35)
		// =============================================
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.5f, 0.0f, -1.35f));
		model = glm::rotate(model, glm::radians(-135.0f + mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 pivote4 = model;

		// Dibujar Esfera y Muslo Rojo
		model_temp = glm::scale(pivote4, glm::vec3(0.45f, 0.45f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
		sp.render();

		model_temp = glm::translate(pivote4, glm::vec3(0.75f, 0.0f, 0.0f));
		model_temp = glm::scale(model_temp, glm::vec3(1.5f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
		meshList[0]->RenderMesh();

		
		// =============================================
		// 1. RODILLA DELANTERA DERECHA (Control: Articulación 2)
		// =============================================
		model = pivote1; // Salimos desde el hombro derecho
		model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f)); // Al final del muslo rojo
		model = glm::rotate(model, glm::radians(90.0f + mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 rodilla1 = model; // Nuevo pivote para la parte baja

		// Esfera Rodilla 1
		model_temp = glm::scale(rodilla1, glm::vec3(0.45f, 0.45f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
		sp.render();

		// Pantorrilla Amarilla 1
		model_temp = glm::translate(rodilla1, glm::vec3(0.75f, 0.0f, 0.0f));
		model_temp = glm::scale(model_temp, glm::vec3(1.5f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.0f)));
		meshList[0]->RenderMesh();

		// =============================================
		// 2. RODILLA DELANTERA IZQUIERDA
		// =============================================
		model = pivote2;
		model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f + mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 rodilla2 = model;

		// Esfera Rodilla 2
		model_temp = glm::scale(rodilla2, glm::vec3(0.45f, 0.45f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
		sp.render();

		// Pantorrilla Amarilla 2
		model_temp = glm::translate(rodilla2, glm::vec3(0.75f, 0.0f, 0.0f));
		model_temp = glm::scale(model_temp, glm::vec3(1.5f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.0f)));
		meshList[0]->RenderMesh();

		// =============================================
		// 3. RODILLA TRASERA DERECHA
		// =============================================
		model = pivote3;
		model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f + mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 rodilla3 = model;

		// Esfera Rodilla 3
		model_temp = glm::scale(rodilla3, glm::vec3(0.45f, 0.45f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
		sp.render();

		// Pantorrilla Amarilla 3
		model_temp = glm::translate(rodilla3, glm::vec3(0.75f, 0.0f, 0.0f));
		model_temp = glm::scale(model_temp, glm::vec3(1.5f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.0f)));
		meshList[0]->RenderMesh();

		// =============================================
		// 4. RODILLA TRASERA IZQUIERDA
		// =============================================
		model = pivote4;
		model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f + mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 rodilla4 = model;

		// Esfera Rodilla 4
		model_temp = glm::scale(rodilla4, glm::vec3(0.45f, 0.45f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
		sp.render();

		// Pantorrilla Amarilla 4
		model_temp = glm::translate(rodilla4, glm::vec3(0.75f, 0.0f, 0.0f));
		model_temp = glm::scale(model_temp, glm::vec3(1.5f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.0f)));
		meshList[0]->RenderMesh();

		// =============================================
		// 1. PIE DELANTERO DERECHO (Fijo -135°)
		// =============================================
		model = rodilla1; // Salimos desde la rodilla derecha
		model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f)); // Al final de la pantorrilla amarilla
		model = glm::rotate(model, glm::radians(-135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 pie1 = model;

		// Esfera Tobillo 1
		model_temp = glm::scale(pie1, glm::vec3(0.42f, 0.42f, 0.42f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
		sp.render();

		// Pezuña Verde 1
		model_temp = glm::translate(pie1, glm::vec3(0.625f, 0.25f, 0.0f)); // 0.625 es la mitad de 1.25
		model_temp = glm::scale(model_temp, glm::vec3(1.25f, 0.5f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));
		meshList[0]->RenderMesh();

		// =============================================
		// 2. PIE DELANTERO IZQUIERDO
		// =============================================
		model = rodilla2;
		model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 pie2 = model;

		// Esfera Tobillo 2
		model_temp = glm::scale(pie2, glm::vec3(0.42f, 0.42f, 0.42f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
		sp.render();

		// Pezuña Verde 2
		model_temp = glm::translate(pie2, glm::vec3(0.625f, 0.25f, 0.0f));
		model_temp = glm::scale(model_temp, glm::vec3(1.25f, 0.5f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));
		meshList[0]->RenderMesh();

		// =============================================
		// 3. PIE TRASERO DERECHO
		// =============================================
		model = rodilla3;
		model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 pie3 = model;

		// Esfera Tobillo 3
		model_temp = glm::scale(pie3, glm::vec3(0.42f, 0.42f, 0.42f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
		sp.render();

		// Pezuña Verde 3
		model_temp = glm::translate(pie3, glm::vec3(0.625f, 0.25f, 0.0f));
		model_temp = glm::scale(model_temp, glm::vec3(1.25f, 0.5f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));
		meshList[0]->RenderMesh();

		// =============================================
		// 4. PIE TRASERO IZQUIERDO
		// =============================================
		model = rodilla4;
		model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 pie4 = model;

		// Esfera Tobillo 4
		model_temp = glm::scale(pie4, glm::vec3(0.42f, 0.42f, 0.42f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
		sp.render();

		// Pezuña Verde 4
		model_temp = glm::translate(pie4, glm::vec3(0.625f, 0.25f, 0.0f));
		model_temp = glm::scale(model_temp, glm::vec3(1.25f, 0.5f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_temp));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));
		meshList[0]->RenderMesh();

		// =============================================
		// ARTICULACIÓN DE LA COLA (Articulación 3 )
		// =============================================
		model = colaPivote; // Iniciamos desde el anclaje trasero del cuerpo

		// 1. Posicionamos la articulación en el extremo superior de la cola
		model = glm::translate(model, glm::vec3(4.0f, -0.5f, 0.0f));

		// 2. Rotación dinámica controlada por el usuario
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(1.0f, 0.5f, 0.5f));

		// 3. Inclinación fija de la cola (45 grados sobre el eje Z)
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		// Guardamos la matriz resultante como la base jerárquica
		glm::mat4 articulacionCola = model;

		// 4. Dibujar Esfera de la articulación (Mantiene su color negro para contraste)
		model = glm::scale(articulacionCola, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.1f, 0.1f, 0.1f)));
		sp.render();

		// 5. Dibujar el segmento físico de la cola
		model = articulacionCola;
		model = glm::translate(model, glm::vec3(0.75f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 0.7f, 0.7f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		// CAMBIO DE COLOR: Rosa Brillante (R: 1.0, G: 0.2, B: 0.6)
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 0.2f, 0.6f)));

		meshList[0]->RenderMesh();


		// =============================================
		// CUELLO (Segmento fijo y unión para cabeza)
		// =============================================
		model = cabezaPivote; // Iniciamos desde el anclaje delantero del cuerpo

		// 1. Posicionamos el origen del cuello relativo al anclaje del cuerpo
		model = glm::translate(model, glm::vec3(-3.5f, 0.0f, 0.0f));

		// 2. Inclinación fija del cuello (130 grados para orientación ascendente)
		model = glm::rotate(model, glm::radians(130.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		// Guardamos esta matriz para heredar la posición y rotación a la cabeza
		glm::mat4 cuelloBase = model;

		// 3. Dibujar el segmento del cuello
		model = cuelloBase;
		// Trasladamos 0.5f (mitad del largo) para que el cubo se extienda desde el origen
		model = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
		// Escala: Largo de 1.0 y grosor de 0.8
		model = glm::scale(model, glm::vec3(1.0f, 0.8f, 0.8f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		// CAMBIO DE COLOR: Naranja Brillante (R: 1.0, G: 0.5, B: 0.0)
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 0.5f, 0.0f)));

		meshList[0]->RenderMesh();

		// 4. ARTICULACIÓN FINAL CUELLO (Esfera negra para conectar con la cabeza)
		model = cuelloBase;
		// Nos movemos al extremo final del cuello (largo total de 1.0f)
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));

		// Guardamos esta matriz para que la cabeza nazca desde este punto
		glm::mat4 articulacionCabezaPivote = model;

		// Dibujo de la esfera de unión (Se mantiene negra para resaltar la articulación)
		model = glm::scale(articulacionCabezaPivote, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
		sp.render();

		
		// =============================================
		// CABEZA - PARTE 1: MANDÍBULA (Verde)
		// =============================================
		model = articulacionCabezaPivote; // Nace del extremo final del cuello

		// 1. Rotación fija de la mandíbula (50 grados en Z)
		model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		// Guardamos este estado como base para la jerarquía de la boca
		glm::mat4 mandibulaBase = model;

		// 2. Dibujar segmento de la mandíbula
		model = glm::translate(mandibulaBase, glm::vec3(0.6f, 0.0f, 0.0f)); // 0.6 es la mitad del largo 1.2
		model = glm::scale(model, glm::vec3(1.2f, 0.7f, 0.8f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f))); // Verde identificación
		meshList[0]->RenderMesh();
		// =============================================
		// CABEZA - PARTE 2: CRÁNEO (Más grande que la mandíbula)
		// =============================================
		model = mandibulaBase;

		// 1. Posicionamiento del Cráneo (ajustado para encajar sobre la mandíbula)
		model = glm::translate(model, glm::vec3(0.5f, -0.4f, 0.0f));
		glm::mat4 craneoBase = model; // Matriz de referencia para el cráneo y sus hijos

		// 2. Dibujar segmento del Cráneo
		model = glm::scale(craneoBase, glm::vec3(1.6f, 0.8f, 1.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		// CAMBIO DE COLOR: Azul Pastel (R: 0.68, G: 0.85, B: 0.9)
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.68f, 0.85f, 0.9f)));

		meshList[0]->RenderMesh();

		// =============================================
		// ARTICULACIONES DE LAS OREJAS (Esferas)
		// =============================================

		// --- Esfera Oreja Derecha ---
		// Posicionamos el pivote en la parte superior y lateral del cráneo
		model = glm::translate(craneoBase, glm::vec3(-0.5f, -0.2f, 0.55f));
		glm::mat4 articulacionOrejaD = model; // Guardamos para la oreja derecha

		// Dibujo de la esfera pequeña (escala 0.25 para que sea proporcional)
		model = glm::scale(articulacionOrejaD, glm::vec3(0.25f, 0.25f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f))); // Negro
		sp.render();

		// --- Esfera Oreja Izquierda ---
		// Posicionamos el pivote en el lado opuesto del eje Z
		model = glm::translate(craneoBase, glm::vec3(-0.5f, -0.2f, -0.55f));
		glm::mat4 articulacionOrejaI = model; // Guardamos para la oreja izquierda

		// Dibujo de la esfera pequeña
		model = glm::scale(articulacionOrejaI, glm::vec3(0.25f, 0.25f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f))); // Negro
		sp.render();

		// =============================================
		// OREJAS - COLOR MORADO ELÉCTRICO (Movimiento Independiente)
		// =============================================

		// --- OREJA DERECHA ---
		model = articulacionOrejaD;
		// Rotación fija e inclinación dinámica con Articulación 4 
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 1.0f, 1.0f));
		glm::mat4 orejaDPivote = model;

		// Dibujo Oreja Derecha
		model = glm::translate(orejaDPivote, glm::vec3(0.0f, 0.35f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.7f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		// CAMBIO DE COLOR: Morado Eléctrico (R: 0.5, G: 0.0, B: 1.0)
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.5f, 0.0f, 1.0f)));
		meshList[0]->RenderMesh();

		// --- OREJA IZQUIERDA ---
		model = articulacionOrejaI;
		// Rotación fija e inclinación dinámica con Articulación 5
		model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, -1.0f, 1.0f));
		glm::mat4 orejaIPivote = model;

		// Dibujo Oreja Izquierda
		model = glm::translate(orejaIPivote, glm::vec3(0.0f, 0.35f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.7f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		// CAMBIO DE COLOR: Morado Eléctrico
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.5f, 0.0f, 1.0f)));
		meshList[0]->RenderMesh();

		// =============================================
		// OJOS (Esferas Negras)
		// =============================================
		// Ojo Derecho
		model = glm::translate(craneoBase, glm::vec3(0.5f, -0.2f, 0.55f));
		model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f)); // Pequeños y discretos
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f))); // Negro
		sp.render();

		// Ojo Izquierdo
		model = glm::translate(craneoBase, glm::vec3(0.5f, -0.2f, -0.55f));
		model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f))); // Negro
		sp.render();
		
		
		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}



