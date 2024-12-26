#include "MeshPainter.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
MeshPainter::MeshPainter() {};
MeshPainter::~MeshPainter() {};

std::vector<std::string> MeshPainter::getMeshNames() { return mesh_names; };
std::vector<TriMesh*> MeshPainter::getMeshes() { return meshes; };
std::vector<openGLObject> MeshPainter::getOpenGLObj() { return opengl_objects; };

void MeshPainter::bindObjectAndData(TriMesh* mesh, openGLObject& object, const std::string& texture_image_diffuse, const std::string& texture_image_normal, const std::string& texture_image_specular, const std::string& texture_image_metalness, const std::string& texture_image_roughness, const std::string& texture_image_ambientOcclusion, const std::string& vshader, const std::string& fshader)
{
	// 初始化各种对象

	std::vector<glm::vec3> points = mesh->getPoints();
	std::vector<glm::vec3> normals = mesh->getNormals();
	std::vector<glm::vec3> colors = mesh->getColors();
	std::vector<glm::vec2> textures = mesh->getTextures();
	// 创建顶点数组对象
#ifdef __APPLE__                            // for MacOS
	glGenVertexArraysAPPLE(1, &object.vao); // 分配1个顶点数组对象
	glBindVertexArrayAPPLE(object.vao);     // 绑定顶点数组对象
#else                                       // for Windows
	glGenVertexArrays(1, &object.vao); // 分配1个顶点数组对象
	glBindVertexArray(object.vao);     // 绑定顶点数组对象
#endif

	// 创建并初始化顶点缓存对象
	glGenBuffers(1, &object.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
	glBufferData(GL_ARRAY_BUFFER,
		points.size() * sizeof(glm::vec3) +
		normals.size() * sizeof(glm::vec3) +
		colors.size() * sizeof(glm::vec3) +
		textures.size() * sizeof(glm::vec2),
		NULL, GL_STATIC_DRAW);

	// 绑定顶点数据
	glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(glm::vec3), points.data());
	// 绑定颜色数据
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), colors.size() * sizeof(glm::vec3), colors.data());
	// 绑定法向量数据
	glBufferSubData(GL_ARRAY_BUFFER, (points.size() + colors.size()) * sizeof(glm::vec3), normals.size() * sizeof(glm::vec3), normals.data());
	// 绑定纹理数据
	glBufferSubData(GL_ARRAY_BUFFER, (points.size() + normals.size() + colors.size()) * sizeof(glm::vec3), textures.size() * sizeof(glm::vec2), textures.data());

	object.vshader = vshader;
	object.fshader = fshader;
	object.program = InitShader(object.vshader.c_str(), object.fshader.c_str());

	// 将顶点传入着色器
	object.pLocation = glGetAttribLocation(object.program, "vPosition");
	glEnableVertexAttribArray(object.pLocation);
	glVertexAttribPointer(object.pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// 将颜色传入着色器
	object.cLocation = glGetAttribLocation(object.program, "vColor");
	glEnableVertexAttribArray(object.cLocation);
	glVertexAttribPointer(object.cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(glm::vec3)));

	// 将法向量传入着色器
	object.nLocation = glGetAttribLocation(object.program, "vNormal");
	glEnableVertexAttribArray(object.nLocation);
	glVertexAttribPointer(object.nLocation, 3,
		GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET((points.size() + colors.size()) * sizeof(glm::vec3)));

	object.tLocation = glGetAttribLocation(object.program, "vTexture");
	glEnableVertexAttribArray(object.tLocation);
	glVertexAttribPointer(object.tLocation, 2,
		GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET((points.size() + colors.size() + normals.size()) * sizeof(glm::vec3)));

	// 获得矩阵位置
	object.modelLocation = glGetUniformLocation(object.program, "model");
	object.viewLocation = glGetUniformLocation(object.program, "view");
	object.projectionLocation = glGetUniformLocation(object.program, "projection");

	object.shadowLocation = glGetUniformLocation(object.program, "isShadow");

	// 读取纹理图片数
	object.texture_image_diffuse = texture_image_diffuse;
	object.texture_image_normal = texture_image_normal;
	object.texture_image_specular = texture_image_specular;
	object.texture_image_metalness = texture_image_metalness;
	object.texture_image_roughness = texture_image_roughness;
	object.texture_image_ambientOcclusion = texture_image_ambientOcclusion;
	// 创建纹理的缓存对象
	glGenTextures(1, &object.texture_diffuse);
	glGenTextures(1, &object.texture_normal);
	glGenTextures(1, &object.texture_specular);
	glGenTextures(1, &object.texture_metalness);
	glGenTextures(1, &object.texture_roughness);
	glGenTextures(1, &object.texture_ambientOcclusion);
	// 调用stb_image生成纹理
	load_texture_STBImage(object.texture_image_diffuse, object.texture_diffuse);
	load_texture_STBImage(object.texture_image_normal, object.texture_normal);
	load_texture_STBImage(object.texture_image_specular, object.texture_specular);
	load_texture_STBImage(object.texture_image_metalness, object.texture_metalness);
	load_texture_STBImage(object.texture_image_roughness, object.texture_roughness);
	load_texture_STBImage(object.texture_image_ambientOcclusion, object.texture_ambientOcclusion);

	// Clean up
	glUseProgram(0);
#ifdef __APPLE__
	glBindVertexArrayAPPLE(0);
#else
	glBindVertexArray(0);
#endif
};

void MeshPainter::bindLightAndMaterial(TriMesh* mesh, openGLObject& object, Light* light, Camera* camera)
{

	// 传递材质、光源等数据给着色器

	// 传递相机的位置
	glUniform3fv(glGetUniformLocation(object.program, "eye_position"), 1, &camera->eye[0]);

	// 传递物体的材质
	glm::vec4 meshAmbient = mesh->getAmbient();
	glm::vec4 meshDiffuse = mesh->getDiffuse();
	glm::vec4 meshSpecular = mesh->getSpecular();
	float meshShininess = mesh->getShininess();
	float meshRoughness = mesh->getRoughness(); // 新增粗糙度
	float meshMetalness = mesh->getMetalness(); // 新增金属度
	float opacity = mesh->getOpacity();

	glUniform4fv(glGetUniformLocation(object.program, "material.ambient"), 1, &meshAmbient[0]);
	glUniform4fv(glGetUniformLocation(object.program, "material.diffuse"), 1, &meshDiffuse[0]);
	glUniform4fv(glGetUniformLocation(object.program, "material.specular"), 1, &meshSpecular[0]);
	glUniform1f(glGetUniformLocation(object.program, "material.shininess"), meshShininess);
	glUniform1f(glGetUniformLocation(object.program, "material.roughness"), meshRoughness); // 传递粗糙度
	glUniform1f(glGetUniformLocation(object.program, "material.metalness"), meshMetalness); // 传递金属度
	glUniform1f(glGetUniformLocation(object.program, "material.opacity"), opacity);

	// 传递光源信息
	glm::vec4 lightAmbient = light->getAmbient();
	glm::vec4 lightDiffuse = light->getDiffuse();
	glm::vec4 lightSpecular = light->getSpecular();
	glm::vec3 lightPosition = light->getTranslation();
	glUniform4fv(glGetUniformLocation(object.program, "light.ambient"), 1, &lightAmbient[0]);
	glUniform4fv(glGetUniformLocation(object.program, "light.diffuse"), 1, &lightDiffuse[0]);
	glUniform4fv(glGetUniformLocation(object.program, "light.specular"), 1, &lightSpecular[0]);
	glUniform3fv(glGetUniformLocation(object.program, "light.position"), 1, &lightPosition[0]);

	glUniform1f(glGetUniformLocation(object.program, "light.constant"), light->getConstant());
	glUniform1f(glGetUniformLocation(object.program, "light.linear"), light->getLinear());
	glUniform1f(glGetUniformLocation(object.program, "light.quadratic"), light->getQuadratic());

	// 设置平行光的方向
	glm::vec3 lightDirection = glm::vec3(-1.0f, -1.0f, -0.5f); // 例如，光线从右上角射向左下角
	lightDirection = glm::normalize(lightDirection);

	// 将平行光方向传递给着色器
	GLuint lightDirLocation = glGetUniformLocation(object.program, "light.direction");
	glUniform3fv(lightDirLocation, 1, &lightDirection[0]);
}

void MeshPainter::addMesh(TriMesh* mesh, const std::string& name, const std::string& texture_image_diffuse, const std::string& texture_image_normal, const std::string& texture_image_specular, const std::string& texture_image_metalness, const std::string& texture_image_roughness, const std::string& texture_image_ambientOcclusion, const std::string& vshader, const std::string& fshader)
{
	mesh_names.push_back(name);
	meshes.push_back(mesh);

	openGLObject object;
	// 绑定openGL对象，并传递顶点属性的数据
	bindObjectAndData(mesh, object, texture_image_diffuse, texture_image_normal, texture_image_specular, texture_image_metalness, texture_image_roughness, texture_image_ambientOcclusion, vshader, fshader);

	opengl_objects.push_back(object);
}

void MeshPainter::drawMesh(TriMesh* mesh, openGLObject& object, Light* light, Camera* camera)
{

	// 相机矩阵计算
	camera->updateCamera();
	camera->viewMatrix = camera->getViewMatrix();
	camera->projMatrix = camera->getProjectionMatrix(false);

#ifdef __APPLE__ // for MacOS
	glBindVertexArrayAPPLE(object.vao);
#else
	glBindVertexArray(object.vao);
#endif
	glUseProgram(object.program);

	// 物体的变换矩阵
	glm::mat4 modelMatrix = mesh->getModelMatrix();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -(mesh->getClowest()), 0.0));

	// 传递矩阵
	glUniformMatrix4fv(object.modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(object.viewLocation, 1, GL_TRUE, &camera->viewMatrix[0][0]);
	glUniformMatrix4fv(object.projectionLocation, 1, GL_TRUE, &camera->projMatrix[0][0]);
	// 将着色器 isShadow 设置为0，表示正常绘制的颜色，如果是1着表示阴影
	glUniform1i(object.shadowLocation, 0);

	glUniform1f(glGetUniformLocation(object.program, "exposure"), exposure);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object.texture_diffuse); // 该语句必须，否则将只使用同一个纹理进行绘制

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, object.texture_normal);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, object.texture_specular);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, object.texture_metalness);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, object.texture_roughness);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, object.texture_ambientOcclusion);
	// 传递纹理数据 将生成的纹理传给shader
	glUniform1i(glGetUniformLocation(object.program, "diffuseTexture"), 0);
	glUniform1i(glGetUniformLocation(object.program, "normalTexture"), 1);
	glUniform1i(glGetUniformLocation(object.program, "specularTexture"), 2);
	glUniform1i(glGetUniformLocation(object.program, "metalnessTexture"), 3);
	glUniform1i(glGetUniformLocation(object.program, "roughnessTexture"), 4);
	glUniform1i(glGetUniformLocation(object.program, "ambientOcclusionTexture"), 5);

	// 将材质和光源数据传递给着色器
	bindLightAndMaterial(mesh, object, light, camera);
	// 绘制
	glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());

	//// 阴影绘制
	glm::mat4 shadowProjMatrix = light->getShadowProjectionMatrix();

	//// 计算阴影的模型变换矩阵。
	shadowProjMatrix = shadowProjMatrix * modelMatrix;
	glUniform1i(object.shadowLocation, 1);
	glUniformMatrix4fv(object.modelLocation, 1, GL_FALSE, &shadowProjMatrix[0][0]);
	glUniformMatrix4fv(object.viewLocation, 1, GL_TRUE, &camera->viewMatrix[0][0]);
	glUniformMatrix4fv(object.projectionLocation, 1, GL_TRUE, &camera->projMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());

	// 将材质和光源数据传递给着色器
	bindLightAndMaterial(mesh, object, light, camera);
	// 绘制
	glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());

#ifdef __APPLE__ // for MacOS
	glBindVertexArrayAPPLE(0);
#else
	glBindVertexArray(0);
#endif
	glUseProgram(0);
};

void MeshPainter::drawMesh_background(TriMesh* mesh, openGLObject& object, Light* light, Camera* camera)
{
	// 相机矩阵计算
	camera->updateCamera();
	camera->viewMatrix = camera->getViewMatrix();
	camera->projMatrix = camera->getProjectionMatrix(false);

#ifdef __APPLE__ // for MacOS
	glBindVertexArrayAPPLE(object.vao);
#else
	glBindVertexArray(object.vao);
#endif
	glUseProgram(object.program);

	glUniform1f(glGetUniformLocation(object.program, "exposure"), exposure);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object.texture_diffuse); // 该语句必须，否则将只使用同一个纹理进行绘制

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, object.texture_normal);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, object.texture_specular);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, object.texture_metalness);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, object.texture_roughness);

	// glActiveTexture(GL_TEXTURE5);
	// glBindTexture(GL_TEXTURE_2D, object.texture_ambientOcclusion);
	//  传递纹理数据 将生成的纹理传给shader
	glUniform1i(glGetUniformLocation(object.program, "diffuseTexture"), 0);
	glUniform1i(glGetUniformLocation(object.program, "normalTexture"), 1);
	glUniform1i(glGetUniformLocation(object.program, "specularTexture"), 2);
	glUniform1i(glGetUniformLocation(object.program, "metalnessTexture"), 3);
	glUniform1i(glGetUniformLocation(object.program, "roughnessTexture"), 4);
	// glUniform1i(glGetUniformLocation(object.program, "ambientOcclusionTexture"), 5);

	glm::mat4 modelMatrix = mesh->getModelMatrix();

	glUniformMatrix4fv(object.modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(object.viewLocation, 1, GL_TRUE, &camera->viewMatrix[0][0]);
	glUniformMatrix4fv(object.projectionLocation, 1, GL_TRUE, &camera->projMatrix[0][0]);
	glUniform1i(object.shadowLocation, 0);
	bindLightAndMaterial(mesh, object, light, camera);
	glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());
}

void MeshPainter::drawMeshes(Light* light, Camera* camera)
{
	if (isDrive) {
		camera->driving(speed / 200);
	}
	for (int i = 0; i < meshes.size(); i++)
	{
		if (mesh_names[i] == "plane")
			drawMesh_background(meshes[i], opengl_objects[i], light, camera);
		else if (mesh_names[i].find("su7") != mesh_names[i].npos) {
			if (!isDrive) {
				drawMesh(meshes[i], opengl_objects[i], light, camera);
			}
			else {
				TriMesh* mesh = meshes[i];
				glm::mat4 model = mesh->getModelMatrix();
				glm::mat4 modelNew = model;
				modelNew = glm::translate(modelNew, glm::vec3(0, -speed, 0));
				mesh->setModelMatrix(modelNew);
				drawMesh(mesh, opengl_objects[i], light, camera);
			}
		}
		else
			drawMesh(meshes[i], opengl_objects[i], light, camera);
		if (mesh_names[i] == "tree") {
			for (int j = 1; j < 30; j++) {
				TriMesh* mesh = meshes[i];
				glm::mat4 model = mesh->getModelMatrix();
				glm::mat4 modelNew = model;
				modelNew = glm::translate(modelNew, glm::vec3(0, 0, -5000 * j));
				mesh->setModelMatrix(modelNew);
				openGLObject object;
				mesh->setModelMatrix(modelNew);
				drawMesh(mesh, opengl_objects[i], light, camera);
				mesh->setModelMatrix(model);
			}
		}
	}
};

void MeshPainter::cleanMeshes()
{
	// 将数据都清空释放
	mesh_names.clear();

	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i]->cleanData();

		delete meshes[i];
		meshes[i] = NULL;

#ifdef __APPLE__
		glDeleteVertexArraysAPPLE(1, &opengl_objects[i].vao);
#else
		glDeleteVertexArrays(1, &opengl_objects[i].vao);
#endif
		glDeleteBuffers(1, &opengl_objects[i].vbo);
		glDeleteProgram(opengl_objects[i].program);
	}

	meshes.clear();
	opengl_objects.clear();

};

void MeshPainter::load_texture_STBImage(const std::string& file_name, GLuint& texture)
{
	// 读取纹理图片，并将其传递给着色器

	int width, height, channels = 0;
	unsigned char* pixels = NULL;
	// 读取图片的时候先翻转一下图片，如果不设置的话显示出来是反过来的图片
	stbi_set_flip_vertically_on_load(true);
	// 读取图片数据
	pixels = stbi_load(file_name.c_str(), &width, &height, &channels, 0);

	// 调整行对齐格式
	if (width * channels % 4 != 0)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLenum format = GL_RGB;
	// 设置通道格式
	switch (channels)
	{
	case 1:
		format = GL_RED;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		format = GL_RGB;
		break;
	}

	// 绑定纹理对象
	glBindTexture(GL_TEXTURE_2D, texture);

	// 指定纹理的放大，缩小滤波，使用线性方式，即当图片放大的时候插值方式
	// 将图片的rgb数据上传给opengl
	glTexImage2D(
		GL_TEXTURE_2D,    // 指定目标纹理，这个值必须是GL_TEXTURE_2D
		0,                // 执行细节级别，0是最基本的图像级别，n表示第N级贴图细化级别
		format,           // 纹理数据的颜色格式(GPU显存)
		width,            // 宽度。早期的显卡不支持不规则的纹理，则宽度和高度必须是2^n
		height,           // 高度。早期的显卡不支持不规则的纹理，则宽度和高度必须是2^n
		0,                // 指定边框的宽度。必须为0
		format,           // 像素数据的颜色格式(CPU内存)
		GL_UNSIGNED_BYTE, // 指定像素数据的数据类型
		pixels            // 指定内存中指向图像数据的指针
	);

	// 生成多级渐远纹理，多消耗1/3的显存，较小分辨率时获得更好的效果
	glGenerateMipmap(GL_TEXTURE_2D);

	// 指定插值方法
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// 恢复初始对齐格式
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	// 释放图形内存
	stbi_image_free(pixels);
};

void MeshPainter::bindSkyboxObjectAndData(TriMesh* mesh, openGLObject& object, const std::string& vshader, const std::string& fshader)
{
	// 创建顶点数组对象
	glGenVertexArrays(1, &object.vao);
	glBindVertexArray(object.vao);

	std::vector<glm::vec3> points = mesh->getPoints();

	// 创建并初始化顶点缓存对象
	glGenBuffers(1, &object.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(glm::vec3), points.data());

	// 绑定着色器程序
	object.vshader = vshader;
	object.fshader = fshader;
	object.program = InitShader(vshader.c_str(), fshader.c_str());

	// 将顶点传入着色器
	//object.pLocation = glGetAttribLocation(object.program, "vPosition");
	//glEnableVertexAttribArray(object.pLocation);
	//glVertexAttribPointer(object.pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// 获取矩阵位置
	object.modelLocation = glGetUniformLocation(object.program, "model");
	object.viewLocation = glGetUniformLocation(object.program, "view");
	object.projectionLocation = glGetUniformLocation(object.program, "projection");

	// 解绑
	glBindVertexArray(0);
	glUseProgram(0);
}

GLuint MeshPainter::loadCubemap(const std::vector<std::string>& faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cerr << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void MeshPainter::drawSkybox(TriMesh* mesh, openGLObject& object, Camera* camera, GLuint cubemapTexture)
{
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glUseProgram(object.program);

	// 相机矩阵计算
	camera->updateCamera();
	glm::mat4 view = glm::mat4(glm::mat3(camera->getViewMatrix())); // 移除平移部分
	glm::mat4 projection = camera->getProjectionMatrix(false);

	// 传递矩阵
	glUniformMatrix4fv(object.viewLocation, 1, GL_TRUE, &view[0][0]);
	glUniformMatrix4fv(object.projectionLocation, 1, GL_TRUE, &projection[0][0]);

	// 绑定纹理
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glUniform1i(glGetUniformLocation(object.program, "skybox"), 9);

	// 绘制
	glBindVertexArray(object.vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glBindVertexArray(0);
	glUseProgram(0);
}