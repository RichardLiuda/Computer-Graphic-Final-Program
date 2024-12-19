#include "GLBMesh.h"
#include "TriMesh.h"

void GlbMesh::importGLB(std::string name) {
	Assimp::Importer importer;
	std::string filename = "./assets/" + name + ".glb";
	const aiScene* _scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
	scene = new aiScene(*_scene);
	//*scene = *_scene;

	if (!_scene) {
		std::cerr << "Error: " << importer.GetErrorString() << std::endl;
		return;
	}

	std::cout << "glb loaded successfully!" << std::endl;

	meshesNum = scene->mNumMeshes;
	spMeshes.resize(meshesNum);
	for (int i = 0; i < meshesNum; i++) {
		//aiMesh* mesh = new aiMesh(*(scene->mMeshes[i]));
		spMeshes[i] = new aiMesh;
		*(spMeshes[i]) = *(scene->mMeshes[i]);
		//std::cout << "Mesh name: " << mesh->mName.C_Str() << std::endl;
		//std::cout << "Mesh " << i << " has " << mesh->mNumVertices << " vertices" << std::endl;
	}

	//for (int j = 0; j < scene->mMeshes[0]->mNumFaces; j++)
	//{
	//	aiFace face = scene->mMeshes[0]->mFaces[j];
	//	std::cout << "Face has " << face.mNumIndices << " indices" << std::endl;
	//	for (int i = 0; i < face.mNumIndices; i++) {
	//		std::cout << "Face index " << i << ": " << face.mIndices[i] << std::endl;
	//	}
	//}

	convertToMesh();
	//return;
}

void GlbMesh::convertToMesh() {

	for (int i = 0; i < meshesNum; i++) {
		aiMesh* mesh = spMeshes[i];
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> colors;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> textures;
		std::vector<vec3i> faces;
		meshes.resize(meshesNum);
		for (int j = 0; j < mesh->mNumVertices; j++) {
			aiVector3D vertex = mesh->mVertices[j];
			vertices.push_back(glm::vec3(vertex.x, vertex.y, vertex.z));
			allVertices.push_back(glm::vec3(vertex.x, vertex.y, vertex.z));
			if (mesh->mColors[0])
			{
				aiColor4D color = mesh->mColors[0][j];
				colors.push_back(glm::vec3(color.r, color.g, color.b));
			}
			aiVector3D normal = mesh->mNormals[j];
			normals.push_back(glm::vec3(normal.x, normal.y, normal.z));
			if (mesh->mTextureCoords[0])
			{
				aiVector3D texture = mesh->mTextureCoords[0][j];
				textures.push_back(glm::vec2(texture.x, texture.y));
			}
		}
		for (int j = 0; j < mesh->mNumFaces; j++)
		{
			aiFace face = mesh->mFaces[j];
			faces.push_back(vec3i(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
		}
		if (colors.size() == 0) {
			colors.resize(vertices.size());
		}
		meshes[i] = new TriMesh(vertices, colors, normals, faces, textures);

		//meshes[i]->setTranslation(glm::vec3(-1.6, 0.0, 0.0));
		//meshes[i]->setRotation(glm::vec3(0.0, 0.0, 0.0));
		//meshes[i]->setScale(glm::vec3(0.5, 0.5, 0.5));

		//meshes[i]->setAmbient(glm::vec4(20.0 / 800, 114.0 / 800, 140.0 / 800, 0.5)); // 环境光
		//meshes[i]->setDiffuse(glm::vec4(0.27568f, 0.41424f, 0.57568f, 0.55f)); // 漫反射
		//meshes[i]->setSpecular(glm::vec4(0.633f, 0.727811f, 0.633f, 0.55f)); // 镜面反射
		//meshes[i]->setShininess(76.0f); //高光系数

		meshes[i]->computeTriangleNormals();
		meshes[i]->storeFacesPoints();

	}

	return;
}

void GlbMesh::computeBounds() {
	lowest = 0x3f3f3f3f;
	highest = -0x3f3f3f3f;
	if (rotation.x > 45) {
		for (auto i : allVertices) {
			if (i.z < lowest) {
				lowest = i.z;
			}
			if (i.z > highest) {
				highest = i.z;
			}
		}
		lowest *= -1;
		highest *= -1;
		std::swap(lowest, highest);
	}
	else
	{
		for (auto i : allVertices) {
			if (i.y < lowest) {
				lowest = i.y;
			}
			if (i.y > highest) {
				highest = i.y;
			}
		}
	}
	GLdouble _scale = (scale.x + scale.y + scale.z) / 3;
	clowest = lowest;
	lowest *= _scale;
	highest *= _scale;
}

void GlbMesh::initMeshes(const std::string& filename) {
	importGLB(filename);
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

void GlbMesh::setAmbient(glm::vec4 _ambient) {
	for (int i = 0; i < meshesNum; i++)
	{
		meshes[i]->setAmbient(_ambient);
	}
};
void GlbMesh::setDiffuse(glm::vec4 _diffuse) {
	for (int i = 0; i < meshesNum; i++)
	{
		meshes[i]->setDiffuse(_diffuse);
	}
};
void GlbMesh::setSpecular(glm::vec4 _specular) {
	for (int i = 0; i < meshesNum; i++)
	{
		meshes[i]->setSpecular(_specular);
	}
};
void GlbMesh::setShininess(float _shininess) {
	for (int i = 0; i < meshesNum; i++)
	{
		meshes[i]->setShininess(_shininess);
	}
};

void GlbMesh::compute() {
	computeBounds();
	for (int i = 0; i < meshesNum; i++)
	{
		meshes[i]->computeTriangleNormals();
		meshes[i]->storeFacesPoints();
		meshes[i]->setLowest(lowest);
		meshes[i]->setClowest(clowest);
		meshes[i]->setHighest(highest);
	}
}