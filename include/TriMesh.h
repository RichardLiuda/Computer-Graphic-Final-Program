﻿#ifndef _TRI_MESH_H_
#define _TRI_MESH_H_
#include "Angel.h"
#include "GLBMesh.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

struct openGLObject
{
	// 顶点数组对象
	GLuint vao;
	// 顶点缓存对象
	GLuint vbo;

	// 着色器程序
	GLuint program;
	// 着色器文件
	std::string vshader;
	std::string fshader;
	// 着色器变量
	GLuint pLocation;
	GLuint cLocation;
	GLuint nLocation;

	// 投影变换变量
	GLuint modelLocation;
	GLuint viewLocation;
	GLuint projectionLocation;

	// 阴影变量
	GLuint shadowLocation;
};

// 三角面片中的顶点序列
typedef struct vIndex {
	unsigned int x, y, z;
	vIndex(int ix, int iy, int iz) : x(ix), y(iy), z(iz) {}
} vec3i;

class TriMesh
{
public:
	TriMesh();
	~TriMesh();
	TriMesh(std::vector<glm::vec3> vertices, std::vector<glm::vec3> colors, std::vector<glm::vec3> normals, std::vector<vec3i> faces, std::vector<glm::vec2> textures);

	std::vector<glm::vec3> getVertexPositions();
	std::vector<glm::vec3> getVertexColors();
	std::vector<glm::vec3> getVertexNormals();
	std::vector<glm::vec2> getVertexTextures();
	std::vector<vec3i> getFaces();
	std::vector<glm::vec3> getPoints();
	std::vector<glm::vec3> getColors();
	std::vector<glm::vec3> getNormals();
	std::vector<glm::vec2> getTextures();

	// 从GLB中读取数据
	//void readGLB(const std::string& filename);

	// 新增接口方法
	GLdouble getLowest();
	void setLowest(GLdouble lowest);
	GLdouble getClowest();
	void setClowest(GLdouble clowest);
	GLdouble getHighest();
	void setHighest(GLdouble highest);
	bool getIsDisplay();
	void setIsDisplay(bool isDisplay);


	void computeTriangleNormals();
	void computeVertexNormals();
	void computeBounds();
	void normalReverse();

	// 获取和设置物体的旋转平移变化
	glm::vec3 getTranslation();
	glm::vec3 getRotation();
	glm::vec3 getScale();
	glm::mat4 getModelMatrix();

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

	// 这里添加各种形状的、模型的读取顶点的函数
	void generateCube();
	void generateTriangle(glm::vec3 color);
	void generateSquare(glm::vec3 color);
	void readOff(const std::string& filename);

	// 将读取的顶点根据三角面片上的顶点下标逐个加入
	// 要传递给GPU的points等容器内
	void storeFacesPoints();

	// 清除数据
	void cleanData();

protected:
	std::vector<glm::vec3> vertex_positions;	// 顶点坐标
	std::vector<glm::vec3> vertex_colors;	// 顶点颜色
	std::vector<glm::vec3> vertex_normals;	// 顶点法向量
	std::vector<glm::vec2> vertex_textures;	// 顶点纹理坐标，注意是vec2

	std::vector<vec3i> faces;	// 三角面片上每个顶点对应的下标
	std::vector<glm::vec3> face_normals;	// 每个三角面片的法向量

	std::vector<glm::vec3> points;	// 传入着色器的绘制点
	std::vector<glm::vec3> colors;	// 传入着色器的颜色
	std::vector<glm::vec3> normals;	// 传入着色器的法向量
	std::vector<glm::vec2> textures;	// 传入着色器的纹理坐标，注意是vec2

	glm::vec3 translation;			// 物体的平移参数
	glm::vec3 rotation;				// 物体的旋转参数
	glm::vec3 scale;					// 物体的缩放参数

	glm::vec4 ambient;				// 环境光
	glm::vec4 diffuse;				// 漫反射
	glm::vec4 specular;				// 镜面反射
	float shininess;			// 高光系数

	GLdouble lowest;            //最低点
	GLdouble clowest;		    //初始最低点
	GLdouble highest;           //最高点

	bool isDisplay;				// 是否显示

};


class Light : public TriMesh {
public:
	// 把实验3.2的阴影投影矩阵也加到Light类中
	glm::mat4 getShadowProjectionMatrix();
};

#endif