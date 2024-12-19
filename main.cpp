
#include "shortcuts.h"
#include "TriMesh.h"
//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>

#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>

int WIDTH = 1920;
int HEIGHT = 1080;

int mainWindow;

float exposure = 1.0f;



//glm::vec3 light_position = glm::vec3(0.0, 2, 1.0);

openGLObject su7_object;
openGLObject furina_object;
openGLObject canon_object;
openGLObject cube_object;
std::vector<openGLObject> mesh_objects = { su7_object, furina_object, canon_object, cube_object };
openGLObject plane_object;

Light* light = new Light();
GlbMesh* test = new GlbMesh();
//TriMesh* mesh = new TriMesh();
TriMesh* su7 = new TriMesh();
TriMesh* furina = new TriMesh();
TriMesh* canon = new TriMesh();
TriMesh* cube = new TriMesh();
std::vector<TriMesh*> meshes = { su7, furina, canon, cube };
std::unordered_map<std::string, int> meshName = { {"su7", 0}, {"furina", 1}, {"canon", 2}, {"cube", 3 } };
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
		double x = xpos - lastX, y = ypos - lastY;
		glfwGetCursorPos(window, &x, &y);
		// if (!is_click)
		// {
		float half_winx = WIDTH / 2.0;
		float half_winy = HEIGHT / 2.0;
		float lx = float(x - half_winx) / half_winx;
		float ly = float(HEIGHT - y - half_winy) / half_winy;

		glm::vec3 pos = light->getTranslation();
		GLdouble lowest = 0x3f3f3f3f;
		GLdouble highest = -0x3f3f3f3f;
		bool flag = false;
		for (auto mesh : meshes)
		{
			if (mesh && mesh->getIsDisplay()) {
				flag = true;
				if (highest < mesh->getHighest())
					highest = mesh->getHighest();
				if (lowest > mesh->getLowest())
					lowest = mesh->getLowest();
			}
		}
		if (flag)
		{
			pos.x = lx;
			pos.y = std::max((highest - lowest) / 2 + 0.01, ly + 1 - lowest); // 防止光线角度过低导致渲染出错
		}
		else
		{
			pos.x = lx;
			pos.y = std::max((float)0.01, ly + 1);
		}
		light->setTranslation(pos);
		lastX = xpos;
		lastY = ypos;
		// }
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
	GLdouble scaleFactor = 1.0f + yoffset * 0.1f; // 调整缩放速度

	glm::vec3 pos = light->getTranslation();
	for (auto mesh : meshes)
		if (mesh->getIsDisplay())
		{
			mesh->scaleModel(scaleFactor);
			pos.y = std::max((float)(mesh->getHighest() - mesh->getLowest() + 0.1) / 2, pos.y);
		}

	light->setTranslation(pos);
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
		mesh->setTranslation(glm::vec3(0.0, 0.0, 0.0));
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

void initMesh(std::string name = "") {
	std::string vshader, fshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";

	TriMesh* mesh = new TriMesh();
	setOptions(name, mesh);
	std::string path = "./assets/";
	path += name;
	path += ".off";
	mesh->readOff(path);
	lowest[meshName[name]] = mesh->getClowest();
	mesh->setIsDisplay(true);
	meshes[meshName[name]] = mesh;
	// 将物体的顶点数据传递
	bindObjectAndData(mesh, mesh_objects[meshName[name]], vshader, fshader, exposure);
}

//void initGLB() {
//	Assimp::Importer importer;
//	const aiScene* scene = importer.ReadFile("./assets/su7.glb", aiProcess_Triangulate);
//
//	if (!scene) {
//		std::cerr << "Error: " << importer.GetErrorString() << std::endl;
//		return;
//	}
//
//	std::cout << "Model loaded successfully!" << std::endl;
//}

GLuint hdrFBO;
GLuint colorBuffer;
GLuint toneMappingShader;
void initHDR() {
	// HDR
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	// 创建一个浮点纹理来存储颜色缓冲
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

	// 创建深度和模板缓冲
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	toneMappingShader = InitShader("shaders/vshader", "shaders/fshader");
}

void init(std::string name = "")
{

	std::string vshader, fshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";

	// 设置光源位置
	light->setTranslation(glm::vec3(0, 1.5, 1));
	light->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 环境光
	light->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 漫反射
	light->setSpecular(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 镜面反射


	// 创建正方形平面，给它一个其他颜色
	plane->generateSquare(glm::vec3(1, 1, 1));
	// 设置正方形的位置和旋转，注意这里我们将正方形平面下移了一点点距离，
	// 这是为了防止和阴影三角形重叠在同个平面上导致颜色交叉
	plane->setRotation(glm::vec3(0, 0, 0));
	plane->setTranslation(glm::vec3(0, -0.001, 0));
	plane->setScale(glm::vec3(3, 3, 3));

	plane->setAmbient(glm::vec4(0.105882f, 0.058824f, 0.113725f, 1.0f));
	plane->setDiffuse(glm::vec4(0.427451f, 0.470588f, 0.541176f, 1.0f));
	plane->setSpecular(glm::vec4(0.333333f, 0.333333f, 0.521569f, 1.0f));
	plane->setShininess(9.84615f); //高光系数

	bindObjectAndData(plane, plane_object, vshader, fshader, exposure);

	glClearColor(0.2, 0.2, 0.2, 1);

	//initHDR();

}


void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 相机矩阵计算
	camera->updateCamera();
	camera->viewMatrix = camera->getViewMatrix();
	camera->projMatrix = camera->getProjectionMatrix(false);

	for (int i = 0; i <= 3; i++)
	{
		TriMesh* mesh = meshes[i];
		openGLObject mesh_object = mesh_objects[i];
		glBindVertexArray(mesh_object.vao);
		glUseProgram(mesh_object.program);

		// 物体的变换矩阵
		if (mesh && mesh->getIsDisplay())
		{
			glm::mat4 modelMatrix = mesh->getModelMatrix();
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -lowest[i], 0.0));// 将物体平移到y=0平面上

			// 传递矩阵
			glUniformMatrix4fv(mesh_object.modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
			glUniformMatrix4fv(mesh_object.viewLocation, 1, GL_TRUE, &camera->viewMatrix[0][0]);
			glUniformMatrix4fv(mesh_object.projectionLocation, 1, GL_TRUE, &camera->projMatrix[0][0]);
			// 将着色器 isShadow 设置为0，表示正常绘制的颜色，如果是1着表示阴影
			glUniform1i(mesh_object.shadowLocation, 0);
			glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());

			//// 阴影绘制
			glm::mat4 shadowProjMatrix = light->getShadowProjectionMatrix();

			//// 计算阴影的模型变换矩阵。
			shadowProjMatrix = shadowProjMatrix * modelMatrix;
			glUniform1i(mesh_object.shadowLocation, 1);
			glUniformMatrix4fv(mesh_object.modelLocation, 1, GL_FALSE, &shadowProjMatrix[0][0]);
			glUniformMatrix4fv(mesh_object.viewLocation, 1, GL_TRUE, &camera->viewMatrix[0][0]);
			glUniformMatrix4fv(mesh_object.projectionLocation, 1, GL_TRUE, &camera->projMatrix[0][0]);
			glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());

			// 将材质和光源数据传递给着色器
			bindLightAndMaterial(mesh, mesh_object, light, camera);
			// 绘制
			glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());
		}
	}



	// 绘制平面
	glBindVertexArray(plane_object.vao);
	glUseProgram(plane_object.program);

	glm::mat4 modelMatrix = plane->getModelMatrix();

	glUniformMatrix4fv(plane_object.modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(plane_object.viewLocation, 1, GL_TRUE, &camera->viewMatrix[0][0]);
	glUniformMatrix4fv(plane_object.projectionLocation, 1, GL_TRUE, &camera->projMatrix[0][0]);
	glUniform1i(plane_object.shadowLocation, 0);
	bindLightAndMaterial(plane, plane_object, light, camera);
	glDrawArrays(GL_TRIANGLES, 0, plane->getPoints().size());
}

void renderQuad() {
	float quadVertices[] = {
		// 位置          // 纹理坐标
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	GLuint quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	// 设置顶点属性
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);

}

void display()
{
	//glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderScene();
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//// 应用色调映射
	//glUseProgram(toneMappingShader);

	//// 绑定浮点颜色缓冲纹理
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, colorBuffer);
	//glUniform1i(glGetUniformLocation(toneMappingShader, "hdrBuffer"), 0);

	//renderQuad();
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
		"Space:		Reset material parameters" << std::endl <<
		"1/2/3/!/@/#:	Change ambient parameters" << std::endl <<
		"4/5/6/$/%/^:	Change diffuse parameters" << std::endl <<
		"7/8/9/&/*/(:	Change specular parameters" << std::endl <<
		"0/):		    Change shininess parameters" << std::endl <<
		std::endl <<
		"f/F:		Load/Hide furina model(maybe take some time)" << std::endl <<
		"s/S:		Load/Hide su7 model(maybe take some time)" << std::endl <<
		"c/C:		Load/Hide canon camera model(maybe take some time)" << std::endl <<
		"a/A:		Load/Hide cube model" << std::endl <<
		std::endl <<
		"[Camera]" << std::endl <<
		"SPACE:		Reset camera parameters" << std::endl <<
		"u/U:		Increase/Decrease the rotate angle" << std::endl <<
		"i/I:		Increase/Decrease the up angle" << std::endl <<
		"o/O:		Increase/Decrease the camera radius" << std::endl << std::endl;

}

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
	//else if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	//{
	//	std::cout << "read sphere.off" << std::endl;
	//	mesh = new TriMesh();
	//	//mesh->readOff("./assets/sphere.off");
	//	initMesh("sphere");
	//}
	//else if (key == GLFW_KEY_A && action == GLFW_PRESS)
	//{
	//	std::cout << "read Pikachu.off" << std::endl;
	//	mesh = new TriMesh();
	//	//mesh->readOff("./assets/Pikachu.off");
	//	initMesh("Pikachu");
	//}
	//else if (key == GLFW_KEY_W && action == GLFW_PRESS)
	//{
	//	std::cout << "read Squirtle.off" << std::endl;
	//	mesh = new TriMesh();
	//	//mesh->readOff("./assets/Squirtle.off");
	//	initMesh("Squirtle");
	//}
	//else if (key == GLFW_KEY_S && action == GLFW_PRESS)
	//{
	//	std::cout << "read sphere_coarse.off" << std::endl;
	//	mesh = new TriMesh();
	//	//mesh->readOff("./assets/sphere_coarse.off");
	//	initMesh("sphere_coarse");
	//}
	//else if (key == GLFW_KEY_C && action == GLFW_PRESS)
	//{
	//	std::cout << "read computer.off" << std::endl;
	//	mesh = new TriMesh();
	//	//mesh->readOff("./assets/computer.off");
	//	initMesh("computer");
	//}
	else if (key == GLFW_KEY_C && action == GLFW_PRESS && mode == 0x0000)
	{
		std::cout << "read canon.off" << std::endl;
		initMesh("canon");
	}
	else if (key == GLFW_KEY_C && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		std::cout << "hide canon.off" << std::endl;
		meshes[meshName["canon"]]->setIsDisplay(false);
	}
	else if (key == GLFW_KEY_F && action == GLFW_PRESS && mode == 0x0000)
	{
		std::cout << "read furina.off" << std::endl;
		initMesh("furina");
	}
	else if (key == GLFW_KEY_F && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		std::cout << "hide furina.off" << std::endl;
		meshes[meshName["furina"]]->setIsDisplay(false);
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS && mode == 0x0000)
	{
		std::cout << "read su7.off" << std::endl;
		initMesh("su7");
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		std::cout << "hide su7.off" << std::endl;
		meshes[meshName["su7"]]->setIsDisplay(false);
	}
	else if (key == GLFW_KEY_A && action == GLFW_PRESS && mode == 0x0000)
	{
		std::cout << "read cube.off" << std::endl;
		initMesh("cube");
	}
	else if (key == GLFW_KEY_A && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		std::cout << "hide cube.off" << std::endl;
		meshes[meshName["cube"]]->setIsDisplay(false);
	}
	else if (key == GLFW_KEY_L && action == GLFW_PRESS && mode == 0x0000)
	{
		std::cout << "increase exposure" << std::endl;
		exposure += 0.1;
	}
	else if (key == GLFW_KEY_L && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		std::cout << "decrease exposure" << std::endl;
		exposure -= 0.1;
	}
	else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		for (auto mesh : meshes) {
			mesh->setIsDisplay(false);
		}
	}
	else if (key == GLFW_KEY_1 && action == GLFW_PRESS && mode == 0x0000)
	{
		for (auto mesh : meshes)
		{
			if (mesh->getIsDisplay())
			{
				ambient = mesh->getAmbient();
				tmp = ambient.x;
				ambient.x = std::min(tmp + 0.1, 1.0);
				mesh->setAmbient(ambient);
			}
		}
	}
	else if (key == GLFW_KEY_1 && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		for (auto mesh : meshes)
			if (mesh->getIsDisplay())
			{
				ambient = mesh->getAmbient();
				tmp = ambient.x;
				ambient.x = std::min(tmp - 0.1, 1.0);
				mesh->setAmbient(ambient);
			}
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS && mode == 0x0000)
	{
		for (auto mesh : meshes)
			if (mesh->getIsDisplay())
			{
				ambient = mesh->getAmbient();
				tmp = ambient.y;
				ambient.y = std::min(tmp + 0.1, 1.0);
				mesh->setAmbient(ambient);
			}
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		for (auto mesh : meshes)
			if (mesh->getIsDisplay()) {
				ambient = mesh->getAmbient();
				tmp = ambient.y;
				ambient.y = std::min(tmp - 0.1, 1.0);
				mesh->setAmbient(ambient);
			}
	}
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS && mode == 0x0000)
	{
		for (auto mesh : meshes)
			if (mesh->getIsDisplay()) {
				ambient = mesh->getAmbient();
				tmp = ambient.z;
				ambient.z = std::min(tmp + 0.1, 1.0);
				mesh->setAmbient(ambient);
			}
	}
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		for (auto mesh : meshes)
			if (mesh->getIsDisplay()) {
				ambient = mesh->getAmbient();
				tmp = ambient.z;
				ambient.z = std::min(tmp - 0.1, 1.0);
				mesh->setAmbient(ambient);
			}
	}
	else if (key == GLFW_KEY_4 && action == GLFW_PRESS && mode == 0x0000)
	{
		for (auto mesh : meshes)
			if (mesh->getIsDisplay()) {
				mesh->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0));
				mesh->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0));
				mesh->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0));
				mesh->setShininess(1.0);
			}
	}
	else if (key == GLFW_KEY_4 && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		for (auto mesh : meshes)
			if (mesh->getIsDisplay()) {
				mesh->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0));
				mesh->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0));
				mesh->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0));
				mesh->setShininess(1.0);
			}
	}
	else if (key == GLFW_KEY_5 && action == GLFW_PRESS && mode == 0x0000)
	{
		for (auto mesh : meshes)
			if (mesh->getIsDisplay()) {
				diffuse = mesh->getDiffuse();
				tmp = diffuse.y;
				diffuse.y = std::min(tmp + 0.1, 1.0);
				mesh->setDiffuse(diffuse);
			}
	}
	else if (key == GLFW_KEY_5 && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT) {
		for (auto mesh : meshes)
			if (mesh->getIsDisplay()) {
				diffuse = mesh->getDiffuse();
				tmp = diffuse.y;
				diffuse.y = std::max(tmp - 0.1, 0.0);
				mesh->setDiffuse(diffuse);
			}
	}
	else if (key == GLFW_KEY_6 && action == GLFW_PRESS && mode == 0x0000) {
		for (auto mesh : meshes)
			if (mesh->getIsDisplay()) {
				diffuse = mesh->getDiffuse();
				tmp = diffuse.z;
				diffuse.z = std::min(tmp + 0.1, 1.0);
				mesh->setDiffuse(diffuse);
			}
	}
	else if (key == GLFW_KEY_6 && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT) {
		for (auto mesh : meshes)
			if (mesh->getIsDisplay()) {
				diffuse = mesh->getDiffuse();
				tmp = diffuse.z;
				diffuse.z = std::max(tmp - 0.1, 0.0);
				mesh->setDiffuse(diffuse);
			}
	}
	else if (key == GLFW_KEY_7 && action == GLFW_PRESS && mode == 0x0000) {
		for (auto mesh : meshes)
			if (mesh->getIsDisplay()) {
				specular = mesh->getSpecular();
				tmp = specular.x;
				specular.x = std::min(tmp + 0.1, 1.0);
				mesh->setSpecular(specular);
			}
	}
	else if (key == GLFW_KEY_7 && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT) {
		for (auto mesh : meshes)
			if (mesh->getIsDisplay()) {
				specular = mesh->getSpecular();
				tmp = specular.x;
				specular.x = std::max(tmp - 0.1, 0.0);
				mesh->setSpecular(specular);
			}
	}
	else if (key == GLFW_KEY_8 && action == GLFW_PRESS && mode == 0x0000) {
		for (auto mesh : meshes)
			if (mesh->getIsDisplay()) {
				specular = mesh->getSpecular();
				tmp = specular.y;
				specular.y = std::min(tmp + 0.1, 1.0);
				mesh->setSpecular(specular);
			}
	}
	else if (key == GLFW_KEY_8 && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT) {
		for (auto mesh : meshes)
			if (mesh->getIsDisplay()) {
				specular = mesh->getSpecular();
				tmp = specular.y;
				specular.y = std::max(tmp - 0.1, 0.0);
				mesh->setSpecular(specular);
			}
	}
	else if (key == GLFW_KEY_9 && action == GLFW_PRESS && mode == 0x0000) {
		for (auto mesh : meshes)
			if (mesh->getIsDisplay()) {
				specular = mesh->getSpecular();
				tmp = specular.z;
				specular.z = std::min(tmp + 0.1, 1.0);
				mesh->setSpecular(specular);
			}
	}
	else if (key == GLFW_KEY_9 && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT) {
		for (auto mesh : meshes)
			if (mesh->getIsDisplay()) {
				specular = mesh->getSpecular();
				tmp = specular.z;
				specular.z = std::max(tmp - 0.1, 0.0);
				mesh->setSpecular(specular);
			}
	}
	else if (key == GLFW_KEY_0 && action == GLFW_PRESS && mode == 0x0000) {
		for (auto mesh : meshes)
			if (mesh->getIsDisplay()) {
				shininess = mesh->getShininess();
				tmp = shininess;
				shininess = tmp + 5;
				mesh->setShininess(shininess);
			}
	}
	else if (key == GLFW_KEY_0 && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT) {
		for (auto mesh : meshes)
			if (mesh->getIsDisplay()) {
				shininess = mesh->getShininess();
				tmp = shininess;
				shininess = tmp - 5;
				mesh->setShininess(shininess);
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
	for (auto mesh : meshes)
		if (mesh)
			mesh->cleanData();
	plane->cleanData();

	delete camera;
	camera = NULL;

	// 释放内存
	for (auto mesh : meshes)
		if (mesh)
		{
			delete mesh;
			mesh = NULL;
		}

	delete plane;
	plane = NULL;

	// 删除绑定的对象
	for (auto mesh_object : mesh_objects)
	{
		glDeleteVertexArrays(1, &mesh_object.vao);
		glDeleteBuffers(1, &mesh_object.vbo);
		glDeleteProgram(mesh_object.program);

		glDeleteVertexArrays(1, &plane_object.vao);
		glDeleteBuffers(1, &plane_object.vbo);
		glDeleteProgram(plane_object.program);
	}

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

int main(int argc, char** argv)
{

	test->initMeshes("su7");
	test->setTranslation(glm::vec3(-1.6, 0.0, 0.0));
	test->setRotation(glm::vec3(0.0, 0.0, 0.0));
	test->setScale(glm::vec3(0.01, 0.01, 0.01));
	test->setAmbient(glm::vec4(20.0 / 800, 114.0 / 800, 140.0 / 800, 0.5)); // 环境光
	test->setDiffuse(glm::vec4(0.27568f, 0.41424f, 0.57568f, 0.55f)); // 漫反射
	test->setSpecular(glm::vec4(0.633f, 0.727811f, 0.633f, 0.55f)); // 镜面反射
	test->setShininess(76.0f); //高光系数
	test->compute();

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
	initMesh("cube");
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
