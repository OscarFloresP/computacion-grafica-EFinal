#include <stdlib.h>
#include <time.h> 

#include "files.h"
#include "model.h"
#include "cam.h"
#include "PV2D.h"

#include <shader.h>
#include <texture.h>
 
const i32 SCR_WIDTH = 1280;
const i32 SCR_HEIGHT = 720;
const f32  ASPECT = (f32)SCR_WIDTH / (f32)SCR_HEIGHT;

Cam* cam;


f32 deltaTime = 0.0f;
f32 lastFrame = 0.0f;
bool wireframe = false;

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		deltaTime *= 10;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cam->processKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cam->processKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cam->processKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cam->processKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		cam->processKeyboard(UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		cam->processKeyboard(DOWN, deltaTime);
	}
}
void key_callback(GLFWwindow*, int key, int, int act, int) {
	wireframe ^= key == GLFW_KEY_E && act == GLFW_PRESS;
}
void mouse_callback(GLFWwindow* window, f64 xpos, f64 ypos) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		cam->movePov(xpos, ypos);
	}
	else {
		cam->stopPov();
	}	
}
void scroll_callback(GLFWwindow* window, f64 xoffset, f64 yoffset) {
	cam->processScroll((f32)yoffset);
}

f32 matrix[101][101];


void load_positions(u32& n, std::vector<glm::vec3>& positions) {
	for (u32 x = 0; x < n; ++x) {
		for (u32 z = 0; z < n; ++z) {
			positions[x * n + z] = glm::vec3(x, 0.0f, z);
		}
	}
}
bool interseccion(PV2D p, PV2D v, PV2D v_1, PV2D &n, GLdouble &hit, bool usar_mascaras)
	{
		GLdouble dist[3], proy[3]; int sign[3];
		int nHits= 0; double t_hit[3]; PV2D norm[3]; 

		for(int i=0; i<3; i++)
		{
			PV2D vector_p_pi = p.crearVector_RestaPto2Pto(_v[i]);
			
			dist[i] = vector_p_pi.productoEscalar(v_1.vectorNormal()); 
			proy[i] = vector_p_pi.productoEscalar(v_1); 
			if (dist[i] > _epsilon) 
				sign[i] = 1;
			else if (dist[i] < -(_epsilon)) 
				sign[i] = -1;
			else 
			{
				sign[i] = 0;

				t_hit[nHits] = proy[i]; 
				norm[nHits] = _n_v[i];
				nHits++;
			}
		}
		if(abs(sign[0] + sign[1] + sign[2]) == 3) 
		{
			return false;
		}
		else 
		{
			for(int i=0; i<3; i++)
			{
				int j = (i+1)%3;
				if(sign[i]*sign[j] < 0) 
				{
					t_hit[nHits] = (proy[i]*dist[j] - dist[i]*proy[j]) / (dist[j]-dist[i]);
					norm[nHits] = _n[i];
					nHits++;
				}			
			}
		}

		int min = 0;
		for(int i=0; i<nHits; i++)
		{
			if(t_hit[min] > t_hit[i])
				min = i;
		}

		n = norm[min];
		hit = t_hit[min]/v.moduloVect(); 
		return true;
	}

i32 main() {
	GLFWwindow* window = glutilInit(3, 3, SCR_WIDTH, SCR_HEIGHT, "Agar.io");
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glEnable(GL_DEPTH_TEST);

	cam = new Cam();

	srand(time(NULL));
	u32 n = 101;
	std::vector<glm::vec3> positions(n * n);
	u32 amount = n * n;
	generate_random(n, -5, 15, 20);
	load_positions(n, positions);

	glm::vec3 lightPos = glm::vec3(1.0f);
	glm::vec3 lightColor = glm::vec3(1.0f);

	Files* files = new Files();

	Shader* mapaShader = new Shader("mapa.vert", "mapa.frag");
	Shader* modelShader = new Shader("model.vert", "model.frag");

	Model* tierra = new Model(files, "dirt/dirt.obj");
	Model* jugador = new Model(files, "rock/rock.obj");
	Model* enemigo = new Model(files, "planet/planet.obj");

	std::vector<glm::mat4> modelMatricestierra;

	glm::mat4 modelaux;
	for (ui32 i = 0; i < n; i++) {
		for (ui32 j = 0; j < n; j++) {
			modelaux = glm::mat4(1.0f);
			modelaux = glm::translate(modelaux, 2.0f*positions[i*n+j]);
			modelMatricestierra.push_back(modelaux);

		}
	}
	

	glm::mat4* matricestierra = new glm::mat4[modelMatricestierra.size()];

	for (ui32 i = 0; i < modelMatricestierra.size(); i++) {
		matricestierra[i] = modelMatricestierra[i];
	}

	for (unsigned int i = 0; i < tierra->meshes.size(); i++)
	{
		unsigned int VAO = tierra->meshes[i].Vao;
		glBindVertexArray(VAO);

		unsigned int buffertierra;
		glGenBuffers(1, &buffertierra);
		glBindBuffer(GL_ARRAY_BUFFER, buffertierra);
		glBufferData(GL_ARRAY_BUFFER, modelMatricestierra.size() * sizeof(glm::mat4), &matricestierra[0], GL_STATIC_DRAW);


		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}
	
	
	
	while (!glfwWindowShouldClose(window)) {
		f32 currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(64.0f / 255.0f, 224.0f / 255.0f, 208.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);


		glm::mat4 projection = glm::perspective(cam->zoom, ASPECT, 0.1f, 100.0f);
		glm::vec3 viewPos = cam->pos;
		glm::mat4 view = cam->getViewM4();

		mapaShader->useProgram();
		mapaShader->setVec3("pointLight.position", { 0.0, 0.0, 0.0 });
		mapaShader->setVec3("pointLight.ambient", 0.1f, 0.1f, 0.1f);
		mapaShader->setVec3("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
		mapaShader->setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
		mapaShader->setF32("pointLight.constant", 1.0f); 
		mapaShader->setF32("pointLight.linear", 0.09f);
		mapaShader->setF32("pointLight.quadratic", 0.032f);


		mapaShader->setMat4("proj", projection);
		mapaShader->setMat4("view", view);
		mapaShader->setVec3("viewPos", cam->pos);

		modelShader->useProgram();
		modelShader->setMat4("proj", projection);
		modelShader->setMat4("view", view);
		modelShader->setVec3("lightPos", lightPos);
		modelShader->setVec3("lightColor", lightColor);
		modelShader->setVec3("viewPos", cam->pos);


		model = glm::mat4(1.0f);
		model = glm::translate(model, cam->pos + vec3(0.0f, -0.7f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		modelShader->setMat4("model", model);
		jugador->Draw(modelShader);
		

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(n, (n * 2.0f) / 3.0f, n));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		modelShader->setMat4("model", model);
		enemigo->Draw(modelShader);

		mapaShader->useProgram();
		mapaShader->setF32("material.shininess", 32.0f);
		mapaShader->setF32("material.specular", 0.5f);
		mapaShader->setI32("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_2D, tierra->textures_loaded[0].id);
		for (unsigned int i = 0; i < tierra->meshes.size(); i++) {
			glBindVertexArray(tierra->meshes[i].Vao);
			glDrawElementsInstanced(GL_TRIANGLES, tierra->meshes[i].indices.size(),
				GL_UNSIGNED_INT, 0, modelMatricestierra.size());
			glBindVertexArray(0);
		}
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete cam;
	delete mapaShader;
	delete tierra;
	delete jugador;
	delete enemigo;


	delete modelShader;

	return 0;
}
