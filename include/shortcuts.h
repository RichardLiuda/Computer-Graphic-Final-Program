#pragma once
//#include "Angel.h"
#include "TriMesh.h"
#include "Camera.h"
//#include "GLBMesh.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <implot.h>
#include <string>
//using namespace std;

#define printVec2(vec) std::cout << "(" << vec.x << ", " << vec.y << ")" << std::endl;
#define printVec3(vec) std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
#define printVec4(vec) std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")" << std::endl;
#define printMat4(mat) std::cout << mat[0][0] << " " << mat[0][1] << " " << mat[0][2] << " " << mat[0][3] << std::endl; \
	std::cout << mat[1][0] << " " << mat[1][1] << " " << mat[1][2] << " " << mat[1][3] << std::endl; \
	std::cout << mat[2][0] << " " << mat[2][1] << " " << mat[2][2] << " " << mat[2][3] << std::endl; \
	std::cout << mat[3][0] << " " << mat[3][1] << " " << mat[3][2] << " " << mat[3][3] << std::endl;


// 初始化GLFW，返回窗口
GLFWwindow* initGL(const int WIDTH, const int HEIGHT, const char* title);


void bindLightAndMaterial(TriMesh* mesh, openGLObject& object, Light* light, Camera* camera);

void bindObjectAndData(TriMesh* mesh, openGLObject& object, const std::string& vshader, const std::string& fshader, float exposure);
