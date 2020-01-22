#include "Scene.h"


Scene::Scene(int window_width, int window_height)
{
	//bloom = Bloom(window_width, window_height);
	volumetric_light = VolumeLight(window_width, window_height);
	terrain = Terrain(window_width, window_height);
}

void Scene::draw_scene(float delta_time, const glm::mat4& m_projection, const glm::mat4& m_view, const glm::mat4& m_model, View& view, Scenes scenes)
{
	switch (scenes)
	{
		case Scenes::TERRAIN:
		{
			terrain.draw(m_projection, m_view, m_model, view);
			break;
		}

		case Scenes::VOLUMETRIC_LIGHT:
		{
			volumetric_light.calc_movement(delta_time);
			volumetric_light.draw_normal(m_projection, m_view, m_model);
			volumetric_light.draw_occlusion(m_projection, m_view, m_model);
			volumetric_light.draw_light_shafts(m_projection, m_view);
			break;
		}
	}
}