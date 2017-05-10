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
//glm::vec3 normal;
glm::vec3 temp;
glm::vec3 derP;
glm::vec3 point;
float cont = 0.f;
void collision(glm::vec3 normal, float distance, glm::vec3 vertexs[], glm::mat4 matAnt, glm::mat4 matPost);
void rebot(glm::vec3 vertPos, glm::vec3 normal, glm::vec3 ra);
//glm::vec3 posAnt[];
//glm::vec3 posNext[];

struct Cub {
	float w, d, h = 2.f;
	float massa = 1;
	float gravetat = 9.8f;
	float forsa = 5.f;
	glm::vec3 f = glm::vec3(0, forsa, 0);
	glm::vec3 centreMassa = glm::vec3(0.f, 8.f, 0.f);
	glm::vec3 force = glm::vec3(0, -gravetat, 0);
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

		//TODO
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
	randomize = glm::vec3(x, y, z);
	cub->primer = true;
	cub->posicio = cub->centreMassa;
	cub->momentumAngular = glm::vec3(0.f);
	cub->movLineal = glm::vec3(0.f);
}

void PhysicsUpdate(float dt) {

	cont += ImGui::GetIO().DeltaTime;
	//std::cout << "Time:" << cont;

	if (cont >= 3) {
		PhysicsInit();
		cont = 0;
	}
	cub->force = glm::vec3(x, -cub->gravetat, z);
	cub->tau = glm::cross((randomize - cub->centreMassa), cub->f); //calcul de la tau --> el centre de massa es on comença el cub

	
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
		collision(glm::vec3(0, 0, -1), 5.f, vertexs, matAnt, matrix);
		//back
		collision(glm::vec3(0, 0, 1), 5.f, vertexs, matAnt, matrix);
		//left
		collision(glm::vec3(1, 0, 0), 5.f, vertexs, matAnt, matrix);
		//right
		collision(glm::vec3(-1, 0, 0), 5.f, vertexs, matAnt, matrix);
		//top
		collision(glm::vec3(0, -1, 0), 10.f, vertexs, matAnt, matrix);
		//floor
		collision(glm::vec3(0, 1, 0), 0.f, vertexs, matAnt, matrix);

	Cubo::updateCubo(matrix);
}
void PhysicsCleanup() {
	Cubo::cleanupCubo();
}
void collision(glm::vec3 normal, float distance, glm::vec3 vertexs[], glm::mat4 matAnt, glm::mat4 matPost) {
	for (int i = 0; i < 8; i++) {
		glm::vec3 posAnt = glm::vec3(matAnt * glm::vec4(vertexs[i], 1.f));
		glm::vec3 posT = glm::vec3(matPost * glm::vec4(posAnt, 1.f));

		//colisio
		if ((glm::dot(normal, posT) + distance)*((glm::dot(normal, posAnt) + distance)) < 0) {
			//rebot
			rebot(vertexs[i], normal, posT);	
		}
	}
}
void rebot(glm::vec3 vertPos, glm::vec3 normal, glm::vec3 ra) {

		glm::vec3 derP = cub->velocitat + glm::cross(cub->angularVel, (vertPos - cub->posicio));
		float velrAnt = glm::dot(normal, derP);
		float ep = 0.1f;
		float velrPost = -ep*velrAnt;
		//derP = cub->velocitat + glm::cross(cub->angularVel, (vertexs[i] - cub->posicio));

		float j = (-(1 + ep)*velrPost) / ((1 / cub->massa) + glm::dot(normal, glm::cross((cub->inversa*(glm::cross(ra, normal))), (ra))));
		glm::vec3 J = j*normal;
		glm::vec3 t = glm::cross(ra, J);

		cub->movLineal = cub->movLineal + J;
		cub->momentumAngular = cub->momentumAngular + t;
	
}
