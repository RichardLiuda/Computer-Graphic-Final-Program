#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include "TriMesh.h"
#include <vector>
class TriMesh;

class GlbMesh {
	aiScene* scene;
	std::vector<glm::vec3> allVertices;
	std::vector<aiMesh*> spMeshes;
	int meshesNum;
	std::vector<TriMesh*> meshes;
	GLdouble lowest;            //最低点
	GLdouble clowest;		    //初始最低点
	GLdouble highest;           //最高点
	glm::vec3 translation;			// 物体的平移参数
	glm::vec3 rotation;				// 物体的旋转参数
	glm::vec3 scale;					// 物体的缩放参数

	glm::vec4 ambient;				// 环境光
	glm::vec4 diffuse;				// 漫反射
	glm::vec4 specular;				// 镜面反射
	float shininess;			// 高光系数

public:
	void importGLB(std::string name);
	void convertToMesh();
	void initMeshes(const std::string& filename);
	void computeBounds();
	void compute();
	void setTranslation(glm::vec3 translation);
	void setRotation(glm::vec3 rotation);
	void setScale(glm::vec3 scale);
	void scaleModel(GLdouble scaleFactor); // 缩放模型
	// 设置物体材质
	glm::vec4 getAmbient();
	glm::vec4 getDiffuse();
	glm::vec4 getSpecular();
	float getShininess();

	void setAmbient(glm::vec4 _ambient);
	void setDiffuse(glm::vec4 _diffuse);
	void setSpecular(glm::vec4 _specular);
	void setShininess(float shininess);
};
