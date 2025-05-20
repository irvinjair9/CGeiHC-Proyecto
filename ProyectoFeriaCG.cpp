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
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//La musica
#include "inc/fmod.hpp"
#include <inc/fmod_errors.h>

FMOD::System* fmodSystem;
FMOD::Sound* music;
FMOD::Sound* soundHacha;
FMOD::Sound* soundBateo;
FMOD::Sound* soundTopo;
FMOD::Sound* soundDardos;
FMOD::Channel* channel = nullptr;
FMOD::Channel* channelAttraction = nullptr;


//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;



//Constante para la animación
float angulovaria = 0.0f;

float sunAngle = 0.0f;
//float sunSpeed = 0.05f;
float sunSpeed = 0.01f; //Velocidad de rotación del sol


// Variables para la animación de bateo
float tiempoLanzamiento = 0.0f;          // Contador para el lanzamiento de pelotas
const float intervaloLanzamiento = 3.0f;  // Intervalo de 3 segundos entre lanzamientos
bool pelotaEnMovimiento = false;         // Indica si la pelota está en movimiento
float velocidadPelota = 1.0f;           // Velocidad de la pelota
glm::vec3 posicionPelota;                // Posición actual de la pelota
glm::vec3 posicionInicial;               // Posición inicial de la pelota
float distanciaPelota = 0.0f;            // Distancia recorrida por la pelota

// Variables para la animación del brazo y bate
float anguloGolpe = 0.0f;                // Ángulo de rotación para el golpe
float velocidadGolpe = 0.95f;           // Velocidad de rotación del brazo
bool preparandoGolpe = false;            // Indica si está preparando el golpe
bool batGolpeando = false;               // Indica si el bat está golpeando
bool regresandoBat = false;              // Indica si el bat está regresando a posición

// Variables para controlar la aparición/desaparición de las medusas
struct EstadoMedusa {
	bool visible;          // Si la medusa está visible o no
	float altura;          // Posición Y actual de la medusa
	float alturaInicial;   // Altura base cuando está oculta
	float alturaMaxima;    // Altura máxima cuando está completamente visible
	float velocidad;       // Velocidad de movimiento
	float tiempoVisible;   // Cuánto tiempo permanece visible
	float tiempoActual;    // Contador de tiempo actual
	bool golpeada;         // Si fue golpeada por el martillo
	float tiempoGolpeada;  // Tiempo que lleva golpeada
};

// Variables adicionales para ciclos de aparición de medusas
float cicloAparicion = 50.0f;       // Tiempo total del ciclo completo
float tiempoEntreApariciones = 4.0f; // Tiempo entre cada aparición de medusa
float tiempoActualCiclo = 0.0f;    // Contador del ciclo actual

// Variables para la animación de Jake (martillo)
float anguloMartillo = 0.0f;
bool martilloEnMovimiento = false;
float velocidadMartillo = 5.0f;
float anguloMaximoMartillo = 90.0f;
bool martilloSubiendo = false;

// Variables adicionales para controlar el movimiento automático del martillo
float tiempoEntreGolpes = 0.1f;     // Tiempo entre golpes automáticos del martillo
float tiempoActualMartillo = 0.0f;  // Contador de tiempo para los golpes

// Crear un arreglo para controlar el estado de cada medusa 
EstadoMedusa estadoMedusas[5];



// Variables para la animación del lanzamiento del hacha
float anguloHacha = 0.0f;          // Ángulo de rotación del hacha
float velocidadHacha = 8.0f;       // Velocidad de rotación del hacha 
float posHachaZ = 400.0f;          // Posición inicial del hacha en Z
float posHachaX = -190.0f;         // Posición X del hacha (para variar donde golpea)
float posHachaY = 5.7f;            // Posición Y del hacha (3.7f + 2.0f inicial)
float lanzamientoVel = 4.0f;       // Velocidad del lanzamiento 
bool hachaLanzada = false;         // Estado del hacha (lanzada o no)
bool hachaEnPared = false;         // Indica si el hacha está clavada en la pared
float tiempoEnPared = 0.0f;        // Contador para el tiempo que el hacha está en la pared
float tiempoEnParedMax = 50.0f;    // Tiempo que el hacha permanece en la pared
// Variables para la animación del brazo con hacha
float anguloBrazo = 0.0f;          // Ángulo del brazo al lanzar
float velocidadBrazo = 5.0f;       // Velocidad de movimiento del brazo
bool preparandoLanzamiento = true; // Estado de preparación del lanzamiento
float anguloMaxBrazo = -90.0f;     // Ángulo máximo del brazo hacia atrás
float posicionHachaInicialZ = 400.0f; // Posición inicial del hacha
float anguloHachaFijo = 45.0f;     // Ángulo fijo cuando el hacha está clavada en la pared



// Variables para la animación del lanzamiento del dardo
float angulodardo = 0.0f;          // Ángulo de rotación del dardo
float velocidadDardo = 8.0f;       // Velocidad de rotación del dardo 
float posDardoZ = 200.0f;          // Posición inicial del dardo en Z
float posDardoX = -400.0f;         // Posición X del dardo (para variar donde golpea)
float posDardoY = 10.0f;            // Posición Y del dardo (3.7f + 2.0f inicial)
bool dardoLanzada = false;         // Estado del dardo (lanzada o no)
bool dardoEnPared = false;         // Indica si el dardo está clavada en la pared
// Variables para la animación del brazo con dardo
//float anguloBrazoD = 0.0f;          // Ángulo del brazo al lanzar
//float velocidadBrazoD = 5.0f;       // Velocidad de movimiento del brazo
//bool preparandoLanzamientoD = true; // Estado de preparación del lanzamiento
//float anguloMaxBrazoD = -90.0f;     // Ángulo máximo del brazo hacia atrás
float posicionDardoInicialZ = 200.0f; // Posición inicial del dardo
float anguloDardoFijo = 0.0f;     // Ángulo fijo cuando el dardo está clavada en la pared

//Giro de finn
float anguloGiroFinn = 0.0f; // Ángulo de rotación de Finn






// Variables para la animación de Bob Esponja 
float saltoAltura = 0.0f;          // Altura actual del salto
float saltoCiclo = 0.0f;           // Ciclo para el salto
float saltoVelocidadCiclo = 5.0f;  // Velocidad del ciclo de salto
float brazoAngulo = 0.0f;          // Ángulo para los brazos
float piernaAngulo = 0.0f;         // Ángulo para las piernas
float posicionZ = -250.0f;         // Posición inicial en Z (centro del rango)
float movimientoRango = 100.0f;    // Rango de movimiento (de -150 a -350 es 200 unidades, mitad es 100)
float movimientoVelocidad = 0.5f;  // Velocidad del movimiento
bool avanzando = false;            // Estado del movimiento (comienza yendo hacia atrás)
float rotacionY = 0.0f;            // Rotación en Y para cambiar dirección

// Variables para la animación de HaroldN
float haroldPosX = -150.0f;      // Posición inicial X 
float haroldTargetX = 50.0f;     // Posición destino X
float haroldSpeed = 0.5f;        // Velocidad de movimiento
bool haroldMovingForward = true; // Dirección del movimiento

//variables animacion bolos/dados
float movballOffset;
float movBall;
float rotball;
float rotballOffset;
float movHd;
float movOffset;
float movhdOffset;
bool arrb;

float movDiceY;
float movDiceX;

float movCos;



Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;


//Todo lo relacionado a la camara
Camera camera;

Camera aerocamera;


glm::vec3 camPositionBackup;
GLfloat yawBackup, pitchBackup;
bool respaldoHecho = false;
bool camaraJuegoActiva = false;





Texture pisoTexture;
Texture pisoETexture; //Piso del kiosko
Texture pisoH; //Piso Hora de aventura
Texture pisoB; //Piso Bob Esponja
Texture pisoL; //Piso Los padrinos magicos
Texture camino; //Piso camino de conexión

//Modelos hora de aventura
Model BMO;
Model CasaDelArbol;
Model JakeCuerpo;
Model JakeBrazoDer;
Model JakeBrazoIzq;
Model JakePiernaDer;
Model JakePiernaIzq;
Model Prismo;
Model MarcelinesGuitar;
Model CoinReceptor;
Model JuegoDardos;
Model Dardo;
Model CasaMarceline;
Model FINN;






//Modelos Bob esponja
Model Banca;
Model BancaCom;
Model LamparaBob;
Model CasaBob;
Model BobBrazoDer;
Model BobBrazoIzq;
Model BobCuerpo;
Model BobPiernaDer;
Model BobPiernaIzq;
Model Cangre;
Model CasaCalam;
Model CasaPatricio;
Model CrustaceoCas;
Model HaroldN;
Model Jaula;
Model Pizzas;
Model Topo;
Model Bat;
Model Martillo;
Model Medusa1;
Model Pelota;
Model Coin;



//Modelos Los padrinos magicos
Model varita;
Model Cosmo;
Model Wanda;
Model Timmy;
Model CasaTimmy;
Model Bolos;
Model Ball;
Model Table;
Model Dice1;


Skybox skyboxDay;
Skybox skyboxNight;

//materiales
Material Material_brillante;
Material Material_opaco;

//Materiales extra
Material Material_aluminio;
Material Material_madera;
Material Material_jake;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";














//función de calculo de normales por promedio de vértices 
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

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

//Función para la musica con Fmod
void iniciarFMOD() {
	FMOD::System_Create(&fmodSystem);
	fmodSystem->init(512, FMOD_INIT_NORMAL, 0);
	fmodSystem->createSound("sounds/Fondo_Feria.wav", FMOD_LOOP_NORMAL, 0, &music);
	// Sonidos de atracciones
	fmodSystem->createSound("sounds/hacha.wav", FMOD_LOOP_NORMAL, 0, &soundHacha);
	fmodSystem->createSound("sounds/beisbol.wav", FMOD_LOOP_NORMAL, 0, &soundBateo);
	fmodSystem->createSound("sounds/topo.wav", FMOD_LOOP_NORMAL, 0, &soundTopo);
	fmodSystem->createSound("sounds/dardos.wav", FMOD_LOOP_NORMAL, 0, &soundDardos);
	fmodSystem->playSound(music, 0, false, &channel);
}

void actualizarFMOD() {
	fmodSystem->update();
}

void liberarFMOD() {
	music->release();
	soundHacha->release();
	soundBateo->release();
	soundTopo->release();
	soundDardos->release();
	fmodSystem->close();
	fmodSystem->release();
}

void manejarSonidosAtracciones(glm::vec3 camPos, glm::vec3 camDir) {
	static int atraccionActual = -1; // -1 = ninguna, 0 = hacha, 1 = bateo, etc.
	static bool sonidoReproduciendo = false;

	// Ángulo de visión de la cámara (en radianes)
	const float anguloVision = glm::radians(45.0f); // Asumiendo un FOV de 45 grados

	// Función para verificar si un punto está dentro del campo de visión
	auto estaEnCampoVision = [&](glm::vec3 targetPos) {
		glm::vec3 dirToTarget = glm::normalize(targetPos - camPos);
		float angulo = acos(glm::dot(camDir, dirToTarget));
		return angulo < anguloVision;
		};

	// Posiciones de las atracciones
	glm::vec3 posHacha(-220.0f, 0.0f, 419.0f);
	glm::vec3 posBateo(15.0f, 0.0f, -395.0f);
	glm::vec3 posTopo(-125.0f, 0.0f, -200.0f);
	glm::vec3 posDardos(-390.0f, 0.0f, 182.0f);

	// Determinar si estamos viendo una atracción
	int nuevaAtraccion = -1;
	FMOD::Sound* sonido = nullptr;

	if (estaEnCampoVision(posHacha) && glm::distance(camPos, posHacha) < 100.0f) {
		nuevaAtraccion = 0;
		sonido = soundHacha;
	}
	else if (estaEnCampoVision(posBateo) && glm::distance(camPos, posBateo) < 100.0f) {
		nuevaAtraccion = 1;
		sonido = soundBateo;
	}
	else if (estaEnCampoVision(posTopo) && glm::distance(camPos, posTopo) < 100.0f) {
		nuevaAtraccion = 2;
		sonido = soundTopo;
	}
	else if (estaEnCampoVision(posDardos) && glm::distance(camPos, posDardos) < 100.0f) {
		nuevaAtraccion = 3;
		sonido = soundDardos;
	}

	// Manejar cambios de atracción
	if (nuevaAtraccion != atraccionActual) {
		// Detener sonido anterior si está reproduciéndose
		if (sonidoReproduciendo) {
			channelAttraction->stop();
			sonidoReproduciendo = false;
		}

		// Reproducir nuevo sonido si estamos viendo una atracción
		if (nuevaAtraccion != -1) {
			fmodSystem->playSound(sonido, 0, false, &channelAttraction);
			sonidoReproduciendo = true;
		}

		atraccionActual = nuevaAtraccion;
	}

	// Actualizar sistema de audio
	fmodSystem->update();
}

//---------------------------------------DECORACIONES--------------------------------//

//Creación de bancas para decorar
void RenderBanca(glm::vec3 posicion, float rotY, GLuint uniformModel, glm::vec3 escala = glm::vec3(1.0f)) {
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, posicion);
	base = glm::rotate(base, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::scale(base, escala);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
	Banca.RenderModel();
}

//Creación de bancas comedor para decorar
void RenderBancaCom(glm::vec3 posicion, float rotY, GLuint uniformModel, glm::vec3 escala = glm::vec3(0.7f)) {
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, posicion);
	base = glm::rotate(base, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::scale(base, escala);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
	BancaCom.RenderModel();
}

//Creación lamparas 
void RenderLamparaBob(glm::vec3 posicion, float rotY, GLuint uniformModel, glm::vec3 escala = glm::vec3(0.7f)) {
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, posicion);
	base = glm::rotate(base, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::scale(base, escala);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
	LamparaBob.RenderModel();
}

void RenderVarita(glm::vec3 posicion, float rotY, GLuint uniformModel, glm::vec3 escala = glm::vec3(1000.0f)) {
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, posicion);
	base = glm::rotate(base, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::scale(base, escala);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
	varita.RenderModel();
}

void RenderRecibidorMonedas(glm::vec3 posicion, float rotY, GLuint uniformModel, glm::vec3 escala = glm::vec3(1.0f)) {
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, posicion);
	base = glm::rotate(base, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::scale(base, escala);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
	CoinReceptor.RenderModel();
}




//---------------------------------------ANIMACION-----------------------------------//


// JAULA DE BATEO -------------------------------------------------------
// Función para actualizar la animación de la jaula de bateo
void actualizarAnimacionBateo(GLfloat deltaTime) {
	// Actualizar el temporizador de lanzamiento
	tiempoLanzamiento += deltaTime;

	// Si no hay pelota en movimiento y es tiempo de lanzar una nueva
	if (!pelotaEnMovimiento && tiempoLanzamiento >= intervaloLanzamiento) {
		tiempoLanzamiento = 0.0f;
		pelotaEnMovimiento = true;
		distanciaPelota = 0.0f;
		// Guardar la posición inicial de la pelota (según tu código)
		posicionInicial = glm::vec3(-23.0f, -0.3f, -1.5f);
		posicionPelota = posicionInicial;

		// Preparar el brazo para golpear
		preparandoGolpe = true;
		anguloGolpe = 0.0f;
	}

	// Si la pelota está en movimiento, actualizar su posición
	if (pelotaEnMovimiento) {
		// Calcular la nueva distancia recorrida
		distanciaPelota += velocidadPelota * deltaTime;

		// Calcular la posición interpolada de la pelota desde la máquina hasta cerca de Jake
		float t = distanciaPelota / 100.0f;
		if (t > 1.0f) t = 1.0f;

		// La pelota se mueve hacia Jake 
		float newX = posicionInicial.x + t * 100.0f; // Mover 50 unidades en X
		float newY = posicionInicial.y;

		// Actualizar la posición de la pelota
		posicionPelota = glm::vec3(newX, newY, posicionInicial.z);

		// Si la pelota llega cerca de Jake (donde está el bat)
		if (t >= 0.7f && !batGolpeando && preparandoGolpe) {
			// Iniciar el golpe
			batGolpeando = true;
			preparandoGolpe = false;
		}

		// Si la pelota completa su recorrido
		if (t >= 1.0f) {
			// Reiniciar para el próximo lanzamiento
			pelotaEnMovimiento = false;
		}
	}

	// Animación del brazo y bate
	if (preparandoGolpe) {
		// Preparación: llevar el brazo hacia atrás
		anguloGolpe -= velocidadGolpe * 0.5f * deltaTime;
		if (anguloGolpe <= -45.0f) {
			anguloGolpe = -45.0f;
		}
	}
	else if (batGolpeando) {
		// Golpe: mover el brazo hacia adelante rápidamente
		anguloGolpe += velocidadGolpe * deltaTime;
		if (anguloGolpe >= 30.0f) {
			anguloGolpe = 30.0f;
			batGolpeando = false;
			regresandoBat = true;
		}

		// Si el bat golpea la pelota (detección simple de colisión)
		if (anguloGolpe > 0.0f && anguloGolpe < 15.0f && pelotaEnMovimiento) {
			// La distancia de la pelota está alrededor del punto donde ocurriría el contacto
			float posX = posicionPelota.x - posicionInicial.x;
			if (posX > 55.0f && posX < 65.0f) {  // Si la pelota está cerca de Jake
				// Cambiar la dirección de la pelota (simular que fue golpeada)
				velocidadPelota *= 0.5f; // Acelerar la pelota
				// Invertir dirección (la pelota vuelve en dirección opuesta)
				distanciaPelota = 100.0f * 0.6f; // Reiniciar la distancia recorrida
			}
		}
	}
	else if (regresandoBat) {
		// Regresar lentamente a la posición neutral
		anguloGolpe -= velocidadGolpe * 0.3f * deltaTime;
		if (anguloGolpe <= 0.0f) {
			anguloGolpe = 0.0f;
			regresandoBat = false;
		}
	}
}

//GOLPEA AL TOPO--------------------------------------------------------------

// Función para inicializar los estados de las medusas
void inicializarMedusas() {
	// Inicializar semilla para números aleatorios
	srand(static_cast<unsigned int>(time(nullptr)));

	// Configuración inicial para todas las medusas
	for (int i = 0; i < 5; i++) {
		estadoMedusas[i].visible = false;
		estadoMedusas[i].alturaInicial = 3.3f;    // Altura cuando está oculta
		estadoMedusas[i].alturaMaxima = 10.0f;     // Altura cuando está visible
		estadoMedusas[i].altura = estadoMedusas[i].alturaInicial;
		estadoMedusas[i].velocidad = 0.1f + (rand() % 10) / 300.0f;  // Velocidad aleatoria
		estadoMedusas[i].tiempoVisible = 6.0f + (rand() % 20) / 10.0f;  // Tiempo visible aleatorio
		estadoMedusas[i].tiempoActual = 0.0f;
		estadoMedusas[i].golpeada = false;
		estadoMedusas[i].tiempoGolpeada = 0.0f;
	}
}

// Función para actualizar la lógica de animación de las medusas
void actualizarMedusas(float deltaTime) {
	// Actualizar el contador del ciclo
	tiempoActualCiclo += deltaTime;
	if (tiempoActualCiclo > cicloAparicion) {
		tiempoActualCiclo = 0.0f;
	}

	// Fase del ciclo para determinar qué medusas aparecen
	float faseCiclo = tiempoActualCiclo / cicloAparicion;

	for (int i = 0; i < 5; i++) {
		// Momento específico para que aparezca cada medusa 
		float momentoAparicion = (float)i / 5.0f;
		float margenAparicion = tiempoEntreApariciones / cicloAparicion;

		// Verificar si es el momento de que esta medusa aparezca
		bool debeAparecer = false;
		if (faseCiclo > momentoAparicion && faseCiclo < momentoAparicion + margenAparicion) {
			debeAparecer = true;
		}

		if (estadoMedusas[i].visible) {
			// Si la medusa está visible, actualizar su tiempo
			estadoMedusas[i].tiempoActual += deltaTime;

			// Si ya pasó su tiempo visible, comenzar a ocultarla
			if (estadoMedusas[i].tiempoActual >= estadoMedusas[i].tiempoVisible) {
				estadoMedusas[i].altura -= estadoMedusas[i].velocidad * deltaTime;

				// Si llegó a su posición inicial, marcarla como no visible
				if (estadoMedusas[i].altura <= estadoMedusas[i].alturaInicial) {
					estadoMedusas[i].altura = estadoMedusas[i].alturaInicial;
					estadoMedusas[i].visible = false;
					estadoMedusas[i].tiempoActual = 0.0f;
				}
			}
			else {
				// Si todavía está en tiempo visible, asegurarse de que esté en su altura máxima
				if (estadoMedusas[i].altura < estadoMedusas[i].alturaMaxima) {
					estadoMedusas[i].altura += estadoMedusas[i].velocidad * deltaTime;
					if (estadoMedusas[i].altura > estadoMedusas[i].alturaMaxima) {
						estadoMedusas[i].altura = estadoMedusas[i].alturaMaxima;
					}
				}
			}
		}
		else if (debeAparecer) {
			// Si la medusa debe aparecer según el ciclo
			estadoMedusas[i].visible = true;
			estadoMedusas[i].tiempoActual = 0.0f;

			// Variar ligeramente la velocidad y tiempo para que no sea demasiado predecible
			estadoMedusas[i].velocidad = 0.1f + (rand() % 10) / 300.0f;
			estadoMedusas[i].tiempoVisible = 6.0f + (rand() % 20) / 20.0f;
		}
	}

	// Añadir aleatorización ocasional - medusa extra aleatoria cada cierto tiempo
	if (rand() % 100 < 2) {
		int medusaAleatoria = rand() % 5;
		if (!estadoMedusas[medusaAleatoria].visible) {
			estadoMedusas[medusaAleatoria].visible = true;
			estadoMedusas[medusaAleatoria].tiempoActual = 0.0f;
			estadoMedusas[medusaAleatoria].velocidad = 0.15f + (rand() % 10) / 100.0f; // Más rápida
			estadoMedusas[medusaAleatoria].tiempoVisible = 5.0f + (rand() % 10) / 20.0f; // Menos tiempo visible
		}
	}
}

// Función para actualizar la animación del martillo
void actualizarMartillo(float deltaTime) {
	// Actualizar el contador de tiempo
	tiempoActualMartillo += deltaTime;

	// Si no está en movimiento y ha pasado el tiempo necesario, iniciar un nuevo golpe
	if (!martilloEnMovimiento && tiempoActualMartillo >= tiempoEntreGolpes) {
		martilloEnMovimiento = true;
		martilloSubiendo = true;
		tiempoActualMartillo = 0.0f;
	}

	// Animar el martillo si está en movimiento
	if (martilloEnMovimiento) {
		if (martilloSubiendo) {
			anguloMartillo += velocidadMartillo * deltaTime;
			if (anguloMartillo >= anguloMaximoMartillo) {
				anguloMartillo = anguloMaximoMartillo;
				martilloSubiendo = false;
			}
		}
		else {
			anguloMartillo -= velocidadMartillo * deltaTime;
			if (anguloMartillo <= 0.0f) {
				anguloMartillo = 0.0f;
				martilloEnMovimiento = false;
			}
		}
	}
}

void actualizarAnimaciones(float deltaTime) {
	actualizarMedusas(deltaTime);
	actualizarMartillo(deltaTime);
}



// LANZAMIENTO DE HACHA-------------------------------------------------------------------
// Función para generar un número aleatorio en un rango
float randomFloat(float min, float max) {
	return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

// Función para actualizar la animación del hacha
void actualizarAnimacionHacha(float deltaTime) {
	// Rotación constante del hacha mientras está en vuelo
	if (hachaLanzada && !hachaEnPared) {
		anguloHacha += velocidadHacha * deltaTime;
		if (anguloHacha >= 360.0f) {
			anguloHacha -= 360.0f;
		}
	}
	// Si el hacha no está lanzada ni en la pared
	if (!hachaLanzada && !hachaEnPared) {
		// Fase de preparación del lanzamiento
		if (preparandoLanzamiento) {
			// Movimiento del brazo hacia atrás
			anguloBrazo -= velocidadBrazo * deltaTime;
			if (anguloBrazo <= anguloMaxBrazo) {
				anguloBrazo = anguloMaxBrazo;
				preparandoLanzamiento = false;
			}
		}
		else {
			// Movimiento del brazo hacia adelante
			anguloBrazo += velocidadBrazo * deltaTime;
			if (anguloBrazo >= 80.0f) {
				anguloBrazo = 80.0f;
				hachaLanzada = true;
				// Al iniciar el lanzamiento, decidimos el punto de destino aleatorio
				posHachaX = randomFloat(-200.0f, -180.0f); // Variación en X
				posHachaY = randomFloat(4.0f, 8.0f);       // Variación en Y
			}
		}
	}
	// Si el hacha está en vuelo
	else if (hachaLanzada && !hachaEnPared) {
		// Movimiento del hacha hacia la pared
		posHachaZ += lanzamientoVel * deltaTime;
		// Si el hacha llega a la pared
		if (posHachaZ >= 468.0f) {
			posHachaZ = 468.0f;
			hachaEnPared = true;  // El hacha ahora está clavada en la pared
			tiempoEnPared = 0.0f; // Iniciamos el contador de tiempo
		}
	}
	// Si el hacha está clavada en la pared
	else if (hachaEnPared) {
		// Contamos el tiempo que el hacha permanece en la pared
		tiempoEnPared += deltaTime;
		// Si se cumple el tiempo máximo en la pared
		if (tiempoEnPared >= tiempoEnParedMax) {
			// Reseteamos los estados para que el hacha reaparezca en la mano
			hachaLanzada = false;
			hachaEnPared = false;
			preparandoLanzamiento = true;
			anguloBrazo = 0.0f;  // Reset del ángulo del brazo
			posHachaZ = posicionHachaInicialZ; // Reposicionar el hacha directamente
		}
	}
}


//LANZAMIENTO DE DARDO-------------------------------------------------------------------
// Función para actualizar animación de dardos 
void actualizarAnimacionDardo(float deltaTime) {
	if (dardoLanzada && !dardoEnPared) {
		angulodardo += velocidadDardo * deltaTime;
		if (angulodardo >= 360.0f) {
			angulodardo -= 360.0f;
		}
	}

	if (!dardoLanzada && !dardoEnPared) {
		// Fase de preparación del lanzamiento
		if (preparandoLanzamiento) {
			// Movimiento del brazo hacia atrás
			anguloBrazo -= velocidadBrazo * deltaTime;
			if (anguloBrazo <= anguloMaxBrazo) {
				anguloBrazo = anguloMaxBrazo;
				preparandoLanzamiento = false;
			}
		}
		else {
			// Movimiento del brazo hacia adelante
			anguloBrazo += velocidadBrazo * deltaTime;
			if (anguloBrazo >= 80.0f) {
				anguloBrazo = 80.0f;
				dardoLanzada = true;
				// Al iniciar el lanzamiento, decidimos el punto de destino aleatorio
				posDardoX = randomFloat(-410.0f, -390.0f); // Variación en X
				posDardoY = randomFloat(8.0f, 10.0f);       // Variación en Y
			}
		}
	}
	else if (dardoLanzada && !dardoEnPared) {
		posDardoZ += lanzamientoVel * deltaTime;
		// Si el hacha llega a la pared
		if (posDardoZ >= 152.0f) {
			posDardoZ = 152.0f;
			dardoEnPared = true;  // El hacha ahora está clavada en la pared
			tiempoEnPared = 0.0f; // Iniciamos el contador de tiempo
		}
	}
	// Si el dardo está clavado en la pared
	else if (dardoEnPared) {
		// Contamos el tiempo que el dardo permanece en la pared
		tiempoEnPared += deltaTime;
		// Si se cumple el tiempo máximo en la pared
		if (tiempoEnPared >= tiempoEnParedMax) {
			// Reseteamos los estados para que el hacha reaparezca en la mano
			dardoLanzada = false;
			dardoEnPared = false;
			preparandoLanzamiento = true;
			anguloBrazo = 0.0f;  // Reset del ángulo del brazo
			posDardoZ = posicionDardoInicialZ; // Reposicionar el hacha directamente
		}
	}
}





void animarBobEsponja(float deltaTime) {
	// Actualizar ciclo de salto
	saltoCiclo += saltoVelocidadCiclo * deltaTime;
	if (saltoCiclo >= 360.0f) {
		saltoCiclo -= 360.0f;
	}

	// Calcular altura del salto usando una función seno
	saltoAltura = 1.0f * sin(glm::radians(saltoCiclo));

	// Calcular ángulos de brazos y piernas para que se muevan al caminar
	brazoAngulo = 25.0f * sin(glm::radians(saltoCiclo));
	piernaAngulo = 15.0f * sin(glm::radians(saltoCiclo));

	// Manejar el movimiento adelante/atrás
	if (avanzando) {
		posicionZ += movimientoVelocidad * deltaTime;
		rotacionY = glm::pi<float>(); // 180 grados, mira hacia Z negativo (cuando avanza en Z positivo)

		if (posicionZ >= -150.0f) { // Límite hacia adelante
			posicionZ = -150.0f;
			avanzando = false;
		}
	}
	else {
		posicionZ -= movimientoVelocidad * deltaTime;
		rotacionY = 0.0f; // 0 grados, mira hacia Z positivo (cuando retrocede en Z negativo)

		if (posicionZ <= -350.0f) { // Límite hacia atrás
			posicionZ = -350.0f;
			avanzando = true;
		}
	}
}

void actualizarAnimacionHarold(float deltaTime) {
	if (haroldMovingForward) {
		// Movimiento hacia X positivo
		haroldPosX += haroldSpeed * deltaTime;
		if (haroldPosX >= haroldTargetX) {
			haroldPosX = haroldTargetX;
			haroldMovingForward = false;
		}
	}
	else {
		// Movimiento hacia X negativo (regreso)
		haroldPosX -= haroldSpeed * deltaTime;
		if (haroldPosX <= -150.0f) {
			haroldPosX = -150.0f;
			haroldMovingForward = true;
		}
	}
}


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();





	mainLight.SetAmbientIntensity(0.05f); //intensidad de la luz
	mainLight.SetDiffuseIntensity(0.05f); //intensidad de la luz difusa


	CreateObjects();
	CreateShaders();
	iniciarFMOD(); //iniciar la musica

	//Camara tercera persona
	camera = Camera(glm::vec3(0.0f, -200.0f, -30.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 0.0f, 0.0f);

	//Camara aerea
	aerocamera = Camera(glm::vec3(0.0f, 300.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -3.0f, 0.0f, 0.0f);


	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();

	pisoH = Texture("Textures/pisoH.tga");
	pisoH.LoadTextureA();
	pisoB = Texture("Textures/pisoB.tga");
	pisoB.LoadTextureA();
	pisoL = Texture("Textures/pisoL.tga");
	pisoL.LoadTextureA();
	pisoETexture = Texture("Textures/pisoE.tga");
	pisoETexture.LoadTextureA();

	camino = Texture("Textures/camino.tga");
	camino.LoadTextureA();

	//Modelos hora de aventura
	BMO = Model();
	BMO.LoadModel("Models/BMO.obj");

	CasaDelArbol = Model();
	CasaDelArbol.LoadModel("Models/CasaDelArbol.obj");

	JakeCuerpo = Model();
	JakeCuerpo.LoadModel("Models/CuerpoJake.obj");

	JakeBrazoDer = Model();
	JakeBrazoDer.LoadModel("Models/BrazoDerechoJake.obj");

	JakeBrazoIzq = Model();
	JakeBrazoIzq.LoadModel("Models/BrazoIzquierdoJake.obj");

	JakePiernaDer = Model();
	JakePiernaDer.LoadModel("Models/PiernaDerechaJake.obj");

	JakePiernaIzq = Model();
	JakePiernaIzq.LoadModel("Models/PiernaIzquierdaJake.obj");

	Prismo = Model();
	Prismo.LoadModel("Models/Prismo.obj");

	MarcelinesGuitar = Model();
	MarcelinesGuitar.LoadModel("Models/MarcelinesGuitar.obj");

	CoinReceptor = Model();
	CoinReceptor.LoadModel("Models/CoinReceptor.obj");

	JuegoDardos = Model();
	JuegoDardos.LoadModel("Models/JuegoDardos.obj");

	Dardo = Model();
	Dardo.LoadModel("Models/Dardo.obj");

	CasaMarceline = Model();
	CasaMarceline.LoadModel("Models/CasaMarceline.obj");

	FINN = Model();
	FINN.LoadModel("Models/Finn.obj");






	//Modelos Bob esponja
	Banca = Model();
	Banca.LoadModel("Models/Banca.obj");

	BancaCom = Model();
	BancaCom.LoadModel("Models/BancaCom.obj");

	LamparaBob = Model();
	LamparaBob.LoadModel("Models/LamparaBob.obj");

	CasaBob = Model();
	CasaBob.LoadModel("Models/CasaBob.obj");

	BobBrazoDer = Model();
	BobBrazoDer.LoadModel("Models/BobBrazoDer1.obj");

	BobBrazoIzq = Model();
	BobBrazoIzq.LoadModel("Models/BobBrazoIzq1.obj");

	BobCuerpo = Model();
	BobCuerpo.LoadModel("Models/BobCuerpo1.obj");

	BobPiernaDer = Model();
	BobPiernaDer.LoadModel("Models/BobPiernaDer1.obj");

	BobPiernaIzq = Model();
	BobPiernaIzq.LoadModel("Models/BobPiernaIzq1.obj");

	Cangre = Model();
	Cangre.LoadModel("Models/Cangre.obj");

	CasaCalam = Model();
	CasaCalam.LoadModel("Models/CasaCalam.obj");

	CasaPatricio = Model();
	CasaPatricio.LoadModel("Models/CasaPatricio.obj");

	CrustaceoCas = Model();
	CrustaceoCas.LoadModel("Models/CrustaceoCas.obj");

	HaroldN = Model();
	HaroldN.LoadModel("Models/HaroldN.obj");

	Jaula = Model();
	Jaula.LoadModel("Models/Jaula.obj");

	Pizzas = Model();
	Pizzas.LoadModel("Models/Pizzas.obj");

	Topo = Model();
	Topo.LoadModel("Models/Topo.obj");

	Bat = Model();
	Bat.LoadModel("Models/Bat.obj");

	Martillo = Model();
	Martillo.LoadModel("Models/Martillo.obj");

	Medusa1 = Model();
	Medusa1.LoadModel("Models/Medusa1.obj");

	Pelota = Model();
	Pelota.LoadModel("Models/Pelota.obj");

	Coin = Model();
	Coin.LoadModel("Models/Coin.obj");








	//Modelos Los padrinos magicos

	varita = Model();
	varita.LoadModel("Models/varita.obj");

	CasaTimmy = Model();
	CasaTimmy.LoadModel("Models/casa-timmy.obj");

	Cosmo = Model();
	Cosmo.LoadModel("Models/cosmo.obj");

	Wanda = Model();
	Wanda.LoadModel("Models/wanda.obj");

	Timmy = Model();
	Timmy.LoadModel("Models/timmy.obj");

	Bolos = Model();
	Bolos.LoadModel("Models/bowling.obj");

	Ball = Model();
	Ball.LoadModel("Models/ball.obj");

	Dice1 = Model();
	Dice1.LoadModel("Models/dado1.obj");

	Table = Model();
	Table.LoadModel("Models/mesa.obj");



	//Skybox
	std::vector<std::string> skyboxFacesDay;
	skyboxFacesDay.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFacesDay.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFacesDay.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFacesDay.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFacesDay.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFacesDay.push_back("Textures/Skybox/cupertin-lake_ft.tga");
	skyboxDay = Skybox(skyboxFacesDay);

	std::vector<std::string> skyboxFacesNight;
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_rt.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_lf.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_dn.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_up.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_bk.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_ft.tga");
	skyboxNight = Skybox(skyboxFacesNight);


	//



	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//Materiales nuevos creados
	Material Material_aluminio(3.0f, 512);
	Material Material_madera(0.3f, 8);
	Material Material_jake(0.6f, 32);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f, 0.0f); //Iluminación del sol //Dirección inicial


	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	unsigned int spotLightCount = 0;

	inicializarMedusas();

	// Inicializa el estado del martillo
	anguloMartillo = 0.0f;
	martilloEnMovimiento = false;
	martilloSubiendo = false;

	//inicializar variables bolos
	movBall = 50.0f;
	rotball = 0.0f;
	rotballOffset = 10.0f;
	movballOffset = 0.3f;

	//inicializacion variables dados
	movDiceY = 5.0f;
	movDiceX = 15.0f;
	movCos = 0.0f;

	//variables hadas animacion
	movHd = 4.2f;
	movOffset = 0.3f;
	movhdOffset = 0.05f;
	arrb = true;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		if (mainWindow.getCamaraAerea() == 0 && mainWindow.getJuego() == 0) {
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}





		actualizarAnimacionBateo(deltaTime);
		actualizarAnimaciones(deltaTime);
		actualizarAnimacionHacha(deltaTime);
		actualizarAnimacionDardo(deltaTime);
		animarBobEsponja(deltaTime);
		actualizarAnimacionHarold(deltaTime);

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 camPos = camera.getCameraPosition();

		// Manejar sonidos de atracciones
		manejarSonidosAtracciones(camera.getCameraPosition(), camera.getCameraDirection());

		if (mainWindow.getLantern() == 1)
		{
			//linterna
			spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
				0.0f, 2.0f,
				mainWindow.getPosX(), 20.0f, mainWindow.getPosZ(),
				0.0f, -1.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
				25.0f);
			spotLightCount++;//Encender lampara
		}
		else if (mainWindow.getLantern() == 0)
		{
			//linterna
			spotLights[0] = SpotLight(0.0f, 0.0f, 0.0f,
				0.0f, 2.0f,
				mainWindow.getPosX(), 20.0f, mainWindow.getPosZ(),
				0.0f, -1.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
				25.0f);
			spotLightCount--; //Apagar lampara
		}


		//información al shader de fuentes de iluminación


		//Camara aerea
		if (mainWindow.getCamaraAerea() == 1 && !respaldoHecho) {
			mainWindow.getCamaraPersona() == 0;

			// Respaldar la posición y ángulos actuales de la cámara
			camPositionBackup = camera.getCameraPosition();
			yawBackup = camera.getYaw();
			pitchBackup = camera.getPitch();
			respaldoHecho = true;

			// Cambiar posición a la aérea
			camera.setCameraPosition(glm::vec3(0.0f, 950.0f, 0.0f));
			camera.setYaw(-90.0f);
			camera.setPitch(-89.0f);

		}

		else if (mainWindow.getCamaraAerea() == 0 && respaldoHecho) {
			mainWindow.getCamaraPersona() == 1;

			camera.setCameraPosition(camPositionBackup);
			camera.setYaw(yawBackup);
			camera.setPitch(pitchBackup);
			respaldoHecho = false;

		}


		//Actualizar ángulo del sol
		sunAngle += sunSpeed * deltaTime;
		if (sunAngle > 360.0f)
			sunAngle -= 360.0f;
		float radians = glm::radians(sunAngle); //Convertir a radianes
		glm::vec3 direction = glm::vec3(0.0f, sin(radians), -cos(radians)); //Calcular dirección	
		pointLightCount = 0; //Reiniciar contador de luces puntuales

		//Cambio para que la transición sea más suave
		//Función dinamica de actualización encendido de luces


		if (direction.y > 0.2f) {
			skyboxDay.DrawSkybox(camera.calculateViewMatrix(), projection);

			spotLights[1] = SpotLight(0.0f, 0.0f, 0.0f,
				1.0f, 1.5f,
				-48.0f, 8.0f, -48.0f,
				0.5f, -1.0f, 0.5f,
				1.0f, 0.0f, 0.0f,
				50.0f);
			spotLightCount--;

			spotLights[2] = SpotLight(0.0f, 0.0f, 0.0f,
				1.0f, 1.5f,
				48.0f, 8.0f, -48.0f,
				-0.5f, -1.0f, 0.5f,
				1.0f, 0.0f, 0.0f,
				50.0f);
			spotLightCount--;

			spotLights[3] = SpotLight(0.0f, 0.0f, 0.0f,
				1.0f, 1.5f,
				-48.0f, 8.0f, 48.0f,
				0.5f, -1.0f, -0.5f,
				1.0f, 0.0f, 0.0f,
				50.0f);
			spotLightCount--;

			spotLights[4] = SpotLight(0.0f, 0.0f, 0.0f,
				1.0f, 1.5f,
				48.0f, 8.0f, 48.0f,
				-0.5f, -1.0f, -0.5f,
				1.0f, 0.0f, 0.0f,
				50.0f);
			spotLightCount--;

		}
		else if (direction.y <= 0.2f) {
			skyboxNight.DrawSkybox(camera.calculateViewMatrix(), projection);

			//Luces
			// Luz poste
			if (glm::distance(camPos, glm::vec3(-48.0f, 0.0f, -48.0f)) < 100.0f) {
				spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
					1.0f, 1.5f,
					-48.0f, 8.0f, -48.0f,
					0.5f, -1.0f, 0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount++;
			}
			else {
				spotLights[1] = SpotLight(0.0f, 0.0f, 0.0f,
					1.0f, 1.5f,
					-48.0f, 8.0f, -48.0f,
					0.5f, -1.0f, 0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount--;
			}

			//Luz poste
			if (glm::distance(camPos, glm::vec3(48.0f, 0.0f, -48.0f)) < 100.0f) {
				spotLights[2] = SpotLight(1.0f, 1.0f, 1.0f,
					1.0f, 1.5f,
					48.0f, 8.0f, -48.0f,
					-0.5f, -1.0f, 0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount++;
			}
			else {
				spotLights[2] = SpotLight(0.0f, 0.0f, 0.0f,
					1.0f, 1.5f,
					48.0f, 8.0f, -48.0f,
					-0.5f, -1.0f, 0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount--;
			}

			//Luz poste
			if (glm::distance(camPos, glm::vec3(-48.0f, 0.0f, 48.0f)) < 100.0f) {
				spotLights[3] = SpotLight(1.0f, 1.0f, 1.0f,
					1.0f, 1.5f,
					-48.0f, 8.0f, 48.0f,
					0.5f, -1.0f, -0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount++;
			}
			else {
				spotLights[3] = SpotLight(0.0f, 0.0f, 0.0f,
					1.0f, 1.5f,
					-48.0f, 8.0f, 48.0f,
					0.5f, -1.0f, -0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount--;
			}

			//Luz poste
			if (glm::distance(camPos, glm::vec3(48.0f, 0.0f, 48.0f)) < 100.0f) {
				spotLights[4] = SpotLight(1.0f, 1.0f, 1.0f,
					1.0f, 1.5f,
					48.0f, 8.0f, 48.0f,
					-0.5f, -1.0f, -0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount++;
			}
			else {
				spotLights[4] = SpotLight(0.0f, 0.0f, 0.0f,
					1.0f, 1.5f,
					48.0f, 8.0f, 48.0f,
					-0.5f, -1.0f, -0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount--;
			}




			if (mainWindow.getJuego() == 1) {
				//Iluminaciones para atracciones
				struct LucesAtraccion {
					glm::vec3 position;
				};

				std::vector<LucesAtraccion> atracciones = {
					{{-200.0f, 40.0f, 450.0}}, //Prismo
					{{-400.0f, 20.0f, 170.0}}, //Dardos
					{{-130.0f, 40.0f, -210.0}}, //Topo
					{{0.0f, 6.0f, -420.0}}, //Bate
				};

				for (int i = 0; i < atracciones.size() && pointLightCount < MAX_POINT_LIGHTS; ++i) {
					if (glm::distance(camPos, atracciones[i].position) < 100.0f) {
						pointLights[pointLightCount] = PointLight(1.0f, 1.0f, 1.0f,
							2.0f, 3.0f,
							atracciones[i].position.x, atracciones[i].position.y, atracciones[i].position.z,
							0.3f, 0.05f, 0.01f);
						pointLightCount++;
					}
				}
			}
		}


		//Luces activadas por boton varitas (deseos)

		if (mainWindow.getDeseo() == 1) {
			//Luces de las varitas
			struct VaritaLight {
				glm::vec3 position;
			};

			std::vector<VaritaLight> varitas = {
				{{320.0f, 55.f, 0.0f}},
				{{300.0f, 55.f, 499.0f}},
				{{-320.0f, 55.f, 0.0f}},
				{{-300.0f, 55.f, 499.0f}},
				{{0.0f,   55.f, 320.0f}},
				{{300.0f, 55.f, -320.0f}},
				{{-300.0f,55.f, -320.0f}},
				{{0.0f,   55.f, -499.0f}}
			};

			for (int i = 0; i < varitas.size() && pointLightCount < MAX_POINT_LIGHTS; ++i) {
				if (glm::distance(camPos, varitas[i].position) < 100.0f) {
					pointLights[pointLightCount] = PointLight(1.0f, 1.0f, 1.0f,
						2.0f, 3.0f,
						varitas[i].position.x, varitas[i].position.y, varitas[i].position.z,
						0.3f, 0.05f, 0.01f);
					pointLightCount++;
				}
			}
		}




		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);


		//Nueva implemantación cambio de luz de día a noche con interpolación

		mainLight.SetDirection(direction); //Actualizar dirección de la luz direccional

		float intensidadMin = 0.5f;
		float intensidadMax = 1.0f;

		// Clamp la dirección Y para que siempre esté entre 0 y 1
		float t = glm::clamp(direction.y, 0.0f, 1.0f);

		// Interpolación de luz según la posición del sol
		float ambient = glm::mix(intensidadMin, intensidadMax, t);
		float diffuse = glm::mix(intensidadMin, intensidadMax, t);

		mainLight.SetAmbientIntensity(ambient);
		mainLight.SetDiffuseIntensity(diffuse);

		//Esto ya estaba
		shaderList[0].SetDirectionalLight(&mainLight); //Mainlight para el sol
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//Piso principal escenario
		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(60.0f, 1.0f, 60.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Piso de cada isla

		//Isla Hora de aventura
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-300.0f, -0.8f, 300.0f));
		model = glm::scale(model, glm::vec3(20.0f, 1.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoH.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Isla padrinos magicos		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(300.0f, -0.8f, 300.0f));
		model = glm::scale(model, glm::vec3(20.0f, 1.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoL.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();


		//Isla Bob Esponja
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.8f, -300.0f));
		model = glm::scale(model, glm::vec3(20.0f, 1.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoB.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Kiosko 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.8f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoETexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();


		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.8f, -75.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.8f, 170.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 12.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.8f, 300.0f));
		model = glm::scale(model, glm::vec3(10.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(300.0f, -0.8f, -95.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 19.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-300.0f, -0.8f, -95.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 19.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(255.0f, -0.8f, -300.0f));
		model = glm::scale(model, glm::vec3(5.5f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-255.0f, -0.8f, -300.0f));
		model = glm::scale(model, glm::vec3(5.5f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();





		//Colocar modelos hora de aventura
		//Para animación backflip BMO
		static bool animacionIniciada = false;
		static bool animacionTerminada = false;
		static bool camaraEstabaLejos = true;
		static float tiempoInicioAnimacion = 0.0f;

		float tiempo = glfwGetTime();
		float anguloBackflip = 0.0f;
		float altura = 0.0f;


		if (glm::distance(camPos, glm::vec3(-300.0f, 0.0f, 400.0f)) <= 30.0f && camaraEstabaLejos) {
			animacionIniciada = false;
			animacionTerminada = false;
			camaraEstabaLejos = false; // Ya no está lejos
		}

		if (glm::distance(camPos, glm::vec3(-300.0f, 0.0f, 400.0f)) > 50.0f) {
			camaraEstabaLejos = true;
		}

		if (!animacionTerminada && glm::distance(camPos, glm::vec3(-300.0f, 0.0f, 400.0f)) <= 50.0f) {
			if (!animacionIniciada) {
				animacionIniciada = true;
				tiempoInicioAnimacion = tiempo;
			}

			float tiempoAnimacion = tiempo - tiempoInicioAnimacion;

			if (tiempoAnimacion <= 2.0f) {
				anguloBackflip = glm::radians(-360.0f * (tiempoAnimacion / 2.0f));
				altura = sin(tiempoAnimacion * glm::pi<float>()) * 2.0f;
			}
			else {
				animacionTerminada = true;
				anguloBackflip = 0.0f;
				altura = 0.0f;
			}
		}


		//BMO
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-300.0f, altura, 400.0f));
		model = glm::rotate(model, 2.5f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, anguloBackflip, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación para el backflip
		model = glm::scale(model, glm::vec3(60.0f, 60.0f, 60.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BMO.RenderModel();

		//Casa del arbol
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-400.0f, -0.2f, 400.0));
		model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 2.5f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0f, 7.0f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaDelArbol.RenderModel();

		if (mainWindow.getCamaraPersona() == 1 && mainWindow.getCamaraAerea() == 0 && mainWindow.getJuego() == 0) {
			//PARA LA ANIMACIÓN DE JAKE
			mainWindow.actualizarAnimacionJake(camera.getYaw());


			// Ajusta la cámara para seguir a Jake desde atrás
			float jakeX = mainWindow.getPosX();
			float jakeZ = mainWindow.getPosZ();
			float jakeYaw = mainWindow.getDireccion();

			float distancia = 15.0f;
			float offsetX = sin(glm::radians(jakeYaw)) * -distancia;
			float offsetZ = cos(glm::radians(jakeYaw)) * -distancia;

			glm::vec3 camP(jakeX + offsetX, 20.0f, jakeZ + offsetZ);
			camera.setCameraPosition(camP);
			camera.setFront(glm::normalize(glm::vec3(jakeX, 3.2f, jakeZ) - camP));


			//Jake el perro
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(mainWindow.getPosX(), 3.2f, mainWindow.getPosZ()));
			model = glm::rotate(model, glm::radians(mainWindow.getDireccion()), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f));
			glm::mat4 modelJake = model;
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeCuerpo.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(-1.0f, 2.0f, 0.0f));
			model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::rotate(model, glm::radians(mainWindow.getBrazoDerAng()), glm::vec3(0.0f, 0.0f, 1.0f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeBrazoDer.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(0.95f, 2.0f, 0.0f));
			model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::rotate(model, -glm::radians(mainWindow.getBrazoIzqAng()), glm::vec3(0.0f, 0.0f, 1.0f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeBrazoIzq.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(-0.38f, 0.2f, -0.03f));
			model = glm::rotate(model, glm::radians(mainWindow.getPiernaDerAng()), glm::vec3(1.0f, 0.0f, 0.0f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakePiernaDer.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(0.51f, 0.2f, -0.03f));
			model = glm::rotate(model, glm::radians(mainWindow.getPiernaIzqAng()), glm::vec3(1.0f, 0.0f, 0.0f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakePiernaIzq.RenderModel();


		}






		//Jake el perro
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(mainWindow.getPosX(), 3.2f, mainWindow.getPosZ()));
		model = glm::rotate(model, glm::radians(mainWindow.getDireccion()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		glm::mat4 modelJake = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));


		model = modelJake;
		model = glm::translate(model, glm::vec3(-1.0f, 2.0f, 0.0f));
		model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getBrazoDerAng()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));


		model = modelJake;
		model = glm::translate(model, glm::vec3(0.95f, 2.0f, 0.0f));
		model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, -glm::radians(mainWindow.getBrazoIzqAng()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));


		model = modelJake;
		model = glm::translate(model, glm::vec3(-0.38f, 0.2f, -0.03f));
		model = glm::rotate(model, glm::radians(mainWindow.getPiernaDerAng()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));


		model = modelJake;
		model = glm::translate(model, glm::vec3(0.51f, 0.2f, -0.03f));
		model = glm::rotate(model, glm::radians(mainWindow.getPiernaIzqAng()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));



		//Prismo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-200.0f, -1.4f, 470.0));
		model = glm::scale(model, glm::vec3(23.0f, 23.0f, 23.0f));
		Material_madera.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Prismo.RenderModel();


		//AQUÍ DEBEN DE PONER LAS UBICACIONES DE DONDE VAN A QUERER LOS RECIBIDORES DE MONEDAS (SOLO EDITEN X y Z)
		//Recibidor de monedas
		RenderRecibidorMonedas(glm::vec3(-220.0f, -0.9f, 419.0f), 0.0f, uniformModel); //Tirar hacha 
		RenderRecibidorMonedas(glm::vec3(-390.0f, -0.9f, 180.0f), 0.5f, uniformModel); //Juego de dardos
		RenderRecibidorMonedas(glm::vec3(15.0f, -0.9f, -395.0f), 1.0f, uniformModel); //Bateo
		RenderRecibidorMonedas(glm::vec3(-125.0f, -0.9f, -200.0f), 2.0f, uniformModel); //Golpear al topo
		RenderRecibidorMonedas(glm::vec3(50.0f, -0.9f, 180.0f), 2.0f, uniformModel); //Bolos
		RenderRecibidorMonedas(glm::vec3(130.0f, -0.9f, 70.0f), 2.0f, uniformModel); //Dados

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-400.0f, -0.9f, 150.0));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JuegoDardos.RenderModel();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-150.0f, 0.0f, 150.0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_madera.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaMarceline.RenderModel();



		//FINN

		anguloGiroFinn += deltaTime * 20.0f; // Ajusta la velocidad
		if (anguloGiroFinn > 360.0f) anguloGiroFinn -= 360.0f;
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-300.0f, -1.0f, 150.0f));
		model = glm::rotate(model, glm::radians(anguloGiroFinn), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		FINN.RenderModel();








		//Colocar modelos Bob esponja
		//BOB ESPONJA
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 8.8f + saltoAltura, posicionZ));
		model = glm::rotate(model, rotacionY, glm::vec3(0.0f, 1.0f, 0.0f)); // Aplicar rotación según dirección
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BobCuerpo.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.9f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-brazoAngulo), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BobBrazoDer.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.9f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(brazoAngulo), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BobBrazoIzq.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.8f, -2.5f, 0.0f));
		model = glm::rotate(model, glm::radians(piernaAngulo), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BobPiernaDer.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.8f, -2.5f, 0.0f));
		model = glm::rotate(model, glm::radians(-piernaAngulo), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BobPiernaIzq.RenderModel();

		//CASAS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(140.0f, 0.0f, -420.0));
		model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaBob.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-130.0f, 0.0f, -450.0));
		model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.8f, 2.8f, 2.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaCalam.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-130.0f, -3.0f, -140.0));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaPatricio.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(160.0f, -4.0f, -160.0));
		model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.3f, 2.3f, 2.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CrustaceoCas.RenderModel();

		//TOPO
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-130.0f, 0.0f, -210.0));
		model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.3f, 2.3f, 2.3f));
		modelaux = model;
		Material_aluminio.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Topo.RenderModel();






		//JAULA 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, -420.0));
		model = glm::scale(model, glm::vec3(2.3f, 2.3f, 2.3f));
		modelaux = model;
		Material_aluminio.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Jaula.RenderModel();



		//PIZZAS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-170.0f, 13.0f, -340.0));
		model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.32f, 0.32f, 0.32f));
		Material_aluminio.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pizzas.RenderModel();

		//CANGREBURGUERS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, 9.4f, -120.0));
		model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.3f, 1.3f, 1.3f));
		Material_aluminio.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cangre.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(haroldPosX, 10.3f, -345.0f));
		model = glm::rotate(model, haroldMovingForward ? 0.0f : glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación según dirección
		model = glm::scale(model, glm::vec3(1.6f, 1.6f, 1.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HaroldN.RenderModel();


		//Bancas
		RenderBanca(glm::vec3(-48.0f, -1.0f, 25.0f), -1.6f, uniformModel);
		RenderBanca(glm::vec3(48.0f, -1.0f, 25.0f), 1.6f, uniformModel);
		RenderBanca(glm::vec3(-48.0f, -1.0f, -25.0f), -1.6f, uniformModel);
		RenderBanca(glm::vec3(48.0f, -1.0f, -25.0f), 1.6f, uniformModel);



		//Mesas de comer
		RenderBancaCom(glm::vec3(30.0f, -1.0f, -40.0f), 0.0f, uniformModel);
		RenderBancaCom(glm::vec3(-30.0f, -1.0f, -40.0f), 0.0f, uniformModel);
		RenderBancaCom(glm::vec3(30.0f, -1.0f, 40.0f), 0.0f, uniformModel);
		RenderBancaCom(glm::vec3(-30.0f, -1.0f, 40.0f), 0.0f, uniformModel);



		//Lamparas
		//En kiosko
		RenderLamparaBob(glm::vec3(-49.0f, -0.8f, -49.0f), -0.5f, uniformModel);
		RenderLamparaBob(glm::vec3(49.0f, -0.8f, -49.0f), 3.5f, uniformModel);
		RenderLamparaBob(glm::vec3(-49.0f, -0.8f, 49.0f), 0.5f, uniformModel);
		RenderLamparaBob(glm::vec3(49.0f, -0.8f, 49.0f), 2.5f, uniformModel);



		//Coins
		static bool monedaAnimando = false;
		static float alturaMoneda = 3.0f;
		static float rotacionCoin = 0.0f;







		//Para activar el hacha
		if (glm::distance(camPos, glm::vec3(-220.0f, alturaMoneda, 419.0f)) < 100.0f && mainWindow.getJuego() == 1) {
			mainWindow.getCamaraPersona() == 0;

			if (!monedaAnimando && alturaMoneda == 3.0f) {
				monedaAnimando = true;
			}

			// Si se está animando, baja y gira
			if (monedaAnimando) {
				alturaMoneda -= deltaTime * 0.01f; // Velocidad de caída
				rotacionCoin += deltaTime * glm::radians(360.0f) * 0.001f; // Giro

				if (alturaMoneda <= 0.0f) {
					alturaMoneda = 3.0f;
					rotacionCoin = 0.0f;
					monedaAnimando = false;
				}
			}

			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-220.0f, alturaMoneda, 419.0f));
			model = glm::rotate(model, 1.5f, glm::vec3(0.0f, 1, 0.0f));
			model = glm::rotate(model, rotacionCoin, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.1f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Coin.RenderModel();

			//AQUÍ COLOCAR TODO LO DE LA ANIMACIÓN DEL HACHA, PONER UN RETRASO DE 2 SEGUNDOA PARA QUE EL JUEGO SE ACTIVE


			//Jake el perro 
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-190.0f, 3.7f, 400.0f));
			model = glm::scale(model, glm::vec3(3.0f));
			modelJake = model;
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeCuerpo.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(-1.0f, 2.0f, 0.0f));
			model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
			// Rotación para la animación del lanzamiento
			model = glm::rotate(model, glm::radians(anguloBrazo), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::rotate(model, 3.57f, glm::vec3(0.0f, 0.0f, 1.0f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeBrazoDer.RenderModel();

			if (!hachaLanzada && !hachaEnPared) {
				model = glm::translate(model, glm::vec3(-1.7f, 0.0f, 0.0));
				model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, 1.57f, glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(50.5f, 50.5f, 50.5f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				MarcelinesGuitar.RenderModel();
			}
			else {
				// Hacha en vuelo o clavada en la pared
				model = glm::mat4(1.0);

				// Si está en vuelo, la posición irá cambiando pero alineada con el lanzamiento
				if (hachaLanzada && !hachaEnPared) {
					// Durante el vuelo, mantenemos la dirección pero con la posición Z cambiante
					model = glm::translate(model, glm::vec3(-190.0f, 3.7f + 2.0f, posHachaZ));
					model = glm::rotate(model, glm::radians(anguloHacha), glm::vec3(0.0f, 0.0f, 1.0f));
				}
				// Si está clavada en la pared, usamos la posición X e Y aleatorias generadas
				else if (hachaEnPared) {
					model = glm::translate(model, glm::vec3(posHachaX, posHachaY, posHachaZ));
					// Usamos el ángulo fijo predeterminado al inicio del lanzamiento
					model = glm::rotate(model, glm::radians(anguloHachaFijo), glm::vec3(0.0f, 0.0f, 1.0f));
				}

				model = glm::scale(model, glm::vec3(3.0f * 50.5f, 3.0f * 50.5f, 3.0f * 50.5f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				MarcelinesGuitar.RenderModel();
			}

			model = modelJake;
			model = glm::translate(model, glm::vec3(1.05f, 2.0f, 0.0f));
			model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 0.0f, 1.0f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeBrazoIzq.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(-0.38f, 0.2f, -0.03f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakePiernaDer.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(0.51f, 0.2f, -0.03f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakePiernaIzq.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(1.05f, 2.0f, 0.0f));
			model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 0.0f, 1.0f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeBrazoIzq.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(-0.38f, 0.2f, -0.03f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakePiernaDer.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(0.51f, 0.2f, -0.03f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakePiernaIzq.RenderModel();


		}
		else {
			mainWindow.getCamaraPersona() == 1;
		}
		if (glm::distance(camPos, glm::vec3(-220.0f, alturaMoneda, 419.0f)) < 100.0f && mainWindow.getJuego() == 1 && !camaraJuegoActiva) {
			//Camara de juego
			camPositionBackup = camera.getCameraPosition();
			yawBackup = camera.getYaw();
			pitchBackup = camera.getPitch();

			// Posición fija de la cámara de juego (ej. aérea)
			camera.setCameraPosition(glm::vec3(-200.0f, 30.0f, 380.0f));
			camera.setYaw(-270.0f);//Giro en x
			camera.setPitch(-30.0f);//Giro en z

			camaraJuegoActiva = true;
		}
		else if (glm::distance(camPos, glm::vec3(-220.0f, alturaMoneda, 419.0f)) < 100.0 && mainWindow.getJuego() == 0 && camaraJuegoActiva) {
			camera.setCameraPosition(camPositionBackup);
			camera.setYaw(yawBackup);
			camera.setPitch(pitchBackup);
			camaraJuegoActiva = false;
		}





















		//Para activar el juego de dardos
		if (glm::distance(camPos, glm::vec3(-390.0f, alturaMoneda, 182.0f)) < 50.0f && mainWindow.getJuego() == 1) {
			mainWindow.getCamaraPersona() == 0;

			if (!monedaAnimando && alturaMoneda == 3.0f) {
				monedaAnimando = true;
			}

			// Si se está animando, baja y gira
			if (monedaAnimando) {
				alturaMoneda -= deltaTime * 0.01f; // Velocidad de caída
				rotacionCoin += deltaTime * glm::radians(360.0f) * 0.001f; // Giro

				if (alturaMoneda <= 0.0f) {
					alturaMoneda = 3.0f;
					rotacionCoin = 0.0f;
					monedaAnimando = false;
				}
			}

			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-390.0f, alturaMoneda, 182.0f));
			model = glm::rotate(model, 1.5f, glm::vec3(0.0f, 1, 0.0f));
			model = glm::rotate(model, rotacionCoin, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.1f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Coin.RenderModel();



			//AQU� COLOCAR TODO LO DE LA ANIMACI�N DEL DARDO, PONER UN RETRASO DE 2 SEGUNDOA PARA QUE EL JUEGO SE ACTIVE

			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-400.0f, 3.7f, 200.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Ahora mira hacia -Z
			model = glm::scale(model, glm::vec3(3.0f));
			modelJake = model;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeCuerpo.RenderModel();

			// Brazo derecho lanzando
			model = modelJake;
			model = glm::translate(model, glm::vec3(-1.0f, 2.0f, 0.0f));
			model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(anguloBrazo), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::rotate(model, 3.57f, glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeBrazoDer.RenderModel();

			// Dardo en la mano
			if (!dardoLanzada && !dardoEnPared) {
				model = glm::translate(model, glm::vec3(-1.2f, 0.0f, 0.2f));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(3.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Dardo.RenderModel();
			}
			// Dardo lanzado o clavado
			else {
				model = glm::mat4(1.0);

				if (dardoLanzada && !dardoEnPared) {
					model = glm::translate(model, glm::vec3(-400.0f, 5.7f, posDardoZ)); // eje Z NEGATIVO
					model = glm::rotate(model, glm::radians(angulodardo), glm::vec3(0.0f, 0.0f, 1.0f)); // giro sobre eje Z
				}
				else if (dardoEnPared) {
					model = glm::translate(model, glm::vec3(posDardoX, posDardoY, posDardoZ));
					model = glm::rotate(model, glm::radians(anguloDardoFijo), glm::vec3(0.0f, 0.0f, 1.0f));
				}

				model = glm::scale(model, glm::vec3(20.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Dardo.RenderModel();
			}

			// Brazo izquierdo
			model = modelJake;
			model = glm::translate(model, glm::vec3(1.05f, 2.0f, 0.0f));
			model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeBrazoIzq.RenderModel();

			// Pierna derecha
			model = modelJake;
			model = glm::translate(model, glm::vec3(-0.38f, 0.2f, -0.03f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakePiernaDer.RenderModel();

			// Pierna izquierda
			model = modelJake;
			model = glm::translate(model, glm::vec3(0.51f, 0.2f, -0.03f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakePiernaIzq.RenderModel();

			//AQUÍ COLOCAR TODO LO DE LA ANIMACIoN DEL HACHA, PONER UN RETRASO DE 2 SEGUNDOA PARA QUE EL JUEGO SE ACTIVE


		}
		else {
			mainWindow.getCamaraPersona() == 1;
		}


		if (glm::distance(camPos, glm::vec3(-390.0f, alturaMoneda, 182.0f)) < 100.0f && mainWindow.getJuego() == 1 && !camaraJuegoActiva) {
			//Camara de juego
			camPositionBackup = camera.getCameraPosition();
			yawBackup = camera.getYaw();
			pitchBackup = camera.getPitch();

			// Posici�n fija de la c�mara de juego (ej. a�rea)
			camera.setCameraPosition(glm::vec3(-400.0f, 30.0f, 220.0f));
			camera.setYaw(-90.0f);//Giro en x
			camera.setPitch(-30.0f);//Giro en z

			camaraJuegoActiva = true;
		}
		else if (glm::distance(camPos, glm::vec3(-390.0f, alturaMoneda, 182.0f)) < 100.0 && mainWindow.getJuego() == 0 && camaraJuegoActiva) {
			camera.setCameraPosition(camPositionBackup);
			camera.setYaw(yawBackup);
			camera.setPitch(pitchBackup);
			camaraJuegoActiva = false;
		}






















		//Para activar el bateo
		if (glm::distance(camPos, glm::vec3(15.0f, alturaMoneda, -395.0f)) < 100.0f && mainWindow.getJuego() == 1) {

			if (!monedaAnimando && alturaMoneda == 3.0f) {
				monedaAnimando = true;
			}

			// Si se está animando, baja y gira
			if (monedaAnimando) {
				alturaMoneda -= deltaTime * 0.01f; // Velocidad de caída
				rotacionCoin += deltaTime * glm::radians(360.0f) * 0.001f; // Giro

				if (alturaMoneda <= 0.0f) {
					alturaMoneda = 3.0f;
					rotacionCoin = 0.0f;
					monedaAnimando = false;
				}
			}

			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(15.0f, alturaMoneda, -395.0f));
			model = glm::rotate(model, 1.5f, glm::vec3(0.0f, 1, 0.0f));
			model = glm::rotate(model, rotacionCoin, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.1f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Coin.RenderModel();



			//AQUÍ COLOCAR TODO LO DE LA ANIMACIÓN DEL BATEO, PONER UN RETRASO DE 2 SEGUNDOA PARA QUE EL JUEGO SE ACTIVE


			if (pelotaEnMovimiento) {
				model = glm::mat4(1.0);
				model = glm::translate(model, glm::vec3(-23.0f + posicionPelota.x, 10.0f + posicionPelota.y, -421.5f + posicionPelota.z));
				model = glm::scale(model, glm::vec3(3.3f, 3.3f, 3.3f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Pelota.RenderModel();
			}
			else {
				// Si la pelota no está en movimiento, mostrarla en la máquina lanzadora
				model = modelaux;
				model = glm::translate(model, glm::vec3(-23.0f, -0.3f, -1.5));
				model = glm::scale(model, glm::vec3(3.3f, 3.3f, 3.3f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Pelota.RenderModel();
			}

			//Jake el perro 
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(58.0f, 3.7f, -433.0f));
			model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f));
			modelJake = model;
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeCuerpo.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(-1.0f, 2.0f, 0.0f));
			model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeBrazoDer.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(0.95f, 2.0f, 0.0f));
			// Aplicar rotación para el golpe en el brazo izquierdo - cambiado a rotación en Y para movimiento horizontal
			model = glm::rotate(model, glm::radians(anguloGolpe), glm::vec3(0.0f, 1.0f, 0.0f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeBrazoIzq.RenderModel();

			model = glm::translate(model, glm::vec3(1.5f, -0.2f, 0.0));
			model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Bat.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(-0.38f, 0.2f, -0.03f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakePiernaDer.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(0.51f, 0.2f, -0.03f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakePiernaIzq.RenderModel();



		}
		else {
			mainWindow.getCamaraPersona() == 1;
		}
		if (glm::distance(camPos, glm::vec3(15.0f, alturaMoneda, -395.0f)) < 100.0f && mainWindow.getJuego() == 1 && !camaraJuegoActiva) {
			//Camara de juego
			camPositionBackup = camera.getCameraPosition();
			yawBackup = camera.getYaw();
			pitchBackup = camera.getPitch();

			// Posición fija de la cámara de juego (ej. aérea)
			camera.setCameraPosition(glm::vec3(85.0f, 30.0f, -420.0f));
			camera.setYaw(-180.0f);
			camera.setPitch(-30.0f);

			camaraJuegoActiva = true;
		}
		else if (glm::distance(camPos, glm::vec3(15.0f, alturaMoneda, -395.0f)) < 100.0 && mainWindow.getJuego() == 0 && camaraJuegoActiva) {
			camera.setCameraPosition(camPositionBackup);
			camera.setYaw(yawBackup);
			camera.setPitch(pitchBackup);
			camaraJuegoActiva = false;
		}




		//Para activar golpear al topo
		if (glm::distance(camPos, glm::vec3(-125.0f, alturaMoneda, -200.0f)) < 100.0f && mainWindow.getJuego() == 1) {

			mainWindow.getCamaraPersona() == 0;

			if (!monedaAnimando && alturaMoneda == 3.0f) {
				monedaAnimando = true;
			}

			// Si se está animando, baja y gira
			if (monedaAnimando) {
				alturaMoneda -= deltaTime * 0.01f; // Velocidad de caída
				rotacionCoin += deltaTime * glm::radians(360.0f) * 0.001f; // Giro

				if (alturaMoneda <= 0.0f) {
					alturaMoneda = 3.0f;
					rotacionCoin = 0.0f;
					monedaAnimando = false;
				}
			}

			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-125.0f, alturaMoneda, -200.0f));
			model = glm::rotate(model, 1.5f, glm::vec3(0.0f, 1, 0.0f));
			model = glm::rotate(model, rotacionCoin, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.1f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Coin.RenderModel();


			//AQUÍ COLOCAR TODO LO DE LA ANIMACIÓN DEL TOPO, PONER UN RETRASO DE 2 SEGUNDOA PARA QUE EL JUEGO SE ACTIVE

			//TOPO
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-130.0f, 0.0f, -210.0));
			model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(2.3f, 2.3f, 2.3f));
			modelaux = model;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Topo.RenderModel();

			glm::vec3 posicionesMedusas[5] = {
				glm::vec3(1.3f, 0.0f, 0.4f),    // Medusa 1
				glm::vec3(-0.1f, 0.0f, 0.4f),   // Medusa 2
				glm::vec3(-1.4f, 0.0f, 0.4f),   // Medusa 3
				glm::vec3(-1.0f, 0.0f, -1.1f),  // Medusa 4
				glm::vec3(0.8f, 0.0f, -1.1f)    // Medusa 5
			};

			// Renderiza cada medusa con su animación correspondiente
			for (int i = 0; i < 5; i++) {
				if (estadoMedusas[i].visible) {
					model = modelaux;
					// Aplica la altura animada a la posición Y
					model = glm::translate(model, glm::vec3(
						posicionesMedusas[i].x,
						estadoMedusas[i].altura,  // Altura animada
						posicionesMedusas[i].z
					));

					// Si fue golpeada, aplastar la medusa (escalar en Y)
					if (estadoMedusas[i].golpeada) {
						float factorAplastamiento = 1.0f - (estadoMedusas[i].tiempoGolpeada * 1.5f);
						if (factorAplastamiento < 0.2f) factorAplastamiento = 0.2f;

						model = glm::scale(model, glm::vec3(0.4f, 0.4f * factorAplastamiento, 0.4f));

						// Si está golpeada, también le damos un efecto de ensanchamiento lateral
						model = glm::scale(model, glm::vec3(1.0f + (1.0f - factorAplastamiento), 1.0f, 1.0f + (1.0f - factorAplastamiento)));
					}
					else {
						model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
					}

					// Agrega un pequeño balanceo para más realismo cuando está visible
					if (!estadoMedusas[i].golpeada && estadoMedusas[i].altura > estadoMedusas[i].alturaInicial + 1.0f) {
						float balanceo = sin(glfwGetTime() * 2.0f + i * 1.5f) * 0.1f;
						model = glm::rotate(model, balanceo, glm::vec3(0.0f, 0.0f, 1.0f));
					}

					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					Medusa1.RenderModel();
				}
			}

			//Jake el perro 
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-120.0f, 3.7f, -210.0f));
			model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f));
			modelJake = model;
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeCuerpo.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(-1.0f, 2.0f, 0.0f));
			model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::rotate(model, -1.57f, glm::vec3(1.0f, 0.0f, 0.0f));
			// Aquí aplicamos la rotación del martillo
			model = glm::rotate(model, glm::radians(anguloMartillo), glm::vec3(0.0f, 0.0f, 1.0f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeBrazoDer.RenderModel();

			model = glm::translate(model, glm::vec3(-2.0f, -0.1f, 0.0));
			model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			Material_madera.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Martillo.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(1.05f, 2.0f, 0.0f));
			model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 0.0f, 1.0f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeBrazoIzq.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(-0.38f, 0.2f, -0.03f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakePiernaDer.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(0.51f, 0.2f, -0.03f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakePiernaIzq.RenderModel();

			// Sistema de colisiones básico para detectar si golpeamos una medusa
			if (martilloEnMovimiento && !martilloSubiendo) {
				// Calculamos la posición del martillo basada en el ángulo actual
				float anguloRad = glm::radians(anguloMartillo);

				// Posición aproximada del martillo en el mundo (calculada con la rotación)
				glm::vec3 posMartillo = glm::vec3(
					-120.0f - 3.0f + sin(anguloRad) * 2.0f,  // Ajuste en X según rotación
					3.7f + 2.0f - cos(anguloRad) * 2.0f,     // Ajuste en Y según rotación
					-210.0f
				);

				// Radio de colisión del martillo
				float radioMartillo = 4.0f;

				// Verificar colisión con cada medusa visible
				for (int i = 0; i < 5; i++) {
					if (estadoMedusas[i].visible && !estadoMedusas[i].golpeada &&
						estadoMedusas[i].altura > estadoMedusas[i].alturaInicial + 1.0f) {

						// Posición de la medusa en el mundo
						glm::vec3 posMedusa = glm::vec3(
							-130.0f + posicionesMedusas[i].x * 2.3f,
							estadoMedusas[i].altura * 2.3f,
							-210.0f + posicionesMedusas[i].z * 2.3f
						);

						// Distancia entre el martillo y la medusa
						float distancia = glm::distance(posMartillo, posMedusa);

						// Si hay colisión, marcar la medusa como golpeada
						if (distancia < radioMartillo) {
							estadoMedusas[i].golpeada = true;
							estadoMedusas[i].tiempoGolpeada = 0.0f;
							// Aquí podrías agregar efectos de sonido o incrementar la puntuación
						}
					}

					// Procesar medusas golpeadas (animación de "aplastamiento")
					if (estadoMedusas[i].golpeada) {
						estadoMedusas[i].tiempoGolpeada += deltaTime;

						// Después de un tiempo, hacer que desaparezca
						if (estadoMedusas[i].tiempoGolpeada > 0.3f) {
							estadoMedusas[i].altura -= estadoMedusas[i].velocidad * 0.5f * deltaTime;  // Desaparece más rápido

							if (estadoMedusas[i].altura <= estadoMedusas[i].alturaInicial) {
								estadoMedusas[i].altura = estadoMedusas[i].alturaInicial;
								estadoMedusas[i].visible = false;
								estadoMedusas[i].golpeada = false;
							}
						}
					}
				}
			}



		}
		else {
			mainWindow.getCamaraPersona() == 1;
		}

		if (glm::distance(camPos, glm::vec3(-125.0f, alturaMoneda, -200.0f)) < 100.0f && mainWindow.getJuego() == 1 && !camaraJuegoActiva) {
			//Camara de juego
			camPositionBackup = camera.getCameraPosition();
			yawBackup = camera.getYaw();
			pitchBackup = camera.getPitch();

			// Posición fija de la cámara de juego (ej. aérea)
			camera.setCameraPosition(glm::vec3(-100, 30.0f, -210.0f));
			camera.setYaw(-180.0f);//Giro en x
			camera.setPitch(-30.0f);//Giro en z

			camaraJuegoActiva = true;
		}

		else if (glm::distance(camPos, glm::vec3(-125.0f, alturaMoneda, -200.0f)) < 100.0 && mainWindow.getJuego() == 0 && camaraJuegoActiva) {
			camera.setCameraPosition(camPositionBackup);
			camera.setYaw(yawBackup);
			camera.setPitch(pitchBackup);
			camaraJuegoActiva = false;
		}









		//Colocar modelos Los padrinos magicos

		//Varitas
		RenderVarita(glm::vec3(320.0f, -0.8f, 0.0f), 0.0f, uniformModel);
		RenderVarita(glm::vec3(300.0f, -0.8f, 499.0f), 1.5f, uniformModel);


		RenderVarita(glm::vec3(-320.0f, -0.8f, 0.0f), 0.0f, uniformModel);
		RenderVarita(glm::vec3(-300.0f, -0.8f, 499.0f), 1.5f, uniformModel);

		RenderVarita(glm::vec3(0.0f, -0.8f, 320.0f), 1.5f, uniformModel);


		RenderVarita(glm::vec3(300.0f, -0.8f, -320.0f), 1.5f, uniformModel);
		RenderVarita(glm::vec3(-300.0f, -0.8f, -320.0f), 1.5f, uniformModel);
		RenderVarita(glm::vec3(0.0f, -0.8f, -499.0f), 1.5f, uniformModel);

		//Casa Timmy
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(300.0f, 2.0f, 300.0));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaTimmy.RenderModel();

		//animacion hadas
		
		if (arrb) {
			if (movCos < 3.0f)
			{
				movCos += movhdOffset * deltaTime;
				//printf("avanza%f \n ",movCoche);
			}
			else
			{
				arrb = false;
			}
		}
		else {
			if (movCos > 1.0f)
			{
				movCos -= movhdOffset * deltaTime;
				//printf("avanza%f \n ",movCoche);
			}
			else
			{
				arrb = true;
			}
		}
		

		
		
		
		

		//Cosmo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(300.0f, movCos+6.0f, 240.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cosmo.RenderModel();

		//Wanda
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(301.0f, 8.2f, 240.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Wanda.RenderModel();

		//Timmy
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(301.0f, 0.2f, 240.0));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Timmy.RenderModel();

		//Mesa Dados
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(150.0f, 10.5f, 100.0f));
		modelaux = model;
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Table.RenderModel();

		//Para activar dados
		if (glm::distance(camPos, glm::vec3(130.0f, alturaMoneda, 70.0f)) < 100.0f && mainWindow.getJuego() == 1) {

			if (!monedaAnimando && alturaMoneda == 3.0f) {
				monedaAnimando = true;
			}

			// Si se está animando, baja y gira
			if (monedaAnimando) {
				alturaMoneda -= deltaTime * 0.01f; // Velocidad de caída
				rotacionCoin += deltaTime * glm::radians(360.0f) * 0.001f; // Giro

				if (alturaMoneda <= 0.0f) {
					alturaMoneda = 3.0f;
					rotacionCoin = 0.0f;
					monedaAnimando = false;
				}
			}

			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(130.0f, alturaMoneda, 70.0f));
			model = glm::rotate(model, 1.5f, glm::vec3(0.0f, 1, 0.0f));
			model = glm::rotate(model, rotacionCoin, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.1f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Coin.RenderModel();



			//AQUÍ COLOCAR TODO LO DE LA ANIMACIÓN DE DADOS, PONER UN RETRASO DE 2 SEGUNDOA PARA QUE EL JUEGO SE ACTIVE


			//animacion dados
			if (movDiceY > 0.5f)
			{
				movDiceY -= movOffset * deltaTime;
				//printf("avanza%f \n ",movCoche);
				rotball += rotballOffset * deltaTime;
			}
			else
			{
				movDiceY = 0.5f;
			}

			if (movDiceX > -10.5f)
			{
				movDiceX -= movOffset * deltaTime;
				//printf("avanza%f \n ",movCoche);
			}
			else
			{
				movDiceY = 5.0f;
				movDiceX = 15.0f;
			}

			//Dice1
			model = modelaux;
			model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(movDiceX, movDiceY, 0.0f));
			model = glm::rotate(model, rotball * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
			//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Dice1.RenderModel();

			//Jake el perro 
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(150.0f, 3.7f, 70.0f));
			//model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f));
			modelJake = model;
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeCuerpo.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(-1.0f, 2.0f, 0.0f));
			model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeBrazoDer.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(0.95f, 2.0f, 0.0f));
			// Aplicar rotación para el golpe en el brazo izquierdo - cambiado a rotación en Y para movimiento horizontal
			model = glm::rotate(model, glm::radians(anguloGolpe), glm::vec3(0.0f, 1.0f, 0.0f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeBrazoIzq.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(-0.38f, 0.2f, -0.03f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakePiernaDer.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(0.51f, 0.2f, -0.03f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakePiernaIzq.RenderModel();



		}
		else {
			mainWindow.getCamaraPersona() == 1;
		}
		if (glm::distance(camPos, glm::vec3(130.0f, alturaMoneda, 70.0f)) < 100.0f && mainWindow.getJuego() == 1 && !camaraJuegoActiva) {
			//Camara de juego
			camPositionBackup = camera.getCameraPosition();
			yawBackup = camera.getYaw();
			pitchBackup = camera.getPitch();

			// Posición fija de la cámara de juego (ej. aérea)
			camera.setCameraPosition(glm::vec3(150.0f, 60.0f, 20.0f));
			camera.setYaw(-270.0f);
			camera.setPitch(-30.0f);

			camaraJuegoActiva = true;
		}
		else if (glm::distance(camPos, glm::vec3(130.0f, alturaMoneda, 70.0f)) < 100.0 && mainWindow.getJuego() == 0 && camaraJuegoActiva) {
			camera.setCameraPosition(camPositionBackup);
			camera.setYaw(yawBackup);
			camera.setPitch(pitchBackup);
			camaraJuegoActiva = false;
		}

		

		//Bolos
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, 10.0f, 300.0));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		//model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Bolos.RenderModel();

		//Para activar bolos
		if (glm::distance(camPos, glm::vec3(50.0f, alturaMoneda, 180.0f)) < 100.0f && mainWindow.getJuego() == 1) {

			if (!monedaAnimando && alturaMoneda == 3.0f) {
				monedaAnimando = true;
			}

			// Si se está animando, baja y gira
			if (monedaAnimando) {
				alturaMoneda -= deltaTime * 0.01f; // Velocidad de caída
				rotacionCoin += deltaTime * glm::radians(360.0f) * 0.001f; // Giro

				if (alturaMoneda <= 0.0f) {
					alturaMoneda = 3.0f;
					rotacionCoin = 0.0f;
					monedaAnimando = false;
				}
			}

			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(50.0f, alturaMoneda, 180.0f));
			model = glm::rotate(model, 1.5f, glm::vec3(0.0f, 1, 0.0f));
			model = glm::rotate(model, rotacionCoin, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.1f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Coin.RenderModel();



			//AQUÍ COLOCAR TODO LO DE LA ANIMACIÓN DE BOLOS, PONER UN RETRASO DE 2 SEGUNDOA PARA QUE EL JUEGO SE ACTIVE


			//animacion bolos
			if (movBall > -50.0f)
			{
				movBall -= movballOffset * deltaTime;
				//printf("avanza%f \n ",movCoche);
				rotball += rotballOffset * deltaTime;
			}
			else
			{
				movBall = 70.0f;
			}

			//Ball
			model = modelaux;
			model = glm::translate(model, glm::vec3(movBall, -6.0f, 25.0f));
			//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, rotball * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
			//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Ball.RenderModel();

			//Jake el perro 
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(70.0f, 3.7f, 180.0f));
			//model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f));
			modelJake = model;
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeCuerpo.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(-1.0f, 2.0f, 0.0f));
			model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeBrazoDer.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(0.95f, 2.0f, 0.0f));
			// Aplicar rotación para el golpe en el brazo izquierdo - cambiado a rotación en Y para movimiento horizontal
			model = glm::rotate(model, glm::radians(anguloGolpe), glm::vec3(0.0f, 1.0f, 0.0f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakeBrazoIzq.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(-0.38f, 0.2f, -0.03f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakePiernaDer.RenderModel();

			model = modelJake;
			model = glm::translate(model, glm::vec3(0.51f, 0.2f, -0.03f));
			Material_jake.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JakePiernaIzq.RenderModel();



		}
		else {
			mainWindow.getCamaraPersona() == 1;
		}
		if (glm::distance(camPos, glm::vec3(50.0f, alturaMoneda, 180.0f)) < 100.0f && mainWindow.getJuego() == 1 && !camaraJuegoActiva) {
			//Camara de juego
			camPositionBackup = camera.getCameraPosition();
			yawBackup = camera.getYaw();
			pitchBackup = camera.getPitch();

			// Posición fija de la cámara de juego (ej. aérea)
			camera.setCameraPosition(glm::vec3(80.0f, 60.0f, 120.0f));
			camera.setYaw(-270.0f);
			camera.setPitch(-30.0f);

			camaraJuegoActiva = true;
		}
		else if (glm::distance(camPos, glm::vec3(50.0f, alturaMoneda, 180.0f)) < 100.0 && mainWindow.getJuego() == 0 && camaraJuegoActiva) {
			camera.setCameraPosition(camPositionBackup);
			camera.setYaw(yawBackup);
			camera.setPitch(pitchBackup);
			camaraJuegoActiva = false;
		}


		//animacion bolos
		/*if (movBall > -50.0f)
		{
			movBall -= movballOffset * deltaTime;
			//printf("avanza%f \n ",movCoche);
			rotball += rotballOffset * deltaTime;
		}
		else
		{
			movBall = 70.0f;
		}*/
		

		

		//Ball
		/*model = modelaux;
		model = glm::translate(model, glm::vec3(movBall, -6.0f, 25.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotball * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ball.RenderModel();*/






		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}