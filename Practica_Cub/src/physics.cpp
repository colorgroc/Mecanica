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
glm::vec3 randomize;

struct Cub {
	float massa = 1;
	float gravetat = 9.8f;
	float forsa = 5.f;
	glm::vec3 f = glm::vec3(0, forsa, 0);
	glm::vec3 centreMassa = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 force = glm::vec3(0, -gravetat, 0);
	glm::vec3 x;
	glm::vec3 v;
	glm::vec3 p;
	glm::mat3 iBody = glm::mat3(4);//glm::inverse(glm::mat3(4));
	//glm::vec3 I;
	glm::mat3 I;
	glm::mat3 R = glm::mat3(1);
	glm::vec3 l;
	glm::vec3 tau;
	glm::vec3 angularVel;
	bool primer;
	glm::quat quat = glm::quat(0, 0, 0, 0);
	glm::quat q;
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

Cub *cub = new Cub();

void PhysicsInit() {
	//TODO
	srand((unsigned)time(NULL));
	x = rand() % 20;
	y = rand() % 20;//rand() % 10;
	z = rand() % 20;//rand() % 10;
	randomize = glm::vec3(x, y, z);
	//cub->i = glm::inverse(cub->i);
	//std::cout << random;
	cub->primer = true;

}
void PhysicsUpdate(float dt) {
	//TODO
	//float x = ImGui::GetID().DeltaTime;
	//x += 0.05f;
	cub->force = glm::vec3(x, -cub->gravetat, z);
	cub->tau = glm::cross((randomize - cub->centreMassa), cub->f);
	//y += 0.05f;
	if (cub->primer) {
		cub->primer = false;
		cub->force = glm::vec3(0, cub->gravetat * 30, 0);
	}


	
	cub->p = cub->p + dt*cub->force;
	cub->v = cub->p / cub->massa;
	cub->l = cub->l + dt*cub->tau;
	cub->x = cub->x + dt*cub->v;
	cub->R = glm::mat3_cast(cub->quat);
	cub->I = cub->R*glm::inverse(cub->iBody)*glm::transpose(cub->R);
	cub->angularVel = cub->I*cub->l;
	cub->quat = 1.f / 2.f * glm::quat(0.f, cub->angularVel)*cub->quat; //--> la derivada nose a on s'ha de fer
	cub->quat = glm::normalize(cub->quat);
	//cub->q = 1 / 2 * cub->angularVel*cub->quat;
	//cub->quat = 1 / 2 * cub->angularVel*cub->quat;
	

	//glm::mat4 matrix = glm::translate(glm::mat4(1), glm::vec3(x*random, y*random, z*random));
	glm::mat4 matrix;
	matrix = glm::translate(matrix, cub->x);
	matrix = matrix*glm::mat4_cast(cub->quat);
	//glm::mat4 rotation;
	//rotation = glm::translate(rotation, cub->r);
	//matrix = glm::rotate(matrix, (GLfloat)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 0.0f, z));
	//matrix = glm::rotate(matrix, ImGui::GetTime() * glm::radians(50.0f), glm::vec3(0.0f, 0.0f, rotZ));
	Cubo::updateCubo(matrix);
	
}
void PhysicsCleanup() {
	//TODO
}