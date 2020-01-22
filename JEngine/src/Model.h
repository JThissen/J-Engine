#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "Mesh.h"

class Model
{
public:
    Model(std::string const& path, bool gamma = false);
    void draw(const Shader& shader);

private:
    std::string directory;
    bool gammaCorrection;
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;


    void load_model(std::string const& path);
    void process_node(aiNode* node, const aiScene* scene);
    Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> load_material_textures(aiMaterial* mat, aiTextureType type, std::string typeName);
    unsigned int texture_from_file(const char* path, const std::string& directory, bool gamma = false);
};
#endif