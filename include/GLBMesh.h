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
	GLdouble lowest;            //��͵�
	GLdouble clowest;		    //��ʼ��͵�
	GLdouble highest;           //��ߵ�
	glm::vec3 translation;			// �����ƽ�Ʋ���
	glm::vec3 rotation;				// �������ת����
	glm::vec3 scale;					// ��������Ų���

	glm::vec4 ambient;				// ������
	glm::vec4 diffuse;				// ������
	glm::vec4 specular;				// ���淴��
	float shininess;			// �߹�ϵ��

public:
	void importGLB(std::string name);
	void convertToMesh();
	void initMeshes(const std::string& filename);
	void computeBounds();
	void compute();
	void setTranslation(glm::vec3 translation);
	void setRotation(glm::vec3 rotation);
	void setScale(glm::vec3 scale);
	void scaleModel(GLdouble scaleFactor); // ����ģ��
	// �����������
	glm::vec4 getAmbient();
	glm::vec4 getDiffuse();
	glm::vec4 getSpecular();
	float getShininess();

	void setAmbient(glm::vec4 _ambient);
	void setDiffuse(glm::vec4 _diffuse);
	void setSpecular(glm::vec4 _specular);
	void setShininess(float shininess);
};
