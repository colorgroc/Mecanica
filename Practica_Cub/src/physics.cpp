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
glm::vec3 randomize;
glm::vec3 normal;
glm::vec3 temp;
glm::vec3 derP;
float cont = 0.f;

struct Cub {
	float w, d, h = 2.f;
	float massa = 1;
	float gravetat = 9.8f;
	float forsa = 5.f;
	glm::vec3 f = glm::vec3(0, forsa, 0);
	glm::vec3 centreMassa = glm::vec3(0.f, 8.f, 0.f);
	glm::vec3 force = glm::vec3(0, -gravetat, 0);
	glm::vec3 x;
	glm::vec3 v;
	glm::vec3 p;
	glm::mat3 iBody = glm::mat3(4);
	glm::mat3 I;
	glm::mat3 R = glm::mat3(1);
	glm::vec3 l;
	glm::vec3 tau;
	glm::vec3 angularVel;
	glm::vec3 A = glm::vec3(-w / 2, h / 2, -d / 2);
	glm::vec3 B = glm::vec3(w / 2, h / 2, -d / 2);
	glm::vec3 C = glm::vec3(w / 2, -h / 2, -d / 2);
	glm::vec3 D = glm::vec3(-w / 2, -h / 2, -d / 2);
	glm::vec3 E = glm::vec3(w / 2, -h / 2, d / 2);
	glm::vec3 F = glm::vec3(w / 2, h / 2, d / 2);
	glm::vec3 G = glm::vec3(-w / 2, h / 2, d / 2);
	glm::vec3 H = glm::vec3(-w / 2, -h / 2, d / 2);

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

}
void PhysicsUpdate(float dt) {

	cub->force = glm::vec3(x, -cub->gravetat, z);
	cub->tau = glm::cross((randomize - cub->centreMassa), cub->f); //calcul de la tau --> el centre de massa es on comença el cub

	if (cub->primer) {
		cub->primer = false;
		cub->force = glm::vec3(0, cub->gravetat * 30, 0); //fent aixo fem q el cub vagi cap amunt per caure despres
	}


	
	cub->p = cub->p + dt*cub->force; //la P
	cub->v = cub->p / cub->massa; //velocitat
	cub->l = cub->l + dt*cub->tau; // la L 
	cub->x = cub->x + dt*cub->v; //la X esa
	cub->R = glm::mat3_cast(cub->quat); //pasem el quat a matriu 3x3
	cub->I = cub->R*glm::inverse(cub->iBody)*glm::transpose(cub->R); //Matriu invertida rotacio - iBody
	cub->angularVel = cub->I*cub->l; //velocitat angular
	cub->quat = 1.f / 2.f * glm::quat(0.f, cub->angularVel)*cub->quat;  //derivada del quat
	cub->quat = glm::normalize(cub->quat); //aixo es fa pq mho ha dit el profe XD, q sino es perdia info i no acabava de ferho be
	

	glm::mat4 matrix;
	matrix = glm::translate(matrix, cub->x);
	matrix = matrix*glm::mat4_cast(cub->quat); //apliquem la rotacio a la matrix passant altra vegada el quat a mat4
	
	
	//------------COLISIONS--------------------------------------
	// Floor Colision
	normal = glm::vec3(0, 1, 0);
	dis = 0;
	derP = cub->v + glm::cross(cub->angularVel, (cub->D - cub->x));

	/*if ((glm::dot(normal, derP) + dis)*((glm::dot(normal, cub->D) + dis)) < 0) {
	pC[i].pos = pC[i].pos - (1 + coefElasticity) * (glm::dot(normal, pC[i].pos) + dis)*normal;
	pC[i].vel = pC[i].vel - (1 + coefElasticity) * (glm::dot(normal, pC[i].vel))* normal - coefFriction*(pC[i].vel - temp);
	}

	// Top Colision
	temp = glm::dot(normal, pC[i].vel) * normal;
	normal = glm::vec3(0, -1, 0);
	dis = 10;
	if ((glm::dot(normal, pC[i].pos) + dis)*((glm::dot(normal, initial) + dis)) < 0) {
	pC[i].pos = pC[i].pos - (1 + coefElasticity) * (glm::dot(normal, pC[i].pos) + dis)*normal;
	pC[i].vel = pC[i].vel - (1 + coefElasticity) * (glm::dot(normal, pC[i].vel))* normal - coefFriction*(pC[i].vel - temp);
	}

	// Right Face Colision
	normal = glm::vec3(-1, 0, 0);
	temp = glm::dot(normal, pC[i].vel) * normal;
	dis = 5;
	if ((glm::dot(normal, pC[i].pos) + dis)*((glm::dot(normal, initial) + dis)) < 0) {
	pC[i].pos = pC[i].pos - (1 + coefElasticity) * (glm::dot(normal, pC[i].pos) + dis)*normal;
	pC[i].vel = pC[i].vel - (1 + coefElasticity) * (glm::dot(normal, pC[i].vel))* normal - coefFriction*(pC[i].vel - temp);
	}

	// Left Face Colision
	normal = glm::vec3(1, 0, 0);
	temp = glm::dot(normal, pC[i].vel) * normal;
	dis = 5;
	if ((glm::dot(normal, pC[i].pos) + dis)*((glm::dot(normal, initial) + dis)) < 0) {
	pC[i].pos = pC[i].pos - (1 + coefElasticity) * (glm::dot(normal, pC[i].pos) + dis)*normal;
	pC[i].vel = pC[i].vel - (1 + coefElasticity) * (glm::dot(normal, pC[i].vel))* normal - coefFriction*(pC[i].vel - temp);

	}*/

	// Front Face Colision
	normal = glm::vec3(0, 0, -1);
	derP = cub->v + glm::cross(cub->angularVel, (cub->D - cub->x));
	float velr = glm::dot(normal, derP);
	glm::vec3 posT = cub->D*velr*dt;
	dis = 5;
	//	if ((glm::dot(normal, derP) + dis)*((glm::dot(normal, cub->D) + dis)) < 0) {
	//if ((glm::dot(normal, posT) + dis)*((glm::dot(normal, cub->D) + dis)) < 0) {
	if ((velr + dis)*(glm::dot(normal, cub->D) + dis) < 0) { //nidea tu
		std::cout << "He colis amb D front";
	}

	// Back Face Colision
	/*normal = glm::vec3(0, 0, 1);
	temp = glm::dot(normal, pC[i].vel) * normal;
	dis = 5;
	//int deb = -(particlesContainer[i].pos.x*normal.x) - (particlesContainer[i].pos.y*normal.y) - (particlesContainer[i].pos.z*normal.z);
	//std::cout << deb;
	if ((glm::dot(normal, pC[i].pos) + dis)*((glm::dot(normal, initial) + dis)) < 0) {
	pC[i].pos = pC[i].pos - (1 + coefElasticity) * (glm::dot(normal, pC[i].pos) + dis)*normal;
	pC[i].vel = pC[i].vel - (1 + coefElasticity) * (glm::dot(normal, pC[i].vel))* normal - coefFriction*(pC[i].vel - temp);

	}*/

	//collisions
	/*for (int i = 0; i < 8; i++) {

	}*/
	Cubo::updateCubo(matrix);
}
void PhysicsCleanup() {
	//TODO
	Cubo::cleanupCubo();
}
