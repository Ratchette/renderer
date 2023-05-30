#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>

#include "shader.hpp"
#include "mesh.hpp"

using namespace std;

class Model {
public:
	Model(const char* path);
	void Draw(Shader& shader);

private:
	vector<Mesh> meshes;
	vector<Texture> texturesLoaded;
	string directory;

	void LoadModel(string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type,
		string typeName);
	unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
};

#endif MODEL_H

