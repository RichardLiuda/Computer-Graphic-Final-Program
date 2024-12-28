
#include "MeshPainter.h"

#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>

int WIDTH = 1920;
int HEIGHT = 1080;

int mainWindow;

MeshPainter *painter = new MeshPainter(); // 渲染器

Light *light = new Light(); // 光照

GlbMesh *bmw = new GlbMesh(); // 建模模型列表
GlbMesh *bmw_wheel = new GlbMesh();
GlbMesh *su7 = new GlbMesh();
GlbMesh *su7_wheel1 = new GlbMesh();
GlbMesh *su7_wheel2 = new GlbMesh();
GlbMesh *su7_wheel3 = new GlbMesh();
GlbMesh *su7_wheel4 = new GlbMesh();
GlbMesh *tree = new GlbMesh();
GlbMesh *tree2 = new GlbMesh();
GlbMesh *furina = new GlbMesh();
// GlbMesh *bocchi = new GlbMesh();
// GlbMesh *nina = new GlbMesh();

TriMesh *skybox = new TriMesh(); // 天空盒
openGLObject skyboxObject;
GLuint cubemapTexture; // 天空盒纹理

TriMesh *plane = new TriMesh(); // 地面

Camera *camera = new Camera(); // 相机

float fovTheta = 1.0f; // 视角变化步长

bool mousePressed = false;
double lastX, lastY;

// 定义一个鼠标按钮回调函数，当鼠标按钮状态改变时调用
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            mousePressed = true;
            glfwGetCursorPos(window, &lastX, &lastY);
        }
        else if (action == GLFW_RELEASE)
        {
            mousePressed = false;
        }
    }
}

void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos) // 鼠标移动回调函数
{
    if (mousePressed)
    {
        double xoffset = xpos - lastX;
        double yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        camera->processMouseMovement(xoffset, yoffset); // 更新相机位置
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) // 窗口大小改变回调函数
{
    glViewport(0, 0, width, height);
    glViewport(0, 0, width, height);
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    camera->setAspect(aspect);
    camera->updateProjectionMatrix();
}

void setTexturePath(GlbMesh *glb)
{
    for (int i = 0; i < glb->getMeshes().size(); i++)
    {
        TriMesh *mesh = glb->getMeshes()[i];
        mesh->setIsDisplay(true);
        mesh->diffusePath = glb->getMaterials()[glb->getMaterialIndexes()[i]].diffuseTexturePath;
        mesh->normalPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].normalTexturePath;
        mesh->specularPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].specularTexturePath;
        mesh->metalnessPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].metalnessTexturePath;
        mesh->roughnessPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].roughnessTexturePath;
        mesh->ambientOcclusionPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].ambientOcclusionTexturePath;
    }
}

// use meshpainter
void init()
{
    std::string vshader, fshader;
    vshader = "shaders/vshader.glsl";
    fshader = "shaders/fshader.glsl";
    // 设置光源位置
    light->setTranslation(glm::vec3(50, 100, 50));
    light->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0));     // 环境光
    light->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0));     // 漫反射
    light->setSpecular(glm::vec4(10.0, 10.0, 10.0, 1.0)); // 镜面反射

    tree->initMeshes("tree", "glb"); // 路边树
    tree->setTranslation(glm::vec3(-6, 0, 0));
    tree->setRotation(glm::vec3(0, 0, 0));
    float scale = 0.005;
    tree->setScale(glm::vec3(scale, scale, scale));
    tree->compute();
    setTexturePath(tree);
    tree->updateAndLoad(painter, tree->getModelMatrix(), camera, light, "tree", vshader, fshader, true);

    tree2->initMeshes("tree", "glb"); // 路边树
    tree2->setTranslation(glm::vec3(6, 0, 0));
    tree2->setRotation(glm::vec3(0, 0, 0));
    scale = 0.005;
    tree2->setScale(glm::vec3(scale, scale, scale));
    tree2->compute();
    setTexturePath(tree2);
    tree2->updateAndLoad(painter, tree2->getModelMatrix(), camera, light, "tree", vshader, fshader, true);

    furina->initMeshes("furina", "glb"); // 路边的人
    furina->setTranslation(glm::vec3(-2, 0, 0));
    furina->setRotation(glm::vec3(0, 90, 0));
    scale = 0.5;
    furina->setScale(glm::vec3(scale, scale, scale));
    furina->compute();
    setTexturePath(furina);
    furina->updateAndLoad(painter, furina->getModelMatrix(), camera, light, "furina", vshader, fshader, true);

    bmw->initMeshes("bmw_m3", "glb"); // 车
    bmw->setTranslation(glm::vec3(-4, 0, 0));
    bmw->setRotation(glm::vec3(-90, 0, 0));
    scale = 0.14;
    bmw->setScale(glm::vec3(scale, scale, scale));
    bmw->compute();
    setTexturePath(bmw);
    bmw->updateAndLoad(painter, bmw->getModelMatrix(), camera, light, "bmw", vshader, fshader, true);

    su7->initMeshes("su7", "glb"); // 小米su7 max
    su7->setTranslation(glm::vec3(0.9, 0.0, 0.0));
    su7->setRotation(glm::vec3(90, 0.0, 0.0));
    scale = 0.005;
    su7->setScale(glm::vec3(scale, scale, scale));
    su7->compute();
    setTexturePath(su7);

    su7_wheel1->initMeshes("su7_wheel1", "glb");           // 车轮
    su7_wheel1->setTranslation(glm::vec3(0.0, -208, 0.0)); // 轮子相对于车体的位置
    su7_wheel1->setRotation(glm::vec3(0.0, 0.0, 0.0));
    su7_wheel1->setScale(glm::vec3(1, 1, 1));
    su7_wheel1->compute();
    setTexturePath(su7_wheel1);
    su7->addChild(su7_wheel1);

    su7_wheel2->initMeshes("su7_wheel2", "glb");
    su7_wheel2->setTranslation(glm::vec3(0.0, -208, 0.0)); // 轮子相对于车体的位置
    su7_wheel2->setRotation(glm::vec3(0.0, 0.0, 0.0));
    su7_wheel2->setScale(glm::vec3(1, 1, 1));
    su7_wheel2->compute();
    setTexturePath(su7_wheel2);
    su7->addChild(su7_wheel2);

    su7_wheel3->initMeshes("su7_wheel3", "glb");
    su7_wheel3->setTranslation(glm::vec3(0.0, 95, 0.0)); // 轮子相对于车体的位置
    su7_wheel3->setRotation(glm::vec3(0.0, 0.0, 0.0));
    su7_wheel3->setScale(glm::vec3(1, 1, 1));
    su7_wheel3->compute();
    setTexturePath(su7_wheel3);
    su7->addChild(su7_wheel3);

    su7_wheel4->initMeshes("su7_wheel4", "glb");
    su7_wheel4->setTranslation(glm::vec3(0.0, 95, 0.0)); // 轮子相对于车体的位置
    su7_wheel4->setRotation(glm::vec3(0.0, 0.0, 0.0));
    su7_wheel4->setScale(glm::vec3(1, 1, 1));
    su7_wheel4->compute();
    setTexturePath(su7_wheel4);
    su7->addChild(su7_wheel4);

    su7->updateAndLoad(painter, su7->getModelMatrix(), camera, light, "su7", vshader, fshader, true);

    plane->generateSquare(glm::vec3(1, 1, 1)); // 地面
    plane->setRotation(glm::vec3(0, 0, 0));
    plane->setTranslation(glm::vec3(0, -0.001, 0));
    plane->setScale(glm::vec3(10, 1, 100));
    plane->setAmbient(glm::vec4(0.105882f, 0.058824f, 0.113725f, 1.0f));
    plane->setDiffuse(glm::vec4(0.427451f, 0.470588f, 0.541176f, 1.0f));
    plane->setSpecular(glm::vec4(0.333333f, 0.333333f, 0.521569f, 1.0f));
    plane->setShininess(9.84615f); // 高光系数
    painter->addMesh(plane, "plane", "./assets/track.jpg", "", "", "", "", "", vshader, fshader);

    glClearColor(0.2, 0.2, 0.2, 1);
}

void initSu7() // 初始化小米su7 max位置
{
    su7->setTranslation(glm::vec3(0.9, 0.0, 0.0));
    su7->updateAndLoad(painter, glm::mat4(1.0), camera, light, "su7", "shaders/vshader.glsl", "shaders/fshader.glsl", false);
}

void initSky() // 初始化天空盒
{
    skybox->generateSkybox();
    painter->bindSkyboxObjectAndData(skybox, skyboxObject, "./shaders/skyboxvert.glsl", "./shaders/skyboxfrag.glsl");
    std::vector<std::string> faces = {
        "./assets/skybox/right.jpg",
        "./assets/skybox/left.jpg",
        "./assets/skybox/bottom.jpg",
        "./assets/skybox/top.jpg",
        "./assets/skybox/front.jpg",
        "./assets/skybox/back.jpg"};
    cubemapTexture = painter->loadCubemap(faces);
}

void display() // 绘制
{
    if (painter->isDrive)
    {
        painter->speed += 1; // 车加速度
    }
    if (camera->eye.z < -800)
    {
        std::cout << "Let's explore this area later! " << std::endl;
        painter->speed = 0.3;                         // 设置速度为0.3，退出驾驶模式后的默认速度
        painter->isDrive = false;                     // 将驾驶模式标志设置为false，表示退出驾驶模式
        initSu7();                                    // 初始化驾驶模式相关的设置或资源
        camera->eye = glm::vec4(0.7, 0.55, 0.0, 1.0); // 设置相机的位置向量
        camera->at = glm::vec4(0.7, 0.55, -4.0, 1.0); // 设置相机看向的目标向量
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                // 清除颜色缓冲和深度缓冲
    painter->drawMeshes(light, camera);                                // 绘制所有网格
    painter->drawSkybox(skybox, skyboxObject, camera, cubemapTexture); // 绘制天空盒
}

void printHelp()
{
    std::cout << "==========================================================" << std::endl;
    std::cout << "Use mouse to control the light position (continously drag)." << std::endl;
    std::cout << "==========================================================" << std::endl
              << std::endl;

    std::cout << "Keyboard Usage" << std::endl;
    std::cout << "[Window]" << std::endl
              << "ESC:		Exit" << std::endl
              << "h:		Print help message" << std::endl
              << std::endl
              << "[Model]" << std::endl
              << "l/L:		Increase/Decrease exposure" << std::endl
              << "q/Q:      Driving Mode" << std::endl
              << std::endl
              << "[Camera]" << std::endl
              << "v/V:		Increase/Decrease the camera field of view"
              << std::endl
              << "w/W:		Increase/Decrease the camera height" << std::endl
              << "a/A:		Increase/Decrease the camera left/right angle" << std::endl
              << "s/S:		Increase/Decrease the camera forward/backward angle" << std::endl
              << "d/D:		Increase/Decrease the camera up/down angle" << std::endl
              << "space:    Increase the camera height"
              << "ctrl:     Decrease the camera height"
              << std::endl
              << "u/U:		Increase/Decrease the rotate angle" << std::endl
              << "i/I:		Increase/Decrease the up angle" << std::endl
              << "o/O:		Increase/Decrease the camera radius" << std::endl
              << std::endl;
}

void mainWindow_key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) // 键盘回调函数
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    else if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        printHelp();
    }
    else if (key == GLFW_KEY_L && (action == GLFW_PRESS || action == GLFW_REPEAT) && mode == 0x0000)
    {
        painter->exposure += 1;
        std::cout << "exposure: " << painter->exposure << std::endl;
    }
    else if (key == GLFW_KEY_L && (action == GLFW_PRESS || action == GLFW_REPEAT) && mode == GLFW_MOD_SHIFT)
    {
        if (painter->exposure >= 1)
            painter->exposure -= 1;
        std::cout << "exposure: " << painter->exposure << std::endl;
    }
    else if (key == GLFW_KEY_V && (action == GLFW_PRESS || action == GLFW_REPEAT) && mode == 0x0000)
    {
        camera->fov += fovTheta;
        float f = 18.0f / tan(glm::radians(camera->fov) / 2.0f);
        std::cout << "fov: " << f << "mm" << std::endl;
    }
    else if (key == GLFW_KEY_V && (action == GLFW_PRESS || action == GLFW_REPEAT) && mode == GLFW_MOD_SHIFT)
    {
        if (camera->fov >= 1)
        {
            camera->fov -= fovTheta;
            float f = 18.0f / tan(glm::radians(camera->fov) / 2.0f);
            std::cout << "fov: " << f << "mm" << std::endl;
        }
    }
    else if (key == GLFW_KEY_R && (action == GLFW_PRESS || action == GLFW_REPEAT) && mode == 0x0000)
    {
        painter->treeSplit += 50;
    }
    else if (key == GLFW_KEY_R && (action == GLFW_PRESS || action == GLFW_REPEAT) && mode == GLFW_MOD_SHIFT)
    {
        painter->treeSplit -= 50;
    }
    else if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
        if (painter->isDrive) // 如果当前是驾驶模式
        {
            painter->speed = 0.3;                         // 设置速度为0.3，退出驾驶模式后的默认速度
            painter->isDrive = false;                     // 将驾驶模式标志设置为false，表示退出驾驶模式
            initSu7();                                    // 初始化驾驶模式相关的设置或资源
            camera->eye = glm::vec4(0.7, 0.55, 0.0, 1.0); // 设置相机的位置向量
            camera->at = glm::vec4(0.7, 0.55, -4.0, 1.0); // 设置相机看向的目标向量
        }
        else
        {
            painter->isDrive = !painter->isDrive;         // 切换驾驶模式标志，即从非驾驶模式切换到驾驶模式
            camera->eye = glm::vec4(0.7, 0.55, 0.0, 1.0); // 设置相机的位置向量
            camera->at = glm::vec4(0.7, 0.55, -4.0, 1.0); // 设置相机看向的目标向量
            std::cout << "Switched to " << (painter->isDrive ? "Drive" : "Not Drive") << "\n";
        }
    }
    else
    {
        camera->keyboard(key, action, mode); // 调用相机类的键盘回调函数
    }
}

// 重新设置窗口
void reshape(GLsizei w, GLsizei h)
{
    glViewport(0, 0, w, h);
}

void cleanData() // 清理数据
{

    painter->cleanMeshes();

    glDeleteVertexArrays(1, &skyboxObject.vao);
    glDeleteBuffers(1, &skyboxObject.vbo);
    glDeleteProgram(skyboxObject.program);
    glDeleteTextures(1, &cubemapTexture);

    delete skybox;
    skybox = NULL;
}

void setCallbacks(GLFWwindow *window, const int WIDTH, const int HEIGHT) // 设置回调函数
{
    // 设置窗口的帧缓冲大小回调函数，当窗口大小改变时调用
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 设置窗口的键盘按键回调函数，当键盘按键被按下或释放时调用
    glfwSetKeyCallback(window, mainWindow_key_callback);
    // 设置窗口的鼠标按钮回调函数，当鼠标按钮被按下或释放时调用
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    // 设置窗口的鼠标位置回调函数，当鼠标位置改变时调用
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    // 设置相机的纵横比，即窗口的宽高比
    camera->setAspect(static_cast<float>(WIDTH) / static_cast<float>(HEIGHT));
    // 更新相机的投影矩阵，以反映新的纵横比
    camera->updateProjectionMatrix();
    // 将相机的指针设置为窗口的用户指针，以便在回调函数中访问相机对象
    glfwSetWindowUserPointer(window, &camera);
    // 再次设置窗口的帧缓冲大小回调函数，可能是为了确保设置生效或覆盖之前的设置
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

GLFWwindow *initGL(const int WIDTH, const int HEIGHT, const char *title) // 初始化GLFW
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *mainwindow = glfwCreateWindow(WIDTH, HEIGHT, title, NULL, NULL);
    glfwMakeContextCurrent(mainwindow);
    if (mainwindow == NULL)
    {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        // return -1;
    }
    return mainwindow;
}

int main(int argc, char **argv) // 主函数
{
    GLFWwindow *mainwindow = initGL(WIDTH, HEIGHT, "2023193004-RichardLiu-Final");
    setCallbacks(mainwindow, WIDTH, HEIGHT);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initMeshialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST); // 开启深度测试
    init();
    initSky();
    printHelp();
    // bind callbacks
    while (!glfwWindowShouldClose(mainwindow))
    {
        display();
        glfwSwapBuffers(mainwindow);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}