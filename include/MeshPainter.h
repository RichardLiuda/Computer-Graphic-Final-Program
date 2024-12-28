#ifndef _MESH_PAINTER_H_
#define _MESH_PAINTER_H_

#include "GLBMesh.h"
#include "Angel.h"
#include "Camera.h"
#include <vector>

class MeshPainter
{

public:
    MeshPainter();
    ~MeshPainter();

    std::vector<std::string> getMeshNames();

    std::vector<TriMesh *> getMeshes();
    std::vector<openGLObject> getOpenGLObj();

    // 读取纹理文件
    void load_texture_STBImage(const std::string &file_name, GLuint &texture);

    // 读取立方体纹理文件
    GLuint loadCubemap(const std::vector<std::string> &faces);

    // 传递光线材质数据的
    // void bindLightAndMaterial( int mesh_id, int light_id, Camera* camera );
    void bindLightAndMaterial(TriMesh *mesh, openGLObject &object, Light *light, Camera *camera);

    void bindObjectAndData(TriMesh *mesh, openGLObject &object, const std::string &texture_image_diffuse, const std::string &texture_image_normal, const std::string &texture_image_specular, const std::string &texture_image_metalness, const std::string &texture_image_roughness, const std::string &texture_image_occlusion, const std::string &vshader, const std::string &fshader);

    void bindSkyboxObjectAndData(TriMesh *mesh, openGLObject &object, const std::string &vshader, const std::string &fshader);

    // 添加物体
    void addMesh(TriMesh *mesh, const std::string &name, const std::string &texture_image_diffuse, const std::string &texture_image_normal, const std::string &texture_image_specular, const std::string &texture_image_metalness, const std::string &texture_image_roughness, const std::string &texture_image_ambientOcclusion, const std::string &vshader, const std::string &fshader);

    // 绘制物体
    void drawMesh(TriMesh *mesh, openGLObject &object, Light *light, Camera *camera);
    void drawMesh_background(TriMesh *mesh, openGLObject &object, Light *light, Camera *camera);
    void drawSkybox(TriMesh *mesh, openGLObject &object, Camera *camera, GLuint cubemapTexture);
    // 绘制多个物体
    void drawMeshes(Light *light, Camera *camera);

    // 清空数据
    void cleanMeshes();

    float exposure = 2.0f;
    bool isDrive = false;
    float speed = 2.0f;
    int treeSplit = 5000;

private:
    std::vector<std::string> mesh_names;
    std::vector<TriMesh *> meshes;
    std::vector<openGLObject> opengl_objects;
};

#endif