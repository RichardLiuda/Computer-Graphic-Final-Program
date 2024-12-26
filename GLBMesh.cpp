#include "GLBMesh.h"

void GlbMesh::import(std::string name, std::string type)
{                                                                                                                                                      // 利用assimp库导入GLB模型
    filename = "./assets/" + name + "/" + name + "." + type;                                                                                           // 构建文件路径
    scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType); // 读取文件并应用处理选项

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // 检查场景是否加载成功
    {
        std::cerr << "Error: " << importer.GetErrorString() << std::endl; // 输出错误信息
        return;
    }

    directory = filename.substr(0, filename.find_last_of('/')); // 获取文件目录

    std::cout << filename + " loaded successfully!" << std::endl; // 输出成功信息

    meshesNum = scene->mNumMeshes; // 传入模型属性数量
    materialsNum = scene->mNumMaterials;
    texturesNum = scene->mNumTextures;

    spMeshes.resize(meshesNum); // 初始化属性数组
    spMaterials.resize(materialsNum);
    spTextures.resize(texturesNum);
    materialIndexes.resize(meshesNum);
    texturePath.resize(materialsNum);

    for (int i = 0; i < meshesNum; i++)
    { // 提取网格
        spMeshes[i] = new aiMesh;
        *(spMeshes[i]) = *(scene->mMeshes[i]);
        materialIndexes[i] = spMeshes[i]->mMaterialIndex;
    }

    for (int i = 0; i < materialsNum; i++)
    { // 提取材质
        spMaterials[i] = {new aiMaterial};
        *(spMaterials[i].material) = *(scene->mMaterials[i]);
        aiString path;
        path = "";
        spMaterials[i].material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
        texturePath[i] = path.C_Str();
    }

    for (int i = 0; i < texturesNum; i++)
    { // 提取纹理
        spTextures[i] = new aiTexture;
        *(spTextures[i]) = *(scene->mTextures[i]);
    }

    setVertices(); // 设置顶点
    setMaterial(); // 设置材质
}

void GlbMesh::setVertices() // 设置顶点
{
    for (int i = 0; i < meshesNum; i++) // 遍历网格
    {
        aiMesh *mesh = spMeshes[i];
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> colors;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> textures;
        std::vector<vec3i> faces;
        meshes.resize(meshesNum);

        for (int j = 0; j < mesh->mNumVertices; j++) // 遍历顶点
        {
            aiVector3D vertex = mesh->mVertices[j];
            vertices.push_back(glm::vec3(vertex.x, vertex.y, vertex.z));
            allVertices.push_back(glm::vec3(vertex.x, vertex.y, vertex.z));

            if (mesh->mColors[0]) // 提取颜色
            {
                aiColor4D color = mesh->mColors[0][j];
                colors.push_back(glm::vec3(color.r, color.g, color.b));
            }

            aiVector3D normal = mesh->mNormals[j];
            normals.push_back(glm::vec3(normal.x, normal.y, normal.z));

            if (mesh->mTextureCoords[0]) // 提取纹理
            {
                aiVector3D texture = mesh->mTextureCoords[0][j];
                textures.push_back(glm::vec2(texture.x, texture.y));
            }
            else
            {
                textures.push_back(glm::vec2(0.0, 0.0));
            }
        }

        for (int j = 0; j < mesh->mNumFaces; j++) // 提取三角面片坐标
        {
            aiFace face = mesh->mFaces[j];
            faces.push_back(vec3i(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
        }

        if (colors.size() == 0)
        {
            colors.resize(vertices.size());
        }

        TriMesh *triMesh = new TriMesh(vertices, colors, normals, faces, textures); // 创建子网格

        meshes[i] = triMesh;
        meshes[i]->computeTriangleNormals(); // 计算法线
        meshes[i]->storeFacesPoints();       // 存储顶点
    }
}

void GlbMesh::computeBounds() // 计算边界
{
    // 初始化最低值为一个很大的正数，用于后续比较
    lowest = 0x3f3f3f3f;
    // 初始化最高值为一个很小的负数，用于后续比较
    highest = -0x3f3f3f3f;
    // 检查旋转角度的x分量是否大于45度
    if (rotation.x > 45)
    {
        // 遍历所有顶点
        for (auto i : allVertices)
        {
            // 如果当前顶点的z坐标小于最低值，则更新最低值
            if (i.z < lowest)
            {
                lowest = i.z;
            }
            // 如果当前顶点的z坐标大于最高值，则更新最高值
            if (i.z > highest)
            {
                highest = i.z;
            }
        }
        // 将最低值和最高值取反
        lowest *= -1;
        highest *= -1;
        // 交换最低值和最高值
        std::swap(lowest, highest);
    }
    else
    {
        // 遍历所有顶点
        for (auto i : allVertices)
        {
            // 如果当前顶点的y坐标小于最低值，则更新最低值
            if (i.y < lowest)
            {
                lowest = i.y;
            }
            // 如果当前顶点的y坐标大于最高值，则更新最高值
            if (i.y > highest)
            {
                highest = i.y;
            }
        }
    }
    // 计算缩放因子，取x、y、z三个方向的平均值
    GLdouble _scale = (scale.x + scale.y + scale.z) / 3;
    // 保存原始最低值
    clowest = lowest;
    // 将最低值和最高值分别乘以缩放因子
    lowest *= _scale;
    highest *= _scale;
}

void GlbMesh::initMeshes(const std::string &filename, std::string type) // 初始化网格
{
    import(filename, type); // 导入模型
    getTexture();           // 获取纹理
}

// GlbMesh类的成员函数，用于获取和处理材质中的纹理信息
void GlbMesh::getTexture()
{
    // 遍历所有材质
    for (int i = 0; i < materialsNum; i++)
    {
        // 获取当前材质对象
        aiMaterial *material = spMaterials[i].material;

        // 遍历所有贴图类型
        for (aiTextureType type : {aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_NORMALS, aiTextureType_METALNESS, aiTextureType_DIFFUSE_ROUGHNESS, aiTextureType_AMBIENT_OCCLUSION})
        {
            // 获取当前贴图类型的纹理数量
            for (int j = 0; j < material->GetTextureCount(type); ++j)
            {
                aiString texturePath;
                // 获取纹理路径
                if (material->GetTexture(type, j, &texturePath) == AI_SUCCESS)
                {
                    // 检查纹理路径是否为嵌入式纹理（以 '*' 开头）
                    if (texturePath.data[0] == '*')
                    {
                        // 嵌入式纹理（以 '*' 开头）
                        int textureIndex = std::stoi(texturePath.C_Str() + 1);
                        if (textureIndex < scene->mNumTextures)
                        {
                            aiTexture *texture = scene->mTextures[textureIndex];
                            std::string filename = directory + "/embedded_texture_" + std::to_string(textureIndex) + "." + (texture->achFormatHint ? texture->achFormatHint : "png");
                            std::ofstream outFile(filename, std::ios::binary);
                            if (outFile)
                            {
                                outFile.write(reinterpret_cast<char *>(texture->pcData), texture->mWidth);
                                outFile.close();
                            }

                            // 根据贴图类型保存路径
                            if (type == aiTextureType_DIFFUSE)
                            {
                                spMaterials[i].diffuseTexturePath = filename;
                            }
                            else if (type == aiTextureType_SPECULAR)
                            {
                                spMaterials[i].specularTexturePath = filename;
                            }
                            else if (type == aiTextureType_NORMALS)
                            {
                                spMaterials[i].normalTexturePath = filename;
                            }
                            else if (type == aiTextureType_METALNESS)
                            {
                                spMaterials[i].metalnessTexturePath = filename;
                            }
                            else if (type == aiTextureType_DIFFUSE_ROUGHNESS)
                            {
                                spMaterials[i].roughnessTexturePath = filename;
                            }
                            else if (type == aiTextureType_AMBIENT_OCCLUSION)
                            {
                                spMaterials[i].ambientOcclusionTexturePath = filename;
                            }
                        }
                    }
                    else
                    {
                        // 外部纹理路径
                        std::string externalPath = directory + "/" + texturePath.C_Str();

                        // 根据贴图类型保存路径
                        if (type == aiTextureType_DIFFUSE)
                        {
                            spMaterials[i].diffuseTexturePath = externalPath;
                        }
                        else if (type == aiTextureType_SPECULAR)
                        {
                            spMaterials[i].specularTexturePath = externalPath;
                        }
                        else if (type == aiTextureType_NORMALS)
                        {
                            spMaterials[i].normalTexturePath = externalPath;
                        }
                        else if (type == aiTextureType_METALNESS)
                        {
                            spMaterials[i].metalnessTexturePath = externalPath;
                        }
                        else if (type == aiTextureType_DIFFUSE_ROUGHNESS)
                        {
                            spMaterials[i].roughnessTexturePath = externalPath;
                        }
                        else if (type == aiTextureType_AMBIENT_OCCLUSION)
                        {
                            spMaterials[i].ambientOcclusionTexturePath = externalPath;
                        }
                    }
                }
            }
        }
    }
}

void GlbMesh::setMaterial() // 设置材质
{
    for (int i = 0; i < materialsNum; i++)
    {
        aiMaterial *material = spMaterials[i].material;
        // 提取环境光颜色
        aiColor4D ambientColor(0.0f, 0.0f, 0.0f, 0.0f);
        if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor))
        {
            spMaterials[i].ambientColor = glm::vec4(ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a);
        }

        // 提取漫反射颜色
        aiColor4D diffuseColor(0.0f, 0.0f, 0.0f, 0.0f);
        if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor))
        {
            spMaterials[i].diffuseColor = glm::vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
        }

        // 提取镜面反射颜色
        aiColor4D specularColor(0.0f, 0.0f, 0.0f, 0.0f);
        if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor))
        {
            spMaterials[i].specularColor = glm::vec4(specularColor.r, specularColor.g, specularColor.b, specularColor.a);
        }

        // 提取高光系数
        float shininess = 0.0f;
        unsigned int max;
        aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, &max);
        spMaterials[i].shininess = shininess;

        // 提取粗糙度和金属度（如果存在）
        float roughness = 0.0f; // 默认值
        float metalness = 0.0f; // 默认值
        material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
        material->Get(AI_MATKEY_METALLIC_FACTOR, metalness);
        spMaterials[i].roughness = roughness;
        spMaterials[i].metalness = metalness;

        float opacity = 1.0f;
        material->Get(AI_MATKEY_OPACITY, opacity);
        spMaterials[i].opacity = opacity;
    }

    for (int i = 0; i < meshesNum; i++)
    {
        TriMesh *mesh = meshes[i];
        Material material = spMaterials[materialIndexes[i]];

        // 将材质信息传递给 TriMesh
        mesh->setDiffuse(material.diffuseColor);
        mesh->setAmbient(material.ambientColor);
        mesh->setSpecular(material.specularColor);
        mesh->setShininess(material.shininess);
        mesh->setRoughness(material.roughness);
        mesh->setMetalness(material.metalness);
        mesh->setOpacity(material.opacity);
    }
}

void GlbMesh::setTranslation(glm::vec3 translation)
{
    this->translation = translation;
}

void GlbMesh::setRotation(glm::vec3 rotation)
{
    this->rotation = rotation;
}

void GlbMesh::setScale(glm::vec3 scale)
{
    this->scale = scale;
}

void GlbMesh::scaleModel(GLdouble scaleFactor)
{
    scale *= scaleFactor;
    lowest *= scaleFactor;
    highest *= scaleFactor;
}

glm::vec4 GlbMesh::getAmbient() { return ambient; };
glm::vec4 GlbMesh::getDiffuse() { return diffuse; };
glm::vec4 GlbMesh::getSpecular() { return specular; };
float GlbMesh::getShininess() { return shininess; };

void GlbMesh::setAmbient(glm::vec4 _ambient)
{
    for (int i = 0; i < meshesNum; i++)
    {
        meshes[i]->setAmbient(_ambient);
    }
};
void GlbMesh::setDiffuse(glm::vec4 _diffuse)
{
    for (int i = 0; i < meshesNum; i++)
    {
        meshes[i]->setDiffuse(_diffuse);
    }
};
void GlbMesh::setSpecular(glm::vec4 _specular)
{
    for (int i = 0; i < meshesNum; i++)
    {
        meshes[i]->setSpecular(_specular);
    }
};
void GlbMesh::setShininess(float _shininess)
{
    for (int i = 0; i < meshesNum; i++)
    {
        meshes[i]->setShininess(_shininess);
    }
};

void GlbMesh::compute() // 计算相关参数
{
    computeBounds();                    // 计算包围盒
    for (int i = 0; i < meshesNum; i++) // 计算每个网格的参数
    {
        meshes[i]->computeTriangleNormals();
        meshes[i]->storeFacesPoints();
        meshes[i]->setLowest(lowest);
        meshes[i]->setClowest(clowest);
        meshes[i]->setHighest(highest);
    }
}

std::vector<TriMesh *> GlbMesh::getMeshes() { return meshes; }

std::string GlbMesh::getFilename() { return getDirectoryPath(filename); }

std::string GlbMesh::getDirectoryPath(std::string &filename)
{
    size_t pos = filename.find_last_of("/\\"); // 查找最后一个斜杠的位置
    if (pos != std::string::npos)
    {
        return filename.substr(0, pos + 1); // 包括最后的斜杠
    }
    return "./"; // 如果没有斜杠，返回当前目录
}

std::vector<int> GlbMesh::getMaterialIndexes()
{
    return materialIndexes;
}

std::vector<std::string> GlbMesh::getTexturePath()
{
    return texturePath;
}

std::vector<Material> GlbMesh::getMaterials()
{
    return spMaterials;
}

void GlbMesh::addChild(GlbMesh *child)
{
    children.push_back(child);
}

glm::mat4 GlbMesh::getGlbMatrix() // 获取glb的模型矩阵
{
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 trans = translation;
    model = glm::translate(model, translation);
    model = glm::rotate(model, glm::radians(rotation[2]), glm::vec3(0.0, 0.0, 1.0));
    model = glm::rotate(model, glm::radians(rotation[1]), glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, glm::radians(rotation[0]), glm::vec3(1.0, 0.0, 0.0));
    model = glm::scale(model, scale);
    return model;
}

glm::vec3 GlbMesh::getGlbRotation() // 获取glb旋转
{
    return rotation;
}

glm::mat4 GlbMesh::getModelMatrix() // 获取网格矩阵
{
    if (!meshes.empty())
    {
        return meshes[0]->getModelMatrix(); // 返回第一个网格的模型矩阵
    }
    return glm::mat4(1.0f); // 如果没有网格，返回单位矩阵
}

glm::vec3 GlbMesh::getRotation() // 获取网格旋转
{
    if (!meshes.empty())
    {
        return meshes[0]->getRotation(); // 返回第一个网格的模型矩阵
    }
    return glm::vec3(1.0f); // 如果没有网格，返回单位矩阵
}

void addMesh(GlbMesh *glb, TriMesh *mesh, std::string name, std::string vshader, std::string fshader, MeshPainter *painter, int i) // 添加网格
{
    mesh->setIsDisplay(true);
    const std::string diffusePath = glb->getMaterials()[glb->getMaterialIndexes()[i]].diffuseTexturePath;
    const std::string normalPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].normalTexturePath;
    const std::string specularPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].specularTexturePath;
    const std::string metalnessPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].metalnessTexturePath;
    const std::string roughnessPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].roughnessTexturePath;
    const std::string ambientOcclusionPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].ambientOcclusionTexturePath;
    painter->addMesh(mesh, name, diffusePath, normalPath, specularPath, metalnessPath, roughnessPath, ambientOcclusionPath, vshader, fshader);
}

void GlbMesh::updateAndLoad(MeshPainter *painter, glm::mat4 modelMatrix, Camera *camera, Light *light, std::string name, std::string vshader, std::string fshader) // 层级建模
{
    // 应用父节点的变换
    modelMatrix = glm::translate(modelMatrix, translation);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0));
    modelMatrix = glm::scale(modelMatrix, scale);

    // 加载当前节点的所有网格
    for (int i = 0; i < getMeshes().size(); i++)
    {
        TriMesh *mesh = meshes[i];
        mesh->setModelMatrix(modelMatrix);
        addMesh(this, mesh, name, vshader, fshader, painter, i);
    }
    // 更新并渲染子节点
    for (auto child : children)
    {
        std::cout << child->getFilename() << std::endl;
        child->updateAndLoad(painter, modelMatrix, camera, light, name, vshader, fshader);
        std::cout << child->getTranslation().x << " " << child->getTranslation().y << " " << child->getTranslation().z << std::endl;
    }
}