#include "BasicMesh.h"




bool BasicMesh::loadMesh(const std::string& fileName) {

	//clear();

	bool ret{ false };
	Assimp::Importer importer;

	const aiScene* scene{ importer.ReadFile(fileName, ASSIMP_LOAD_FLAGS) };

	if (scene) {
		ret = initFromScene(scene, fileName);
	}
	else {
		
		MessageBox(0, L"Error parsing  Failed.", 0, 0);
	}

	return ret;
}

bool BasicMesh::initFromScene(const aiScene* scene, const std::string& fileName) {


	meshes.resize(scene->mNumMeshes);
	textures.resize(scene->mNumMaterials);

	unsigned int numVertices{ 0 };
	unsigned int numIndices{ 0 };

	countVerticesAndIndices(scene, numVertices, numIndices);

	initAllMeshs(scene);

	if (!initMaterials(scene, fileName)) {
		return false;
	}

	//populateBuffers();

	return //checkError function;
		true;


}

void BasicMesh::countVerticesAndIndices(const aiScene* scene, unsigned int& numVertices, unsigned int& numIndices) {
	

	for (unsigned int i{0}; i < meshes.size(); ++i) {
		meshes[i].materialIndex = scene->mMeshes[i]->mMaterialIndex;
		meshes[i].numIndices = scene->mMeshes[i]->mNumFaces * 3;
		meshes[i].baseVertex = numVertices;
		meshes[i].baseIndex = numIndices;

		numVertices += scene->mMeshes[i]->mNumVertices;
		numIndices += meshes[i].numIndices;
	}
}

void BasicMesh::initAllMeshs(const aiScene* scene) {

	for (unsigned int i{ 0 }; i < meshes.size(); ++i) {
		const aiMesh* paiMesh = scene->mMeshes[i];
		initSingleMesh(paiMesh);
	}

}


void BasicMesh::initSingleMesh(const aiMesh* paiMesh) {

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	Vertex v;

	// Populate the vertex attribute vectors
	for (unsigned int i{ 0 }; i < paiMesh->mNumVertices; i++) {
		
		const aiVector3D& pPos{ paiMesh->mVertices[i] };
		v.Position = DirectX::XMFLOAT3(pPos.x, pPos.y, pPos.z);


		const aiVector3D& pNormal{ paiMesh->mNormals[i] };
		v.Normal = DirectX::XMFLOAT3(pNormal.x, pNormal.y, pNormal.z);
		
		const aiVector3D& pTexCoord{ paiMesh->HasTextureCoords(0) ? paiMesh->mTextureCoords[0][i] : Zero3D };
		v.TexCoords = DirectX::XMFLOAT2(pTexCoord.x, pTexCoord.y);

		vertices.push_back(v);
	}

	// Populate the index buffer
	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		indices.push_back(Face.mIndices[0]);
		indices.push_back(Face.mIndices[1]);
		indices.push_back(Face.mIndices[2]);
	}
}


bool BasicMesh::initMaterials(const aiScene* scene, const std::string& fileName) {

	std::string::size_type slashIndex = fileName.find_last_of("/");
	std::string DIR;

	if (slashIndex == std::string::npos) {
		DIR = ".";
	}
	else if (slashIndex == 0) {
		DIR = "/";
	} else {
		DIR = fileName.substr(0, slashIndex);
	}

	bool Ret = true;

	//change the code from this to imgui
	//printf("Num materials: %d\n", pScene->mNumMaterials);
	

	// Initialize the materials
	for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
		const aiMaterial* pMaterial{ scene->mMaterials[i] };

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string p{ path.data };

				if(p.substr(0, 2) == ".\\") {
					p = p.substr(2, p.size() - 2);
				}

				//std::string fullPath{ dir + "/" + p };

				textures[i] = new Texture();
			}
		}

	}

	return Ret;

}