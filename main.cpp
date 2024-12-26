
//#include "shortcuts.h"
#include "MeshPainter.h"

#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>

int WIDTH = 1920;
int HEIGHT = 1080;

int mainWindow;

MeshPainter* painter = new MeshPainter();
std::vector<TriMesh*> meshList;

Light* light = new Light();
GlbMesh* bmw = new GlbMesh();
GlbMesh* bmw_wheel = new GlbMesh();
GlbMesh* su7 = new GlbMesh();
GlbMesh* su7_wheel1 = new GlbMesh();
GlbMesh* su7_wheel2 = new GlbMesh();
GlbMesh* su7_wheel3 = new GlbMesh();
GlbMesh* su7_wheel4 = new GlbMesh();
GlbMesh* tree = new GlbMesh();
GlbMesh* tree2 = new GlbMesh();
GlbMesh* furina = new GlbMesh();
GlbMesh* bocchi = new GlbMesh();
GlbMesh* nina = new GlbMesh();
TriMesh* canon = new TriMesh();
TriMesh* cube = new TriMesh();
//std::unordered_map<std::string, int> meshName = { {"su7", 0}, {"furina", 1}, {"canon", 2}, {"cube", 3 } };
TriMesh* plane = new TriMesh();

Camera* camera = new Camera();

bool mousePressed = false;
double lastX, lastY;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			mousePressed = true;
			glfwGetCursorPos(window, &lastX, &lastY);
		}
		else if (action == GLFW_RELEASE) {
			mousePressed = false;
		}
	}
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
	if (mousePressed)
	{
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	glViewport(0, 0, width, height);
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	//Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	camera->setAspect(aspect);
	camera->updateProjectionMatrix();
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

std::unordered_map<std::string, int> options = {
	{"su7", 0},
	{"furina", 1},
	{"canon", 2},
	{"cube", 3},
	{"", -1}
};

void setOptions(std::string name, TriMesh*& mesh) {
	switch (options[name]) {
	case 0:
		mesh->setTranslation(glm::vec3(-1.6, 0.0, 0.0));
		mesh->setRotation(glm::vec3(0.0, 0.0, 0.0));
		mesh->setScale(glm::vec3(0.5, 0.5, 0.5));

		mesh->setAmbient(glm::vec4(20.0 / 800, 114.0 / 800, 140.0 / 800, 0.5)); // 环境光
		mesh->setDiffuse(glm::vec4(0.27568f, 0.41424f, 0.57568f, 0.55f)); // 漫反射
		mesh->setSpecular(glm::vec4(0.633f, 0.727811f, 0.633f, 0.55f)); // 镜面反射
		mesh->setShininess(76.0f); //高光系数
		break;
	case 1:
		mesh->setTranslation(glm::vec3(0.0, 0.0, 2));
		mesh->setRotation(glm::vec3(90, 0.0, 0.0));
		mesh->setScale(glm::vec3(0.8, 0.8, 0.8));

		mesh->setAmbient(glm::vec4(99.0 / 600, 129.0 / 600, 236.0 / 600, 0.5)); // 环境光
		mesh->setDiffuse(glm::vec4(0.37568f, 0.51424f, 0.37568f, 0.55f)); // 漫反射
		mesh->setSpecular(glm::vec4(0.633f, 0.727811f, 0.633f, 0.55f)); // 镜面反射
		mesh->setShininess(20.0f); //高光系数
		break;
	case 2:
		mesh->setTranslation(glm::vec3(1, 0.0, 0.0));
		mesh->setRotation(glm::vec3(0.0, 0.0, 0.0));
		mesh->setScale(glm::vec3(0.07, 0.07, 0.07));

		mesh->setAmbient(glm::vec4(0, 0, 0, 0.5)); // 环境光
		mesh->setDiffuse(glm::vec4(0.27568f, 0.27f, 0.27568f, 0.8f)); // 漫反射
		mesh->setSpecular(glm::vec4(0.633f, 0.727811f, 0.633f, 0.55f)); // 镜面反射
		mesh->setShininess(76.0f); //高光系数
		break;
	case 3:
		mesh->setTranslation(glm::vec3(0.0, 0.0, -1));
		mesh->setRotation(glm::vec3(0.0, 0.0, 0.0));
		mesh->setScale(glm::vec3(1, 1, 1));

		mesh->setAmbient(glm::vec4(52.0 / 255, 52.0 / 255, 54.0 / 255, 0.5)); // 环境光
		mesh->setDiffuse(glm::vec4(0.07568f, 0.61424f, 0.07568f, 0.55f)); // 漫反射
		mesh->setSpecular(glm::vec4(0.633f, 0.727811f, 0.633f, 0.55f)); // 镜面反射
		mesh->setShininess(76.0f); //高光系数
		break;
	default:
		break;
	}
}

std::vector<GLdouble> lowest(4);
glm::mat4 su7_matrix = glm::mat4(1.0f);

void setTexturePath(GlbMesh* glb)
{
	for (int i = 0; i < glb->getMeshes().size(); i++)
	{
		TriMesh* mesh = glb->getMeshes()[i];
		mesh->setIsDisplay(true);
		mesh->diffusePath = glb->getMaterials()[glb->getMaterialIndexes()[i]].diffuseTexturePath;
		mesh->normalPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].normalTexturePath;
		mesh->specularPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].specularTexturePath;
		mesh->metalnessPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].metalnessTexturePath;
		mesh->roughnessPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].roughnessTexturePath;
		mesh->ambientOcclusionPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].ambientOcclusionTexturePath;
		//painter->addMesh(mesh, name, mesh->diffusePath, mesh->normalPath, mesh->specularPath, mesh->metalnessPath, mesh->roughnessPath, mesh->ambientOcclusionPath, vshader, fshader);
	}
}
//use meshpainter
void init() {
	std::string vshader, fshader;
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";
	// 设置光源位置
	light->setTranslation(glm::vec3(50, 100, 50));
	light->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 环境光
	light->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 漫反射
	light->setSpecular(glm::vec4(10.0, 10.0, 10.0, 1.0)); // 镜面反射

	tree->initMeshes("tree", "glb");
	tree->setTranslation(glm::vec3(-6, 0, 0));
	tree->setRotation(glm::vec3(0, 0, 0));
	float scale = 0.005;
	tree->setScale(glm::vec3(scale, scale, scale));
	tree->compute();
	setTexturePath(tree);
	tree->updateAndLoad(painter, tree->getModelMatrix(), camera, light, "tree", vshader, fshader);

	tree2->initMeshes("tree", "glb");
	tree2->setTranslation(glm::vec3(6, 0, 0));
	tree2->setRotation(glm::vec3(0, 0, 0));
	scale = 0.005;
	tree2->setScale(glm::vec3(scale, scale, scale));
	tree2->compute();
	setTexturePath(tree2);
	tree2->updateAndLoad(painter, tree2->getModelMatrix(), camera, light, "tree", vshader, fshader);

	furina->initMeshes("furina", "glb");
	furina->setTranslation(glm::vec3(2, 0, 0));
	furina->setRotation(glm::vec3(0, -90, 0));
	scale = 0.5;
	furina->setScale(glm::vec3(scale, scale, scale));
	furina->compute();
	setTexturePath(furina);
	furina->updateAndLoad(painter, furina->getModelMatrix(), camera, light, "furina", vshader, fshader);

	bocchi->initMeshes("bocchi", "glb");
	bocchi->setTranslation(glm::vec3(6, 0, -3));
	bocchi->setRotation(glm::vec3(0, -90, 0));
	scale = 0.5;
	bocchi->setScale(glm::vec3(scale, scale, scale));
	bocchi->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 环境光
	bocchi->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 漫反射
	bocchi->setSpecular(glm::vec4(10.0, 10.0, 10.0, 1.0)); // 镜面反射
	bocchi->compute();
	setTexturePath(bocchi);
	bocchi->updateAndLoad(painter, bocchi->getModelMatrix(), camera, light, "bocchi", vshader, fshader);

	//nina->initMeshes("486", "glb");
	//nina->setTranslation(glm::vec3(-6, 0, -3));
	//nina->setRotation(glm::vec3(0, -90, 0));
	//scale = 0.5;
	//nina->setScale(glm::vec3(scale, scale, scale));
	//nina->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 环境光
	//nina->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 漫反射
	//nina->setSpecular(glm::vec4(10.0, 10.0, 10.0, 1.0)); // 镜面反射
	//nina->compute();
	//setTexturePath(nina);
	//nina->updateAndLoad(painter, nina->getModelMatrix(), camera, light, "nina", vshader, fshader);

	bmw->initMeshes("bmw_m3", "glb");
	bmw->setTranslation(glm::vec3(-4, 0, 0));
	bmw->setRotation(glm::vec3(-90, 0, 0));
	scale = 0.14;
	bmw->setScale(glm::vec3(scale, scale, scale));
	bmw->compute();
	bmw->updateAndLoad(painter, bmw->getModelMatrix(), camera, light, "bmw", vshader, fshader);


	su7->initMeshes("su7", "glb");
	su7->setTranslation(glm::vec3(0.9, 0.0, 0.0));
	su7->setRotation(glm::vec3(90, 0.0, 0.0));
	scale = 0.005;
	su7->setScale(glm::vec3(scale, scale, scale));
	su7_matrix = glm::rotate(su7_matrix, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
	su7_matrix = glm::translate(su7_matrix, glm::vec3(0.9, 0.0, 0.0));
	su7_matrix = glm::scale(su7_matrix, glm::vec3(scale, scale, scale));
	su7->compute();
	setTexturePath(su7);
	//addGlb(su7, "su7", vshader, fshader);

	su7_wheel1->initMeshes("su7_wheel1", "glb");
	su7_wheel1->setTranslation(glm::vec3(0.0, -208, 0.0));  // 轮子相对于车体的位置
	su7_wheel1->setRotation(glm::vec3(0.0, 0.0, 0.0));
	su7_wheel1->setScale(glm::vec3(1, 1, 1));
	su7_wheel1->compute();
	su7->addChild(su7_wheel1);

	su7_wheel2->initMeshes("su7_wheel2", "glb");
	su7_wheel2->setTranslation(glm::vec3(0.0, -208, 0.0));  // 轮子相对于车体的位置
	su7_wheel2->setRotation(glm::vec3(0.0, 0.0, 0.0));
	su7_wheel2->setScale(glm::vec3(1, 1, 1));
	su7_wheel2->compute();
	su7->addChild(su7_wheel2);

	su7_wheel3->initMeshes("su7_wheel3", "glb");
	su7_wheel3->setTranslation(glm::vec3(0.0, 95, 0.0));  // 轮子相对于车体的位置
	su7_wheel3->setRotation(glm::vec3(0.0, 0.0, 0.0));
	su7_wheel3->setScale(glm::vec3(1, 1, 1));
	su7_wheel3->compute();
	su7->addChild(su7_wheel3);

	su7_wheel4->initMeshes("su7_wheel4", "glb");
	su7_wheel4->setTranslation(glm::vec3(0.0, 95, 0.0));  // 轮子相对于车体的位置
	su7_wheel4->setRotation(glm::vec3(0.0, 0.0, 0.0));
	su7_wheel4->setScale(glm::vec3(1, 1, 1));
	su7_wheel4->compute();
	su7->addChild(su7_wheel4);

	su7->updateAndLoad(painter, su7->getModelMatrix(), camera, light, "su7", vshader, fshader);

	plane->generateSquare(glm::vec3(1, 1, 1));
	plane->setRotation(glm::vec3(0, 0, 0));
	plane->setTranslation(glm::vec3(0, -0.001, 0));
	plane->setScale(glm::vec3(10, 1, 100));

	plane->setAmbient(glm::vec4(0.105882f, 0.058824f, 0.113725f, 1.0f));
	plane->setDiffuse(glm::vec4(0.427451f, 0.470588f, 0.541176f, 1.0f));
	plane->setSpecular(glm::vec4(0.333333f, 0.333333f, 0.521569f, 1.0f));
	plane->setShininess(9.84615f); //高光系数
	painter->addMesh(plane, "plane", "./assets/track.jpg", "", "", "", "", "", vshader, fshader);


	glClearColor(0.2, 0.2, 0.2, 1);
}

void initSu7() {
	su7->setTranslation(glm::vec3(0.9, 0.0, 0.0));
	//su7->setRotation(glm::vec3(90, 0.0, 0.0));
	//float scale = 0.005;
	//su7->setScale(glm::vec3(scale, scale, scale));
	//su7->compute();

	su7->updateAndLoad(painter, glm::mat4(1.0), camera, light, "su7", "shaders/vshader.glsl", "shaders/fshader.glsl");
}

//void renderScene() {
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	// 相机矩阵计算
//	camera->updateCamera();
//	camera->viewMatrix = camera->getViewMatrix();
//	camera->projMatrix = camera->getProjectionMatrix(false);
//
//	for (int i = 0; i <= 3; i++)
//	{
//		TriMesh* mesh = meshes[i];
//		openGLObject mesh_object = mesh_objects[i];
//		glBindVertexArray(mesh_object.vao);
//		glUseProgram(mesh_object.program);
//
//		// 物体的变换矩阵
//		if (mesh && mesh->getIsDisplay())
//		{
//			glm::mat4 modelMatrix = mesh->getModelMatrix();
//			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -lowest[i], 0.0));// 将物体平移到y=0平面上
//
//			// 传递矩阵
//			glUniformMatrix4fv(mesh_object.modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
//			glUniformMatrix4fv(mesh_object.viewLocation, 1, GL_TRUE, &camera->viewMatrix[0][0]);
//			glUniformMatrix4fv(mesh_object.projectionLocation, 1, GL_TRUE, &camera->projMatrix[0][0]);
//			// 将着色器 isShadow 设置为0，表示正常绘制的颜色，如果是1着表示阴影
//			glUniform1i(mesh_object.shadowLocation, 0);
//			glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());
//
//			//// 阴影绘制
//			glm::mat4 shadowProjMatrix = light->getShadowProjectionMatrix();
//
//			//// 计算阴影的模型变换矩阵。
//			shadowProjMatrix = shadowProjMatrix * modelMatrix;
//			glUniform1i(mesh_object.shadowLocation, 1);
//			glUniformMatrix4fv(mesh_object.modelLocation, 1, GL_FALSE, &shadowProjMatrix[0][0]);
//			glUniformMatrix4fv(mesh_object.viewLocation, 1, GL_TRUE, &camera->viewMatrix[0][0]);
//			glUniformMatrix4fv(mesh_object.projectionLocation, 1, GL_TRUE, &camera->projMatrix[0][0]);
//			glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());
//
//			// 将材质和光源数据传递给着色器
//			bindLightAndMaterial(mesh, mesh_object, light, camera);
//			// 绘制
//			glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());
//		}
//	}
//
//
//
//	// 绘制平面
//	glBindVertexArray(plane_object.vao);
//	glUseProgram(plane_object.program);
//
//	glm::mat4 modelMatrix = plane->getModelMatrix();
//
//	glUniformMatrix4fv(plane_object.modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
//	glUniformMatrix4fv(plane_object.viewLocation, 1, GL_TRUE, &camera->viewMatrix[0][0]);
//	glUniformMatrix4fv(plane_object.projectionLocation, 1, GL_TRUE, &camera->projMatrix[0][0]);
//	glUniform1i(plane_object.shadowLocation, 0);
//	bindLightAndMaterial(plane, plane_object, light, camera);
//	glDrawArrays(GL_TRIANGLES, 0, plane->getPoints().size());
//}
//
//void renderQuad() {
//	float quadVertices[] = {
//		// 位置          // 纹理坐标
//		-1.0f, -1.0f,  0.0f, 0.0f,
//		 1.0f, -1.0f,  1.0f, 0.0f,
//		-1.0f,  1.0f,  0.0f, 1.0f,
//		 1.0f,  1.0f,  1.0f, 1.0f
//	};
//
//	GLuint quadVAO, quadVBO;
//	glGenVertexArrays(1, &quadVAO);
//	glGenBuffers(1, &quadVBO);
//
//	glBindVertexArray(quadVAO);
//	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
//
//	// 设置顶点属性
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
//
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
//
//	glBindVertexArray(0);
//
//}
TriMesh* skybox = new TriMesh();
openGLObject skyboxObject;
GLuint cubemapTexture;
void initSky() {
	skybox->generateSkybox();
	painter->bindSkyboxObjectAndData(skybox, skyboxObject, "./shaders/skyboxvert.glsl", "./shaders/skyboxfrag.glsl");
	std::vector<std::string> faces = {
		"./assets/skybox/right.jpg",
		"./assets/skybox/left.jpg",
		 "./assets/skybox/bottom.jpg",
		 "./assets/skybox/top.jpg",
		 "./assets/skybox/front.jpg",
		 "./assets/skybox/back.jpg"
	};
	cubemapTexture = painter->loadCubemap(faces);
}

void display()
{
	if (painter->isDrive) {
		painter->speed += 0.1;
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	painter->drawMeshes(light, camera);
	painter->drawSkybox(skybox, skyboxObject, camera, cubemapTexture);
}


void printHelp()
{
	std::cout << "==========================================================" << std::endl;
	std::cout << "Use mouse to control the light position (continously drag)." << std::endl;
	std::cout << "==========================================================" << std::endl << std::endl;

	std::cout << "Keyboard Usage" << std::endl;
	std::cout <<
		"[Window]" << std::endl <<
		"ESC:		Exit" << std::endl <<
		"h:		Print help message" << std::endl <<
		std::endl <<
		"[Model]" << std::endl <<
		"Space:		up" << std::endl <<
		std::endl <<
		"[Camera]" << std::endl <<
		"SPACE:		Reset camera parameters" << std::endl <<
		"u/U:		Increase/Decrease the rotate angle" << std::endl <<
		"i/I:		Increase/Decrease the up angle" << std::endl <<
		"o/O:		Increase/Decrease the camera radius" << std::endl << std::endl;

}

float fovTheta = 1.0f;

void mainWindow_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	float tmp;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	float shininess;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	else if (key == GLFW_KEY_H && action == GLFW_PRESS)
	{
		printHelp();
	}
	else if (key == GLFW_KEY_L && (action == GLFW_PRESS || action == GLFW_REPEAT) && mode == 0x0000)
	{
		std::cout << "increase exposure" << std::endl;
		painter->exposure += 1;
	}
	else if (key == GLFW_KEY_L && (action == GLFW_PRESS || action == GLFW_REPEAT) && mode == GLFW_MOD_SHIFT)
	{
		std::cout << "decrease exposure" << std::endl;
		painter->exposure -= 1;
	}
	else if (key == GLFW_KEY_V && (action == GLFW_PRESS || action == GLFW_REPEAT) && mode == 0x0000)
	{
		camera->fov += fovTheta;
		std::cout << "fov: " << camera->fov << std::endl;
	}
	else if (key == GLFW_KEY_V && (action == GLFW_PRESS || action == GLFW_REPEAT) && mode == GLFW_MOD_SHIFT)
	{
		camera->fov -= fovTheta;
		std::cout << "fov: " << camera->fov << std::endl;
	}
	else if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		camera->toggleMode();
		std::cout << "Switched to " << (camera->mode == Camera::ORBIT ? "Orbit Mode" : "Free Mode") << "\n";
	}
	else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		camera->follow = !camera->follow;
	}
	else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		if (painter->isDrive)
		{
			initSu7();
			painter->isDrive = false;
			camera->follow = false;
			painter->speed = 0.3;
			camera->eye = glm::vec4(0.7, 0.55, 0.0, 1.0);
			camera->at = glm::vec4(0.7, 0.55, -4.0, 1.0);
		}
		else
		{
			painter->isDrive = !painter->isDrive;
			camera->mode = Camera::FREE;
			camera->eye = glm::vec4(0.7, 0.55, 0.0, 1.0);
			camera->at = glm::vec4(0.7, 0.55, -4.0, 1.0);
			std::cout << "Switched to " << (painter->isDrive ? "Drive" : "Not Drive") << "\n";
		}
	}
	else
	{
		camera->keyboard(key, action, mode);
	}

}

// 重新设置窗口
void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

void cleanData() {

	painter->cleanMeshes();

	glDeleteVertexArrays(1, &skyboxObject.vao);
	glDeleteBuffers(1, &skyboxObject.vbo);
	glDeleteProgram(skyboxObject.program);
	glDeleteTextures(1, &cubemapTexture);

	delete skybox;
	skybox = NULL;
}

void setCallbacks(GLFWwindow* window, const int WIDTH, const int HEIGHT) {
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, mainWindow_key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetScrollCallback(window, scroll_callback);
	camera->setAspect(static_cast<float>(WIDTH) / static_cast<float>(HEIGHT));
	camera->updateProjectionMatrix();
	glfwSetWindowUserPointer(window, &camera);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

GLFWwindow* initGL(const int WIDTH, const int HEIGHT, const char* title) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* mainwindow = glfwCreateWindow(WIDTH, HEIGHT, title, NULL, NULL);
	glfwMakeContextCurrent(mainwindow);
	if (mainwindow == NULL)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		//return -1;
	}
	return mainwindow;
}

int main(int argc, char** argv)
{

	GLFWwindow* mainwindow = initGL(WIDTH, HEIGHT, "2023193004-RichardLiu-Final");
	setCallbacks(mainwindow, WIDTH, HEIGHT);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initMeshialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	//cube->readOff("./assets/sphere.off");
	//cube->generateCube();
	// Init mesh, shaders, buffer
	init();
	initSky();
	//initMesh("cube");
	printHelp();
	// bind callbacks
	while (!glfwWindowShouldClose(mainwindow))
	{

		display();
		glfwSwapBuffers(mainwindow);
		glfwPollEvents();

	}
	glfwTerminate();
	return 0;
}