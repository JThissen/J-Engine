#ifndef SCENE_H
#define SCENE_H

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Bloom.h"
#include "VolumeLight.h"
#include "Terrain.h"

class Scene
{
private:
	Bloom				bloom;
	VolumeLight			volumetric_light;
	Terrain				terrain;

public:
	enum class Scenes { TERRAIN, VOLUMETRIC_LIGHT };

	Scene() = delete;
	Scene(int window_width, int window_height);
	void draw_scene(float delta_time, const glm::mat4& m_projection, const glm::mat4& m_view, const glm::mat4& m_model, View& view, Scenes scenes);
};

#endif