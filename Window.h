#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);
	}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }

	// Variables para movimiento y animación de Jake
	GLfloat getPosX() { return posX; }
	GLfloat getPosZ() { return posZ; }
	GLfloat getBrazoDerAng() { return brazoDerAng; }
	GLfloat getBrazoIzqAng() { return brazoIzqAng; }
	GLfloat getPiernaDerAng() { return piernaDerAng; }
	GLfloat getPiernaIzqAng() { return piernaIzqAng; }
	GLfloat getDireccion() { return direccion; }

	GLfloat getLantern() { return Lantern; }
	GLfloat getJuego() { return Juego; }

	GLfloat getCamaraAerea() { return CamaraAerea; }//Camara aerea

	// Función para actualizar la animación de Jake
	void actualizarAnimacionJake();

	~Window();
private:
	GLFWwindow* mainWindow;
	GLint width, height;



	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	GLfloat Lantern;//para linterna
	GLfloat Juego;//para los juego

	GLfloat CamaraAerea;

	


	bool mouseFirstMoved;

// Variables para el movimiento y animación de Jake
	GLfloat posX, posZ;          // Posición de Jake
	GLfloat direccion;           // Dirección a la que mira Jake
	GLfloat brazoDerAng;         // Ángulo del brazo derecho
	GLfloat brazoIzqAng;         // Ángulo del brazo izquierdo
	GLfloat piernaDerAng;        // Ángulo de la pierna derecha
	GLfloat piernaIzqAng;        // Ángulo de la pierna izquierda
	bool enMovimiento;           // Indica si Jake está en movimiento
	GLfloat velocidadAnim;       // Velocidad de la animación
	GLfloat rangoAnim;           // Rango de movimiento de las extremidades
	bool direccionAnim;          // Dirección de la animación (oscilación)

	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

