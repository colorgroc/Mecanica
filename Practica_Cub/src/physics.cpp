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

struct Cub {
	float massa = 1;
	float gravetat = 9.8f;
	glm::vec3 force = glm::vec3(0, -gravetat, 0);
	glm::vec3 x;
	glm::vec3 v;
	glm::vec3 p;
	bool primer;
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
	random = rand() % 10;
	//std::cout << random;
	cub->primer = true;
	//cub->force = glm::vec3(0, cub->gravetat, 0);
	//glm::mat4 matrix;
	//matrix = glm::translate(matrix, cub->force);
	
	//Cubo::setupCubo(cub->force);
}
void PhysicsUpdate(float dt) {
	//TODO
	//float x = ImGui::GetID().DeltaTime;
	//x += 0.05f;
	cub->force = glm::vec3(0, -cub->gravetat, 0);
	//y += 0.05f;
	if (cub->primer) {
		cub->primer = false;
		cub->force = glm::vec3(0, cub->gravetat * 30, 0);
	}
	//z += 0.05f;
	
	
	//rotZ += 1.f;
	glm::vec3 initialP = cub->p;
	glm::vec3 initialX = cub->x;

	cub->p = cub->p + dt*cub->force;
	cub->v = cub->p / cub->massa;
	cub->x = cub->x + dt*cub->v;
	//glm::mat4 matrix = glm::translate(glm::mat4(1), glm::vec3(x*random, y*random, z*random));
	glm::mat4 matrix;
	matrix = glm::translate(matrix, cub->x);
	//matrix = glm::rotate(matrix, (GLfloat)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 0.0f, z));
	//matrix = glm::rotate(matrix, ImGui::GetTime() * glm::radians(50.0f), glm::vec3(0.0f, 0.0f, rotZ));
	Cubo::updateCubo(matrix);
	
}
void PhysicsCleanup() {
	//TODO
}