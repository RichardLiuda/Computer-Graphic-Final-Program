#include "TriMesh.h"

TriMesh::TriMesh(std::vector<glm::vec3> vertices, std::vector<glm::vec3> colors, std::vector<glm::vec3> normals, std::vector<vec3i> faces, std::vector<glm::vec2> textures) : vertex_positions(vertices), vertex_colors(colors), vertex_normals(normals), faces(faces), vertex_textures(textures) {}

// 一些基础颜色
const glm::vec3 basic_colors[8] = {
    glm::vec3(1.0, 1.0, 1.0), // White
    glm::vec3(1.0, 1.0, 0.0), // Yellow
    glm::vec3(0.0, 1.0, 0.0), // Green
    glm::vec3(0.0, 1.0, 1.0), // Cyan
    glm::vec3(1.0, 0.0, 1.0), // Magenta
    glm::vec3(1.0, 0.0, 0.0), // Red
    glm::vec3(0.0, 0.0, 0.0), // Black
    glm::vec3(0.0, 0.0, 1.0)  // Blue
};

// 立方体的各个点
const glm::vec3 cube_vertices[8] = {
    glm::vec3(-0.5, -0.5, -0.5),
    glm::vec3(0.5, -0.5, -0.5),
    glm::vec3(-0.5, 0.5, -0.5),
    glm::vec3(0.5, 0.5, -0.5),
    glm::vec3(-0.5, -0.5, 0.5),
    glm::vec3(0.5, -0.5, 0.5),
    glm::vec3(-0.5, 0.5, 0.5),
    glm::vec3(0.5, 0.5, 0.5)};

// 三角形的点
const glm::vec3 triangle_vertices[3] = {
    glm::vec3(-0.5, -0.5, 0.0),
    glm::vec3(0.5, -0.5, 0.0),
    glm::vec3(0.0, 0.5, 0.0)};

// 正方形平面
const glm::vec3 square_vertices[4] = {
    glm::vec3(-5, 0, -10),
    glm::vec3(5, 0, -10),
    glm::vec3(5, 0, 1),
    glm::vec3(-5, 0, 1),
};

TriMesh::TriMesh()
{
    scale = glm::vec3(1.0);
    rotation = glm::vec3(0.0);
    translation = glm::vec3(0.0);
    isDisplay = false;
}

TriMesh::~TriMesh()
{
}

std::vector<glm::vec3> TriMesh::getVertexPositions()
{
    return vertex_positions;
}

std::vector<glm::vec3> TriMesh::getVertexColors()
{
    return vertex_colors;
}

std::vector<glm::vec3> TriMesh::getVertexNormals()
{
    return vertex_normals;
}

std::vector<glm::vec2> TriMesh::getVertexTextures()
{
    return vertex_textures;
}

std::vector<vec3i> TriMesh::getFaces()
{
    return faces;
}

std::vector<glm::vec3> TriMesh::getPoints()
{
    return points;
}

std::vector<glm::vec3> TriMesh::getColors()
{
    return colors;
}

std::vector<glm::vec3> TriMesh::getNormals()
{
    return normals;
}

std::vector<glm::vec2> TriMesh::getTextures()
{
    return textures;
}

void TriMesh::computeTriangleNormals() // 计算每个面的法向量
{
    face_normals.resize(faces.size());
    for (size_t i = 0; i < faces.size(); i++)
    {
        auto &face = faces[i];
        // 计算每个面片的法向量并归一化
        glm::vec3 norm;
        glm::vec3 v01 = vertex_positions[face.y] - vertex_positions[face.x];
        glm::vec3 v02 = vertex_positions[face.z] - vertex_positions[face.x];
        face_normals[i] = normalize(cross(v01, v02));
    }
}

void TriMesh::computeVertexNormals() // 计算每个顶点的法向量
{
    // 计算面片的法向量
    if (face_normals.size() == 0 && faces.size() > 0)
    {
        computeTriangleNormals();
    }

    // 初始化法向量为0
    vertex_normals.resize(vertex_positions.size(), glm::vec3(0, 0, 0));
    // Task1 求法向量均值
    for (size_t i = 0; i < faces.size(); i++)
    {
        auto &face = faces[i];
        // 先累加面的法向量
        vertex_normals[face.x] += face_normals[i];
        vertex_normals[face.y] += face_normals[i];
        vertex_normals[face.z] += face_normals[i];
    }
    // 对累加的法向量并归一化
    for (size_t i = 0; i < vertex_normals.size(); i++)
    {
        vertex_normals[i] = normalize(vertex_normals[i]);
    }
}

glm::vec3 TriMesh::getTranslation()
{
    return translation;
}

glm::vec3 TriMesh::getRotation()
{
    return rotation;
}

glm::vec3 TriMesh::getScale()
{
    return scale;
}

GLdouble TriMesh::getLowest()
{
    return lowest;
}

GLdouble TriMesh::getClowest()
{
    return clowest;
}

GLdouble TriMesh::getHighest()
{
    return highest;
}

float TriMesh::getOpacity()
{
    return opacity;
}

void TriMesh::setOpacity(float opacity)
{
    this->opacity = opacity;
}

void TriMesh::setLowest(GLdouble lowest)
{
    this->lowest = lowest;
}

void TriMesh::setClowest(GLdouble clowest)
{
    this->clowest = clowest;
}

void TriMesh::setHighest(GLdouble highest)
{
    this->highest = highest;
}

bool TriMesh::getIsDisplay()
{
    return isDisplay;
}

void TriMesh::updateMatrix() // 更新模型矩阵
{
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 trans = getTranslation();
    model = glm::translate(model, getTranslation());
    model = glm::rotate(model, glm::radians(getRotation().z), glm::vec3(0.0, 0.0, 1.0));
    model = glm::rotate(model, glm::radians(getRotation().y), glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, glm::radians(getRotation().x), glm::vec3(1.0, 0.0, 0.0));
    model = glm::scale(model, getScale());
    modelMatrix = model;
}

glm::mat4 TriMesh::getModelMatrix()
{
    return modelMatrix;
}

void TriMesh::setTranslation(glm::vec3 translation)
{
    this->translation = translation;
    updateMatrix();
}

void TriMesh::setRotation(glm::vec3 rotation)
{
    this->rotation = rotation;
    updateMatrix();
}

void TriMesh::setScale(glm::vec3 scale)
{
    this->scale = scale;
    updateMatrix();
}

void TriMesh::scaleModel(GLdouble scaleFactor) // 缩放模型
{
    scale *= scaleFactor;
    lowest *= scaleFactor;
    highest *= scaleFactor;
}

void TriMesh::setIsDisplay(bool isDisplay)
{
    this->isDisplay = isDisplay;
}

// 获取材质属性
glm::vec4 TriMesh::getAmbient() { return ambient; };
glm::vec4 TriMesh::getDiffuse() { return diffuse; };
glm::vec4 TriMesh::getSpecular() { return specular; };
float TriMesh::getShininess() { return shininess; };

// 设置材质属性
void TriMesh::setAmbient(glm::vec4 _ambient) { ambient = _ambient; };
void TriMesh::setDiffuse(glm::vec4 _diffuse) { diffuse = _diffuse; };
void TriMesh::setSpecular(glm::vec4 _specular) { specular = _specular; };
void TriMesh::setShininess(float _shininess) { shininess = _shininess; };

void TriMesh::cleanData() // 清空数据
{
    vertex_positions.clear();
    vertex_colors.clear();
    vertex_normals.clear();
    vertex_textures.clear();

    faces.clear();
    face_normals.clear();

    points.clear();
    colors.clear();
    normals.clear();
    textures.clear();
}

void TriMesh::storeFacesPoints() // 存储要传入GPU的数据
{
    // 计算法向量
    if (vertex_normals.size() == 0)
        computeVertexNormals();
    // 根据每个三角面片的顶点下标存储要传入GPU的数据
    for (int i = 0; i < faces.size(); i++)
    {
        // 坐标
        points.push_back(vertex_positions[faces[i].x]);
        points.push_back(vertex_positions[faces[i].y]);
        points.push_back(vertex_positions[faces[i].z]);
        // 颜色
        colors.push_back(vertex_colors[faces[i].x]);
        colors.push_back(vertex_colors[faces[i].y]);
        colors.push_back(vertex_colors[faces[i].z]);
        // 法向量
        if (vertex_normals.size() != 0)
        {
            normals.push_back(vertex_normals[faces[i].x]);
            normals.push_back(vertex_normals[faces[i].y]);
            normals.push_back(vertex_normals[faces[i].z]);
        }
        // 纹理
        if (vertex_textures.size() != 0)
        {
            textures.push_back(vertex_textures[faces[i].x]);
            textures.push_back(vertex_textures[faces[i].y]);
            textures.push_back(vertex_textures[faces[i].z]);
        }
    }
}

// 立方体生成12个三角形的顶点索引
void TriMesh::generateCube()
{
    // 创建顶点前要先把那些vector清空
    cleanData();

    // 存储立方体的各个面信息
    for (int i = 0; i < 8; i++)
    {
        vertex_positions.push_back(cube_vertices[i]);
        vertex_colors.push_back(basic_colors[i]);
    }

    // 每个三角面片的顶点下标
    faces.push_back(vec3i(0, 3, 1));
    faces.push_back(vec3i(0, 2, 3));
    faces.push_back(vec3i(1, 5, 4));
    faces.push_back(vec3i(1, 4, 0));
    faces.push_back(vec3i(4, 2, 0));
    faces.push_back(vec3i(4, 6, 2));
    faces.push_back(vec3i(5, 6, 4));
    faces.push_back(vec3i(5, 7, 6));
    faces.push_back(vec3i(2, 6, 7));
    faces.push_back(vec3i(2, 7, 3));
    faces.push_back(vec3i(1, 7, 5));
    faces.push_back(vec3i(1, 3, 7));

    storeFacesPoints();
}

void TriMesh::generateTriangle(glm::vec3 color)
{
    // 创建顶点前要先把那些vector清空
    cleanData();

    for (int i = 0; i < 3; i++)
    {
        vertex_positions.push_back(triangle_vertices[i]);
        vertex_colors.push_back(color);
    }

    // 每个三角面片的顶点下标
    faces.push_back(vec3i(0, 1, 2));

    storeFacesPoints();
}

void TriMesh::generateSquare(glm::vec3 color)
{
    // 创建顶点前要先把那些vector清空
    cleanData();

    for (int i = 0; i < 4; i++)
    {
        vertex_positions.push_back(square_vertices[i]);
        vertex_colors.push_back(glm::vec3(20 / 255, 114 / 255, 140 / 255));
    }

    vertex_textures.push_back(glm::vec2(0.0, 0.0));
    vertex_textures.push_back(glm::vec2(1.0, 0.0));
    vertex_textures.push_back(glm::vec2(1.0, 1.0));
    vertex_textures.push_back(glm::vec2(0.0, 1.0));

    // 每个三角面片的顶点下标
    faces.push_back(vec3i(0, 1, 2));
    faces.push_back(vec3i(0, 2, 3));
    storeFacesPoints();
    normalReverse();
}

void TriMesh::normalReverse()
{
    for (auto &i : normals)
    {
        i *= -1;
    }
}

void TriMesh::computeBounds()
{
    lowest = 0x3f3f3f3f;
    highest = -0x3f3f3f3f;
    if (getRotation().x > 45)
    {
        for (auto i : points)
        {
            if (i.z < lowest)
            {
                lowest = i.z;
            }
            if (i.z > highest)
            {
                highest = i.z;
            }
        }
        lowest *= -1;
        highest *= -1;
        std::swap(lowest, highest);
    }
    else
    {
        for (auto i : points)
        {
            if (i.y < lowest)
            {
                lowest = i.y;
            }
            if (i.y > highest)
            {
                highest = i.y;
            }
        }
    }
    GLdouble _scale = (scale.x + scale.y + scale.z) / 3;
    clowest = lowest;
    lowest *= _scale;
    highest *= _scale;
}

void TriMesh::readOff(const std::string &filename)
{
    // fin打开文件读取文件信息
    if (filename.empty())
    {
        return;
    }
    std::ifstream fin;
    fin.open(filename);
    if (!fin)
    {
        printf("File on error\n");
        return;
    }
    else
    {
        printf("File open success\n");

        cleanData();

        int nVertices, nFaces, nEdges;

        // 读取OFF字符串
        std::string str;
        fin >> str;
        // 读取文件中顶点数、面片数、边数
        fin >> nVertices >> nFaces >> nEdges;
        // 根据顶点数，循环读取每个顶点坐标
        for (int i = 0; i < nVertices; i++)
        {
            glm::vec3 tmp_node;
            fin >> tmp_node.x >> tmp_node.y >> tmp_node.z;
            vertex_positions.push_back(tmp_node);
            vertex_colors.push_back(tmp_node);
        }
        // 根据面片数，循环读取每个面片信息，并用构建的vec3i结构体保存
        for (int i = 0; i < nFaces; i++)
        {
            int num, a, b, c;
            // num记录此面片由几个顶点构成，a、b、c为构成该面片顶点序号
            fin >> num >> a >> b >> c;
            faces.push_back(vec3i(a, b, c));
        }
    }
    fin.close();

    storeFacesPoints();

    computeBounds();
};

// Light
glm::mat4 Light::getShadowProjectionMatrix()
{
    float lx, ly, lz;

    glm::mat4 modelMatrix = this->getModelMatrix();
    glm::vec4 light_position = modelMatrix * glm::vec4(this->translation, 1.0);

    lx = light_position[0];
    ly = light_position[1];
    lz = light_position[2];
    // std::cout << "light_position: " << lx << " " << ly << " " << lz << std::endl;

    return glm::mat4(
        -ly, 0.0, 0.0, 0.0,
        lx, 0.0, lz, 1.0,
        0.0, 0.0, -ly, 0.0,
        0.0, 0.0, 0.0, -ly);
}

float TriMesh::getRoughness() { return roughness; }
void TriMesh::setRoughness(float _roughness) { roughness = _roughness; }

float TriMesh::getMetalness() { return metalness; }
void TriMesh::setMetalness(float _metalness) { metalness = _metalness; }

std::string TriMesh::getNormalTexturePath() { return normalTexturePath; }
void TriMesh::setNormalTexturePath(const std::string &path) { normalTexturePath = path; }

float Light::getConstant() { return constant; }
float Light::getLinear() { return linear; }
float Light::getQuadratic() { return quadratic; }

void TriMesh::generateSkybox() // 天空盒
{
    // 创建顶点前要先把那些vector清空
    cleanData();

    // 天空盒的顶点数据
    float skyboxVertices[] = {
        // positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f};

    // 将顶点数据存储到 vertex_positions 中
    for (int i = 0; i < 36; i++)
    {
        vertex_positions.push_back(glm::vec3(skyboxVertices[i * 3], skyboxVertices[i * 3 + 1], skyboxVertices[i * 3 + 2]));
        vertex_colors.push_back(glm::vec3(1.0f)); // 默认颜色为白色
    }

    // 将顶点数据存储到 points 中
    points = vertex_positions;
    colors = vertex_colors;
}

void TriMesh::setModelMatrix(const glm::mat4 &matrix)
{
    modelMatrix = matrix;
}