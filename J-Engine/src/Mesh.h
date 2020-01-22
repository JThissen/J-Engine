#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "Shader.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
    glm::vec3 tangent;
    glm::vec3 bitangent;

    Vertex() = default;
    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv_coords, glm::vec3 tangent, glm::vec3 bitangent);
};

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;

    Texture() = default;
    Texture(unsigned int id, std::string type, std::string path);
};

class Mesh
{
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures);
    void draw(const Shader& shader);

private:
    unsigned int VAO, VBO, EBO;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    void fill_buffers();
};
#endif