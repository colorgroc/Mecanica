#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cstdio>
#include <cassert>
#include <time.h>
#include <stdlib.h>
#include <iostream>

#include "GL_framework.h"

float x = 0;
float y = 0;
float z = 0;
float rotZ = 0;
float random = 0;
int dis;
glm::mat4 matAnt;
glm::mat4 matrix;
glm::vec3 randomize;
float ep = 0.1f;
bool primerCop = true;
glm::vec3 temp;
glm::vec3 derP;
glm::vec3 point;
float cont = 0.f;
float timeCont = 20;
void Collision(glm::vec3 normal, float distance, glm::vec3 vertexs[], glm::mat4 matAnt, glm::mat4 matPost);
void Rebot(glm::vec3 vertPos, glm::vec3 normal, glm::vec3 ra);


struct Cub {
	float w, d, h = 2.f;
	float massa = 1;
	float gravetat = 9.8f;
	float forsa = 2.f;
	glm::vec3 f = glm::vec3(0, forsa, 0);
	glm::vec3 centreMassa = glm::vec3(0.f, 5.f, 0.f);
	glm::vec3 force;
	glm::vec3 posicio;
	glm::vec3 velocitat;
	glm::vec3 movLineal;
	glm::mat3 iBody = glm::mat3(4);
	glm::mat3 inversa;
	glm::mat3 rotacio = glm::mat3(1);
	glm::vec3 momentumAngular;
	glm::vec3 tau;
	glm::vec3 angularVel;
	glm::vec3 j;
	glm::vec3 J;


	bool primer;
	glm::quat quat = glm::quat(0, 0, 0, 0);

	
};

Cub *cub = new Cub();

glm::vec3 vertexs[] = {
	
	glm::vec3(-cub->w / 2.f, cub->h / 2.f, -cub->d / 2.f),
	glm::vec3(cub->w / 2.f, cub->h / 2.f, -cub->d / 2.f),
	glm::vec3(cub->w / 2.f, -cub->h / 2.f, -cub->d / 2.f),
	glm::vec3(-cub->w / 2.f, -cub->h / 2.f, -cub->d / 2.f),
	glm::vec3(-cub->w / 2.f, cub->h / 2.f, cub->d / 2.f),
	glm::vec3(cub->w / 2.f, cub->h / 2.f, cub->d / 2.f),
	glm::vec3(cub->w / 2.f, -cub->h / 2.f, cub->d / 2.f),
	glm::vec3(-cub->w / 2.f, -cub->h / 2.f, cub->d / 2.f),
};

namespace Cubo {
	extern void setupCubo(glm::vec3 pos = glm::vec3(0.f, 0.f, 0.f));
	extern void cleanupCubo();
	extern void updateCubo(glm::mat4 matrix);
	extern void drawCubo();
}

bool show_test_window = false;

void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::SliderFloat("Fuerza", &cub->forsa, -20, 20);
		ImGui::SliderFloat("Massa", &cub->massa, 0, 100);
		ImGui::SliderFloat("Epsilon", &ep, 0, 1);
		ImGui::SliderFloat("Gravetat", &cub->gravetat, 0, 100);
		ImGui::SliderFloat("Time", &cont, 0, timeCont);

	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if(show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void PhysicsInit() {

	srand((unsigned)time(NULL));
	x = rand() % 20;
	y = rand() % 20;
	z = rand() % 20;

	float posX = rand() % 5;
	float posY = rand() % 10;
	float posZ = rand() % 5;
	int signX = rand() % 2;
	int signY = rand() % 2;
	int signZ = rand() % 2;
	//std::cout << "X:" << signX << "Y:" << signY << "Z:" << signZ;
	if (signX == 1) {
		posX = -posX; 
		x = - x;
	}
	if (signY == 1) { 
		posY = -posY; 
		y = - y;
	}

	if (signZ == 1) { 
		posZ = -posZ;
		z = - z;
	}

	randomize = glm::vec3(x, y, z);
	glm::vec3 randPos = glm::vec3(posX, posY, posZ);

	cub->primer = true;

	if(primerCop) cub->posicio = cub->centreMassa;
	else cub->posicio = randPos; //fent aixo fa encara més el loco

	cub->momentumAngular = glm::vec3(0.f);
	cub->movLineal = glm::vec3(0.f);
}

void PhysicsUpdate(float dt) {

	cont += ImGui::GetIO().DeltaTime;


	if (cont >= timeCont) {
		primerCop = false;
		PhysicsInit();
		cont = 0;
	}

	if (cub->primer) {
		cub->tau = glm::cross((randomize - cub->centreMassa), cub->f); //calcul de la tau --> el centre de massa es on comença el cub
		cub->primer = false;
	}
	else cub->tau = glm::vec3(0.f, 0.f, 0.f);

	cub->force = glm::vec3(x, -cub->gravetat, z);

	cub->movLineal = cub->movLineal + dt*cub->force; //la P
	cub->velocitat = cub->movLineal / cub->massa; //velocitat
	cub->momentumAngular = cub->momentumAngular + dt*cub->tau; // la L 
	cub->posicio = cub->posicio + dt*cub->velocitat; //la X esa
	cub->rotacio = glm::mat3_cast(cub->quat); //pasem el quat a matriu 3x3
	cub->inversa = cub->rotacio*glm::inverse(cub->iBody)*glm::transpose(cub->rotacio); //Matriu invertida rotacio - iBody
	cub->angularVel = cub->inversa*cub->momentumAngular; //velocitat angular
	cub->quat = 1.f / 2.f * glm::quat(0.f, cub->angularVel)*cub->quat;  //derivada del quat
	cub->quat = glm::normalize(cub->quat); //aixo es fa pq mho ha dit el profe XD, q sino es perdia info i no acabava de ferho be
	
	matAnt = matrix;

	matrix = glm::translate(matrix, cub->posicio);
	matrix = matrix*glm::mat4_cast(cub->quat); //apliquem la rotacio a la matrix passant altra vegada el quat a mat4
	
	
	//------------COLISIONS--------------------------------------
		//front
		Collision(glm::vec3(0, 0, -1), 5.f, vertexs, matAnt, matrix);
		//back
		Collision(glm::vec3(0, 0, 1), 5.f, vertexs, matAnt, matrix);
		//left
		Collision(glm::vec3(1, 0, 0), 5.f, vertexs, matAnt, matrix);
		//right
		Collision(glm::vec3(-1, 0, 0), 5.f, vertexs, matAnt, matrix);
		//top
		Collision(glm::vec3(0, -1, 0), 10.f, vertexs, matAnt, matrix);
		//floor
		Collision(glm::vec3(0, 1, 0), 0.f, vertexs, matAnt, matrix);

	Cubo::updateCubo(matrix);
}
void PhysicsCleanup() {
	Cubo::cleanupCubo();
}
void Collision(glm::vec3 normal, float distance, glm::vec3 vertexs[], glm::mat4 matAnt, glm::mat4 matPost) {
	for (int i = 0; i < 8; i++) {
		glm::vec3 posAnt = glm::vec3(matAnt * glm::vec4(vertexs[i], 1.f));
		glm::vec3 posT = glm::vec3(matPost * glm::vec4(posAnt, 1.f));

		//colisio
		if ((glm::dot(normal, posT) + distance)*((glm::dot(normal, posAnt) + distance)) < 0) {
			//rebot
			Rebot(vertexs[i], normal, posT);	
			//faltaria comprobar colisio per temps
		}
	}
}
void Rebot(glm::vec3 vertPos, glm::vec3 normal, glm::vec3 ra) {

		glm::vec3 derP = cub->velocitat + glm::cross(cub->angularVel, (vertPos - cub->posicio));
		float velrAnt = glm::dot(normal, derP);
		float velrPost = -ep*velrAnt;
		//derP = cub->velocitat + glm::cross(cub->angularVel, (vertexs[i] - cub->posicio));

		float j = (-(1 + ep)*velrPost) / ((1 / cub->massa) + glm::dot(normal, glm::cross((cub->inversa*(glm::cross(ra, normal))), (ra))));
		glm::vec3 J = j*normal;
		glm::vec3 t = glm::cross(ra, J);

		cub->movLineal = cub->movLineal + J;
		cub->momentumAngular = cub->momentumAngular + t;
	
}
