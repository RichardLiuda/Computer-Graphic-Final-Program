#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Angel.h"

class Camera
{
public:
    Camera();
    ~Camera();

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix(bool isOrtho);

    glm::mat4 lookAt(const glm::vec4 &eye, const glm::vec4 &at, const glm::vec4 &up);

    glm::mat4 ortho(const GLfloat left, const GLfloat right,
                    const GLfloat bottom, const GLfloat top,
                    const GLfloat zNear, const GLfloat zFar);

    glm::mat4 perspective(const GLfloat fov, const GLfloat aspect,
                          const GLfloat zNear, const GLfloat zFar);

    glm::mat4 frustum(const GLfloat left, const GLfloat right,
                      const GLfloat bottom, const GLfloat top,
                      const GLfloat zNear, const GLfloat zFar);

    // 每次更改相机参数后更新一下相关的数值
    void updateCamera();
    // 处理相机的键盘操作
    void keyboard(int key, int action, int mode);

    // 模视矩阵
    glm::mat4 viewMatrix;
    glm::mat4 projMatrix;

    // 相机位置参数
    float radius = 4.0;
    float rotateAngle = 180;
    float upAngle = 0.0;
    glm::vec4 eye = glm::vec4(0.0, 0.5, 2.0, 1.0);
    glm::vec4 at = glm::vec4(0.0, 0.5, 0.0, 1.0);
    glm::vec4 up = glm::vec4(0.0, 1.0, 0.0, 0.0);
    ;

    float moveSpeed = 0.15f; // 移动速度

    // 投影参数
    float zNear = 0.1;
    float zFar = 100.0;
    // 透视投影参数
    float fov = 45.0;
    float aspect = 1.0;
    // 正交投影参数
    float scale = 1.5;

    void setAspect(float aspect);

    void updateProjectionMatrix();

    void driving(float speed);

    void moveForward(float speed);

    void moveBackward(float speed);

    void moveLeft(float speed);

    void moveRight(float speed);

    void moveUp(float speed);

    void moveDown(float speed);

    enum Mode
    {
        ORBIT,
        FREE
    };
    
    Mode mode = FREE; // 默认模式为旋转视角

    bool follow = false;

    void toggleMode();

    void updateAt();

    void updateOrbitParams();

    void processMouseMovement(double xoffset, double yoffset);
};
#endif