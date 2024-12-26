#include "Camera.h"

Camera::Camera() { updateCamera(); };
Camera::~Camera() {}

glm::mat4 Camera::getViewMatrix()
{
	return this->lookAt(eye, at, up);
}

glm::mat4 Camera::getProjectionMatrix(bool isOrtho)
{
	if (isOrtho) {
		return this->ortho(-scale, scale, -scale, scale, this->zNear, this->zFar);
	}
	else {
		return this->perspective(fov, aspect, this->zNear, this->zFar);
	}
}

void Camera::setAspect(float aspect)
{
	this->aspect = aspect;
}

void Camera::updateProjectionMatrix()
{
	this->projMatrix = this->perspective(this->fov, this->aspect, this->zNear, this->zFar);
}

glm::mat4 Camera::lookAt(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up)
{
	// 获得相机方向。
	glm::vec4 n = glm::normalize(eye - at);
	// 获得右(x)轴方向。
	glm::vec3 up_3 = up;
	glm::vec3 n_3 = n;
	glm::vec4 u = glm::normalize(glm::vec4(glm::cross(up_3, n_3), 0.0));
	// 获得上(y)轴方向。
	glm::vec3 u_3 = u;
	glm::vec4 v = glm::normalize(glm::vec4(glm::cross(n_3, u_3), 0.0));

	glm::vec4 t = glm::vec4(0.0, 0.0, 0.0, 1.0);
	glm::mat4 c = glm::mat4(u, v, n, t);

	// 处理相机位置向量。
	glm::mat4 p = glm::mat4(1.0f);
	p[0].w = -(eye.x);
	p[1].w = -(eye.y);
	p[2].w = -(eye.z);

	glm::mat4 view = p * c;
	return view;
}

glm::mat4 Camera::ortho(const GLfloat left, const GLfloat right,
	const GLfloat bottom, const GLfloat top,
	const GLfloat zNear, const GLfloat zFar)
{
	// 正交投影矩阵的计算
	glm::mat4 c = glm::mat4(1.0f);
	c[0][0] = 2.0 / (right - left);
	c[1][1] = 2.0 / (top - bottom);
	c[2][2] = -2.0 / (zFar - zNear);
	c[3][3] = 1.0;
	c[0][3] = -(right + left) / (right - left);
	c[1][3] = -(top + bottom) / (top - bottom);
	c[2][3] = -(zFar + zNear) / (zFar - zNear);
	return c;
}

glm::mat4 Camera::perspective(const GLfloat fov, const GLfloat aspect,
	const GLfloat zNear, const GLfloat zFar)
{
	// 透视投影矩阵的计算
	GLfloat top = tan(fov * M_PI / 180 / 2) * zNear;
	GLfloat right = top * aspect;

	glm::mat4 c = glm::mat4(1.0f);
	c[0][0] = zNear / right;
	c[1][1] = zNear / top;
	c[2][2] = -(zFar + zNear) / (zFar - zNear);
	c[2][3] = -(2.0 * zFar * zNear) / (zFar - zNear);
	c[3][2] = -1.0;
	c[3][3] = 0.0;
	return c;
}

glm::mat4 Camera::frustum(const GLfloat left, const GLfloat right,
	const GLfloat bottom, const GLfloat top,
	const GLfloat zNear, const GLfloat zFar)
{
	// 任意视锥体矩阵
	glm::mat4 c = glm::mat4(1.0f);
	c[0][0] = 2.0 * zNear / (right - left);
	c[0][2] = (right + left) / (right - left);
	c[1][1] = 2.0 * zNear / (top - bottom);
	c[1][2] = (top + bottom) / (top - bottom);
	c[2][2] = -(zFar + zNear) / (zFar - zNear);
	c[2][3] = -2.0 * zFar * zNear / (zFar - zNear);
	c[3][2] = -1.0;
	c[3][3] = 0.0;
	return c;
}


void Camera::updateCamera()
{
	if (mode == ORBIT) {
		// 旋转视角：根据 radius、upAngle 和 rotateAngle 计算 at
		float atx = eye.x + radius * cos(upAngle * M_PI / 180.0) * sin(rotateAngle * M_PI / 180.0);
		float aty = eye.y + radius * sin(upAngle * M_PI / 180.0);
		float atz = eye.z + radius * cos(upAngle * M_PI / 180.0) * cos(rotateAngle * M_PI / 180.0);
		at = glm::vec4(atx, aty, atz, 1.0);
	}

	// 更新视图矩阵
	viewMatrix = lookAt(eye, at, up);
}

void Camera::updateOrbitParams() {
	// 计算 radius：eye 和 at 之间的距离
	radius = glm::length(glm::vec3(at - eye));

	// 计算 direction：从 at 指向 eye 的方向
	glm::vec3 direction = glm::normalize(glm::vec3(at - eye));

	// 计算 rotateAngle：绕 y 轴的旋转角度（水平方向）
	rotateAngle = glm::degrees(atan2(direction.x, direction.z));

	// 计算 upAngle：绕 x 轴的俯仰角度（垂直方向）
	upAngle = glm::degrees(asin(direction.y));
}
void Camera::keyboard(int key, int action, int mode)
{
	if (key == GLFW_KEY_U && (action == GLFW_PRESS || action == GLFW_REPEAT) && mode == 0x0000)
	{
		rotateAngle += 5.0;
	}
	else if (key == GLFW_KEY_U && (action == GLFW_PRESS || action == GLFW_REPEAT) && mode == GLFW_MOD_SHIFT)
	{
		rotateAngle -= 5.0;
	}
	else if (key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT) && mode == 0x0000)
	{
		upAngle += 5.0;
		if (upAngle > 180)
			upAngle = 180;
	}
	else if (key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT) && mode == GLFW_MOD_SHIFT)
	{
		upAngle -= 5.0;
		if (upAngle < -180)
			upAngle = -180;
	}
	else if (key == GLFW_KEY_O && (action == GLFW_PRESS || action == GLFW_REPEAT) && mode == 0x0000)
	{
		radius += 0.1;
	}
	else if (key == GLFW_KEY_O && (action == GLFW_PRESS || action == GLFW_REPEAT) && mode == GLFW_MOD_SHIFT)
	{
		radius -= 0.1;
	}
	else if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		moveForward(moveSpeed);
	}
	else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		moveBackward(moveSpeed);
	}
	else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		moveLeft(moveSpeed);
	}
	else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		moveRight(moveSpeed);
	}
	else if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		moveUp(moveSpeed);
	}
	else if (key == GLFW_KEY_LEFT_CONTROL && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		moveDown(moveSpeed);
	}
}