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

    void driving(float speed)
    {
        glm::vec3 direction = glm::normalize(glm::vec3(0.0, 0.0, -1.0));
        eye += glm::vec4(direction * speed, 0.0);
        at += glm::vec4(direction * speed, 0.0);
    }

    void moveForward(float speed)
    {
        if (mode == FREE)
        {
            glm::vec3 direction = glm::normalize(glm::vec3(at - eye));
            eye += glm::vec4(direction * speed, 0.0);
            at += glm::vec4(direction * speed, 0.0);
        }
    }

    void moveBackward(float speed)
    {
        if (mode == FREE)
        {
            glm::vec3 direction = glm::normalize(glm::vec3(at - eye));
            eye -= glm::vec4(direction * speed, 0.0);
            at -= glm::vec4(direction * speed, 0.0);
        }
    }

    void moveLeft(float speed)
    {
        if (mode == FREE)
        {
            glm::vec3 direction = glm::normalize(glm::vec3(at - eye));
            glm::vec3 right = glm::normalize(glm::cross(glm::vec3(up), direction));
            eye += glm::vec4(right * speed, 0.0);
            at += glm::vec4(right * speed, 0.0);
        }
    }

    void moveRight(float speed)
    {
        if (mode == FREE)
        {
            glm::vec3 direction = glm::normalize(glm::vec3(at - eye));
            glm::vec3 right = glm::normalize(glm::cross(glm::vec3(up), direction));
            eye -= glm::vec4(right * speed, 0.0);
            at -= glm::vec4(right * speed, 0.0);
        }
    }

    void moveUp(float speed)
    {
        if (mode == FREE)
        {
            eye += glm::vec4(glm::vec3(up) * speed, 0.0);
            at += glm::vec4(glm::vec3(up) * speed, 0.0);
        }
    }

    void moveDown(float speed)
    {
        if (mode == FREE)
        {
            eye -= glm::vec4(glm::vec3(up) * speed, 0.0);
            at -= glm::vec4(glm::vec3(up) * speed, 0.0);
        }
    }

    enum Mode
    {
        ORBIT,
        FREE
    };
    Mode mode = FREE; // 默认模式为旋转视角

    bool follow = false;

    void toggleMode()
    {
        mode = (mode == ORBIT) ? FREE : ORBIT;
        if (mode == ORBIT)
        {
            updateAt();
            updateOrbitParams();
        }
    }

    void updateAt()
    {
        // 计算 at 的位置：位于相机正前方一定距离
        glm::vec3 direction = glm::normalize(glm::vec3(at - eye));
        at = eye + glm::vec4(direction * radius, 0.0);
    }

    void updateOrbitParams();

    void processMouseMovement(double xoffset, double yoffset)
    {
        if (mode == ORBIT)
        {
            at = at + glm::vec4(-xoffset * 0.01, -yoffset * 0.01, 0.0, 0.0);
            updateOrbitParams();
            std::cout << "rotateAngle: " << rotateAngle << ", upAngle: " << upAngle << std::endl;
        }
    }
};
#endif