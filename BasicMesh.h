#pragma once

#include <string>
#include <vector>

#include <DirectXMath.h>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Texture.h"
#include "Material.h"
#include "helper.h"

class BasicMesh {
public:
	
	
	BasicMesh() = default;
	~BasicMesh() = default;

	bool loadMesh(const std::string& fileName);

	std::vector<unsigned int> indices;

private:

	struct Vertex {
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 TexCoords;
		DirectX::XMFLOAT3 Normal;
	};

	struct BasicMeshEntry {

		BasicMeshEntry() = default;

		unsigned int numIndices{ 0 };
		unsigned int baseVertex{ 0 };
		unsigned int baseIndex{ 0 };
		unsigned int materialIndex{ 0 };
	};


	bool initFromScene(const aiScene* scene, const std::string& fileName);
	void countVerticesAndIndices(const aiScene* scene, unsigned int& numVertices, unsigned int& numIndices);
	void initAllMeshs(const aiScene* scene);
	void initSingleMesh(const aiMesh* paiMesh);
	bool initMaterials(const aiScene* scene, const std::string& fileName);

	std::vector<BasicMeshEntry> meshes;
	std::vector<Texture*> textures;
	std::vector<Vertex> vertices;
	std::vector<Material> materials;

};