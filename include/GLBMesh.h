#pragma once
#include "TriMesh.h"
#include "MeshPainter.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include <vector>
class TriMesh;
class MeshPainter;
class Camera;

struct Texture
{
    GLuint id;
    std::string type;
    std::string path;
};

struct Material
{
    aiMaterial *material;
    std::string diffuseTexturePath;
    std::string normalTexturePath;
    std::string specularTexturePath;
    std::string metalnessTexturePath;        // ��������ͼ
    std::string roughnessTexturePath;        // �ֲڶ���ͼ
    std::string ambientOcclusionTexturePath; // �������ڱ���ͼ
    float shininess;
    float roughness; // �����ֲڶ�
    float metalness; // ����������
    // ������������
    // ������ɫ��Ϣ
    glm::vec4 ambientColor;  // ��������ɫ
    glm::vec4 diffuseColor;  // ��������ɫ
    glm::vec4 specularColor; // ���淴����ɫ
    float opacity;
};

class GlbMesh
{
    Assimp::Importer importer;
    std::vector<GlbMesh *> children;
    std::string filename;
    std::string directory;
    const aiScene *scene;
    std::vector<glm::vec3> allVertices;
    std::vector<int> materialIndexes;
    std::vector<aiMesh *> spMeshes;
    std::vector<Material> spMaterials;
    std::vector<aiTexture *> spTextures;
    std::vector<std::string> texturePath;
    std::vector<Texture> textures;
    std::vector<TriMesh *> meshes;
    int meshesNum;
    int materialsNum;
    int texturesNum;
    GLdouble lowest;       // ��͵�
    GLdouble clowest;      // ��ʼ��͵�
    GLdouble highest;      // ��ߵ�
    glm::vec3 translation; // �����ƽ�Ʋ���
    glm::vec3 rotation;    // �������ת����
    glm::vec3 scale;       // ��������Ų���

    glm::vec4 ambient;  // ������
    glm::vec4 diffuse;  // ������
    glm::vec4 specular; // ���淴��
    float shininess;    // �߹�ϵ��

public:
    void import(std::string name, std::string type);
    void setVertices();
    void initMeshes(const std::string &filename, std::string type);
    void computeBounds();
    void compute();
    void setTranslation(glm::vec3 translation);
    void setRotation(glm::vec3 rotation);
    void setScale(glm::vec3 scale);
    glm::vec3 getTranslation() { return translation; };
    void scaleModel(GLdouble scaleFactor); // ����ģ��
    // �����������
    glm::vec4 getAmbient();
    glm::vec4 getDiffuse();
    glm::vec4 getSpecular();
    float getShininess();

    void setMaterial();
    void getTexture();

    glm::vec3 getRotation();
    glm::vec3 getGlbRotation();

    glm::mat4 getGlbMatrix();

    void setAmbient(glm::vec4 _ambient);
    void setDiffuse(glm::vec4 _diffuse);
    void setSpecular(glm::vec4 _specular);
    void setShininess(float shininess);

    std::vector<TriMesh *> getMeshes();
    std::string getFilename();
    std::string getDirectoryPath(std::string &filename);
    std::vector<int> getMaterialIndexes();
    std::vector<std::string> getTexturePath();
    std::vector<Material> getMaterials();

    void addChild(GlbMesh *child);
    glm::mat4 getModelMatrix();
    void updateAndLoad(MeshPainter *painter, glm::mat4 modelMatrix, Camera *camera, Light *light, std::string name, std::string vshader, std::string fshader);
};
