#include "Utils.h"
#include "Input.h"
#include "Camera.h"
#include "Texture.h"
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "Light.h"
#include "Material.h"
#include "Model.h"
#include "Terrain.h"



int InsertMayaModelIntoScene();

int main()
{
	InsertMayaModelIntoScene();
}

int InsertMayaModelIntoScene()
{
	init();
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	GLFWwindow* window = createWindow(1920, 1080, "OpenGL", true);
	glfwSetKeyCallback(window, keyboard_callback);
	glfwSetCursorPosCallback(window, mousepos_callback);
	glfwSetMouseButtonCallback(window, mousebutton_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Shader* shdPhong = new Shader("res/shaders/phong.vs", "res/shaders/phong.fs");
	Shader* shdBlinnPhong = new Shader("res/shaders/blinn-phong.vs", "res/shaders/blinn-phong.fs");
	Shader* shdLight = new Shader("res/shaders/light.vs", "res/shaders/light.fs");
	Shader* shdCurrent = shdBlinnPhong;

	

	

	std::vector<Model*> models;

	Model* axe = new Model("res/models/axe/axe_OBJ.obj");
	axe->scale = { 0.2f, 0.2f, 0.2f };
	axe->rotation.y = -90;
	axe->rotation.x = 90;
	axe->position = { -7.0f, 0.2f, 2.0f };
	models.push_back(axe);

	Model* house = new Model("res/models/house/house_OBJ.obj");
	house->scale = { 0.5f, 0.5f, 0.5f };
	house->position = { -8.0f, 0.0f, 0.0f };
	house->rotation.y = -180.0f;
	models.push_back(house);

	Model* revolver = new Model("res/models/revolver/revolver_OBJ.obj");
	revolver->scale = { 0.3f, 0.3f, 0.3f };
	revolver->position = { 7.0f, 0.2f, 2.0f };
	revolver->rotation.x = -90;
	//revolver->rotation.z = -90;
	//revolver->rotation.y = 90.0f;
	models.push_back(revolver);

	Model* soba = new Model("res/models/sobata_od_domasnite/soba.obj");
	soba->scale = { 0.5f, 0.5f, 0.5f };
	soba->position = { 0.0f, -5.0f, 0.0f };
	soba->rotation.y = -90.0f;
	models.push_back(soba);

	Model* robot = new Model("res/models/robot/robot_OBJ.obj");
	robot->scale = { 1.5f, 1.5f, 1.5f };
	robot->position = { 0.0f, 4.0f, -7.0f };
	models.push_back(robot);

	

	Model* lightModel = new Model(Mesh::generate_uvsphere(Material::Default()));
	lightModel->scale = { 0.2, 0.2, 0.2 };
	std::vector<Light*> lights;
	Light* pointLight = new Light();
	lights.push_back(pointLight);
	Light* directionalLight = new Light();
	directionalLight->type = Light::DirectionalLight;
	lights.push_back(directionalLight);
	Light* spotLight = new Light();
	spotLight->type = Light::SpotLight;
	lights.push_back(spotLight);

	//Material* terrainMaterial = new Material(new Texture("res/textures/grass/diffuse.png", GL_SRGB_ALPHA));
	//terrainMaterial->specular = { 0,0,0 };
	//Terrain terrain(100, 100, 100, terrainMaterial, false, true);
	//terrain.scale = glm::vec3(0.0f / terrain.width+0.5, 0.0f / terrain.height, 0.1f / terrain.depth+0.5);

	

	Camera* camera = new Camera();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	float delta = 0;

	while (!glfwWindowShouldClose(window))
	{
		float time = glfwGetTime();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//camera movement
		float xAxis = Input::d() - Input::a();
		float yAxis = Input::space() - Input::lctrl();
		float zAxis = Input::w() - Input::s();
		glm::vec3 deltaPosition = { 0, 0, 0 };
		deltaPosition += zAxis * glm::normalize(camera->front);
		deltaPosition += yAxis * glm::normalize(camera->up);
		deltaPosition += xAxis * glm::normalize(glm::cross(camera->front, camera->up));
		camera->position += deltaPosition * 5.0f *  delta;
		camera->update(delta);
		
		//lights
		/*for (Light* light : lights)
		{
			if (light->type == Light::PointLight)
			{
				lightModel->position = light->position;
				shdLight->bind();
				shdLight->setUniform3f("color", light->diffuse);
				lightModel->draw(shdLight, camera);
			}
		}*/
		spotLight->position = camera->position;
		spotLight->direction = camera->direction;
		pointLight->diffuse = glm::vec3((cos(time) + 1.0f) / 2.0f, (cos(time * 2.0f) + 1.0f) / 2.0f, (cos(time * -2.0f) + 1.0f) / 2.0f);
		pointLight->position.x = 3.0f * cos(time);
		pointLight->position.y = 2.0;
		pointLight->position.z = 3.0f * sin(time);

		if (Input::f())
		{
			Input::f(false);
			if (spotLight->range != 0.0f)
				spotLight->setRange(0.0f);
			else
				spotLight->setRange(5.0f);
		}

		if (Input::b())
		{
			Input::b(false);
			if (shdCurrent == shdPhong)
				shdCurrent = shdBlinnPhong;
			else
				shdCurrent = shdPhong;
		}

		//models
		for (Model* model : models)
		{
			model->update(delta);
			model->draw(shdCurrent, camera, &lights);
		}

		//terrain.draw(shdCurrent, camera, &lights);
		
		
		
		glfwSwapBuffers(window);
		if (Input::escape())
			glfwSetWindowShouldClose(window, true);
		glfwPollEvents();

		delta = glfwGetTime() - time;
	}

	glfwTerminate();

	return 0;
}