#include "GLBMesh.h"

void GlbMesh::import(std::string name, std::string type) {
	filename = "./assets/" + name + "/" + name + "." + type;
	scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "Error: " << importer.GetErrorString() << std::endl;
		return;
	}

	directory = filename.substr(0, filename.find_last_of('/'));

	std::cout << filename + " loaded successfully!" << std::endl;

	meshesNum = scene->mNumMeshes;
	materialsNum = scene->mNumMaterials;
	texturesNum = scene->mNumTextures;
	spMeshes.resize(meshesNum);
	spMaterials.resize(materialsNum);
	spTextures.resize(texturesNum);
	materialIndexes.resize(meshesNum);
	texturePath.resize(materialsNum);
	for (int i = 0; i < meshesNum; i++) {
		//aiMesh* mesh = new aiMesh(*(scene->mMeshes[i]));
		spMeshes[i] = new aiMesh;
		*(spMeshes[i]) = *(scene->mMeshes[i]);

		materialIndexes[i] = spMeshes[i]->mMaterialIndex;
	}

	for (int i = 0; i < materialsNum; i++) {
		spMaterials[i] = { new aiMaterial };
		*(spMaterials[i].material) = *(scene->mMaterials[i]);
		aiString path;
		path = "";
		spMaterials[i].material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		texturePath[i] = path.C_Str();


	}

	for (int i = 0; i < texturesNum; i++) {
		spTextures[i] = new aiTexture;
		*(spTextures[i]) = *(scene->mTextures[i]);
	}

	setVertices();
	setMaterial();
}

void GlbMesh::setVertices() {
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

			if (mesh->mColors[0]) {
				aiColor4D color = mesh->mColors[0][j];
				colors.push_back(glm::vec3(color.r, color.g, color.b));
			}

			aiVector3D normal = mesh->mNormals[j];
			normals.push_back(glm::vec3(normal.x, normal.y, normal.z));

			if (mesh->mTextureCoords[0]) {
				aiVector3D texture = mesh->mTextureCoords[0][j];
				textures.push_back(glm::vec2(texture.x, texture.y));
			}
			else {
				textures.push_back(glm::vec2(0.0, 0.0));
			}
		}

		for (int j = 0; j < mesh->mNumFaces; j++) {
			aiFace face = mesh->mFaces[j];
			faces.push_back(vec3i(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
		}

		if (colors.size() == 0) {
			colors.resize(vertices.size());
		}

		TriMesh* triMesh = new TriMesh(vertices, colors, normals, faces, textures);

		meshes[i] = triMesh;
		meshes[i]->computeTriangleNormals();
		meshes[i]->storeFacesPoints();
	}
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

void GlbMesh::initMeshes(const std::string& filename, std::string type) {
import(filename, type);
	getTexture();
}

void GlbMesh::getTexture() {
	for (int i = 0; i < materialsNum; i++) {
		aiMaterial* material = spMaterials[i].material;

		// ����������ͼ����
		for (aiTextureType type : {aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_NORMALS, aiTextureType_METALNESS, aiTextureType_DIFFUSE_ROUGHNESS, aiTextureType_AMBIENT_OCCLUSION}) {
			for (int j = 0; j < material->GetTextureCount(type); ++j) {
				aiString texturePath;
				if (material->GetTexture(type, j, &texturePath) == AI_SUCCESS) {
					if (texturePath.data[0] == '*') {
						// Ƕ��ʽ������ '*' ��ͷ��
						int textureIndex = std::stoi(texturePath.C_Str() + 1);
						if (textureIndex < scene->mNumTextures) {
							aiTexture* texture = scene->mTextures[textureIndex];
							std::string filename = directory + "/embedded_texture_" + std::to_string(textureIndex) + "." + (texture->achFormatHint ? texture->achFormatHint : "png");
							std::ofstream outFile(filename, std::ios::binary);
							if (outFile) {
								outFile.write(reinterpret_cast<char*>(texture->pcData), texture->mWidth);
								outFile.close();
							}

							// ������ͼ���ͱ���·��
							if (type == aiTextureType_DIFFUSE) {
								spMaterials[i].diffuseTexturePath = filename;
							}
							else if (type == aiTextureType_SPECULAR) {
								spMaterials[i].specularTexturePath = filename;
							}
							else if (type == aiTextureType_NORMALS) {
								spMaterials[i].normalTexturePath = filename;
							}
							else if (type == aiTextureType_METALNESS) {
								spMaterials[i].metalnessTexturePath = filename;
							}
							else if (type == aiTextureType_DIFFUSE_ROUGHNESS) {
								spMaterials[i].roughnessTexturePath = filename;
							}
							else if (type == aiTextureType_AMBIENT_OCCLUSION) {
								spMaterials[i].ambientOcclusionTexturePath = filename;
							}
						}
					}
					else {
						// �ⲿ����·��
						std::string externalPath = directory + "/" + texturePath.C_Str();

						// ������ͼ���ͱ���·��
						if (type == aiTextureType_DIFFUSE) {
							spMaterials[i].diffuseTexturePath = externalPath;
						}
						else if (type == aiTextureType_SPECULAR) {
							spMaterials[i].specularTexturePath = externalPath;
						}
						else if (type == aiTextureType_NORMALS) {
							spMaterials[i].normalTexturePath = externalPath;
						}
						else if (type == aiTextureType_METALNESS) {
							spMaterials[i].metalnessTexturePath = externalPath;
						}
						else if (type == aiTextureType_DIFFUSE_ROUGHNESS) {
							spMaterials[i].roughnessTexturePath = externalPath;
						}
						else if (type == aiTextureType_AMBIENT_OCCLUSION) {
							spMaterials[i].ambientOcclusionTexturePath = externalPath;
						}
					}
				}
			}
		}
	}
}

void GlbMesh::setMaterial() {
	for (int i = 0; i < materialsNum; i++) {
		aiMaterial* material = spMaterials[i].material;
		// ��ȡ��������ɫ
		aiColor4D ambientColor(0.0f, 0.0f, 0.0f, 0.0f);
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor)) {
			spMaterials[i].ambientColor = glm::vec4(ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a);
		}

		// ��ȡ��������ɫ
		aiColor4D diffuseColor(0.0f, 0.0f, 0.0f, 0.0f);
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor)) {
			spMaterials[i].diffuseColor = glm::vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
		}

		// ��ȡ���淴����ɫ
		aiColor4D specularColor(0.0f, 0.0f, 0.0f, 0.0f);
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor)) {
			spMaterials[i].specularColor = glm::vec4(specularColor.r, specularColor.g, specularColor.b, specularColor.a);
		}

		// ��ȡ�߹�ϵ��
		float shininess = 0.0f;
		unsigned int max;
		aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, &max);
		spMaterials[i].shininess = shininess;

		// ��ȡ�ֲڶȺͽ����ȣ�������ڣ�
		float roughness = 0.0f;  // Ĭ��ֵ
		float metalness = 0.0f;  // Ĭ��ֵ
		material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
		material->Get(AI_MATKEY_METALLIC_FACTOR, metalness);
		spMaterials[i].roughness = roughness;
		spMaterials[i].metalness = metalness;

		float opacity = 1.0f;
		material->Get(AI_MATKEY_OPACITY, opacity);
		spMaterials[i].opacity = opacity;
	}

	for (int i = 0; i < meshesNum; i++) {
		TriMesh* mesh = meshes[i];
		Material material = spMaterials[materialIndexes[i]];

		// ��������Ϣ���ݸ� TriMesh
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
		//meshes[i]->setTranslation(translation);
		//meshes[i]->setRotation(rotation);
		//meshes[i]->setScale(scale);
	}
}

std::vector<TriMesh*> GlbMesh::getMeshes() { return meshes; }

std::string GlbMesh::getFilename() { return getDirectoryPath(filename); }

std::string GlbMesh::getDirectoryPath(std::string& filename) {
	size_t pos = filename.find_last_of("/\\"); // �������һ��б�ܵ�λ��
	if (pos != std::string::npos) {
		return filename.substr(0, pos + 1); // ��������б��
	}
	return "./"; // ���û��б�ܣ����ص�ǰĿ¼
}

std::vector<int> GlbMesh::getMaterialIndexes() {
	return materialIndexes;
}

std::vector<std::string> GlbMesh::getTexturePath() {
	return texturePath;
}

std::vector<Material> GlbMesh::getMaterials() {
	return spMaterials;
}

void GlbMesh::addChild(GlbMesh* child) {
	children.push_back(child);
}

glm::mat4 GlbMesh::getGlbMatrix()
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

glm::mat4 GlbMesh::getModelMatrix() {
	if (!meshes.empty()) {
		return meshes[0]->getModelMatrix();  // ���ص�һ�������ģ�;���
	}
	return glm::mat4(1.0f);  // ���û�����񣬷��ص�λ����
}

glm::vec3 GlbMesh::getRotation() {
	if (!meshes.empty()) {
		return meshes[0]->getRotation();  // ���ص�һ�������ģ�;���
	}
	return glm::vec3(1.0f);  // ���û�����񣬷��ص�λ����
}

glm::vec3 GlbMesh::getGlbRotation() {
	return rotation;
}


void addMesh(GlbMesh* glb, TriMesh* mesh, std::string name, std::string vshader, std::string fshader, MeshPainter* painter, int i) {
	mesh->setIsDisplay(true);
	const std::string diffusePath = glb->getMaterials()[glb->getMaterialIndexes()[i]].diffuseTexturePath;
	const std::string normalPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].normalTexturePath;
	const std::string specularPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].specularTexturePath;
	const std::string metalnessPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].metalnessTexturePath;
	const std::string roughnessPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].roughnessTexturePath;
	const std::string ambientOcclusionPath = glb->getMaterials()[glb->getMaterialIndexes()[i]].ambientOcclusionTexturePath;
	painter->addMesh(mesh, name, diffusePath, normalPath, specularPath, metalnessPath, roughnessPath, ambientOcclusionPath, vshader, fshader);
}

void GlbMesh::updateAndLoad(MeshPainter* painter, glm::mat4 modelMatrix, Camera* camera, Light* light, std::string name, std::string vshader, std::string fshader) {
	// Ӧ�ø��ڵ�ı任
	modelMatrix = glm::translate(modelMatrix, translation);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0));
	modelMatrix = glm::scale(modelMatrix, scale);

	// ���ص�ǰ�ڵ����������
	for (int i = 0; i < getMeshes().size(); i++) {
		TriMesh* mesh = meshes[i];
		mesh->setModelMatrix(modelMatrix);
		addMesh(this, mesh, name, vshader, fshader, painter, i);
	}
	// ���²���Ⱦ�ӽڵ�
	for (auto child : children) {
		std::cout << child->getFilename() << std::endl;
		child->updateAndLoad(painter, modelMatrix, camera, light, name, vshader, fshader);
		std::cout << child->getTranslation().x << " " << child->getTranslation().y << " " << child->getTranslation().z << std::endl;
	}
}