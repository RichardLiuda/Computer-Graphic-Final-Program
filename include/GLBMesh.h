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

struct Material
{
    aiMaterial *material;
    std::string diffuseTexturePath;
    std::string normalTexturePath;
    std::string specularTexturePath;
    std::string metalnessTexturePath;        // 金属度贴图
    std::string roughnessTexturePath;        // 粗糙度贴图
    std::string ambientOcclusionTexturePath; // 环境光遮蔽贴图
    float shininess;
    float roughness;         // 新增粗糙度
    float metalness;         // 新增金属度
    glm::vec4 ambientColor;  // 环境光颜色
    glm::vec4 diffuseColor;  // 漫反射颜色
    glm::vec4 specularColor; // 镜面反射颜色
    float opacity;
};

class GlbMesh
{
    Assimp::Importer importer;            // Assimp的Importer对象，用于导入3D模型文件
    std::vector<GlbMesh *> children;      // 指向GlbMesh对象的指针向量，用于存储模型的子网格
    std::string filename;                 // 字符串变量，用于存储模型文件的文件名
    std::string directory;                // 字符串变量，用于存储模型文件所在的目录
    const aiScene *scene;                 // 指向aiScene对象的常量指针，用于存储导入的3D场景
    std::vector<glm::vec3> allVertices;   // 存储glm::vec3类型向量的向量，用于存储所有顶点数据
    std::vector<int> materialIndexes;     // 存储整数类型的向量，用于存储材质索引
    std::vector<aiMesh *> spMeshes;       // 存储指向aiMesh对象的指针向量，用于存储网格
    std::vector<Material> spMaterials;    // 存储Material类型对象的向量，用于存储特殊材质
    std::vector<aiTexture *> spTextures;  // 存储指向aiTexture对象的指针向量，用于存储特殊纹理
    std::vector<std::string> texturePath; // 存储字符串类型的向量，用于存储纹理路径
    std::vector<TriMesh *> meshes;        // 存储指向TriMesh对象的指针向量，用于存储三角网格
    int meshesNum;
    int materialsNum;
    int texturesNum;
    GLdouble lowest;       // 最低点
    GLdouble clowest;      // 初始最低点
    GLdouble highest;      // 最高点
    glm::vec3 translation; // 物体的平移参数
    glm::vec3 rotation;    // 物体的旋转参数
    glm::vec3 scale;       // 物体的缩放参数

    glm::vec4 ambient;  // 环境光
    glm::vec4 diffuse;  // 漫反射
    glm::vec4 specular; // 镜面反射
    float shininess;    // 高光系数

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
    void scaleModel(GLdouble scaleFactor); // 缩放模型
    // 设置物体材质
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

    void addChild(GlbMesh *child);                                                                                                                                          // 添加子网格
    glm::mat4 getModelMatrix();                                                                                                                                             // 获取模型矩阵
    void updateAndLoad(MeshPainter *painter, glm::mat4 modelMatrix, Camera *camera, Light *light, std::string name, std::string vshader, std::string fshader, bool isLoad); // 更新并加载网格(层级建模)
};
