#include "Scene.h"
#include "Helpers.h"

Scene::Scene(int windowWidth, int windowHeight)
	: windowWidth(windowWidth), windowHeight(windowHeight)
{
	//bloom = Bloom(windowWidth, windowHeight);
	volumeLight = VolumeLight(windowWidth, windowHeight);
	terrain = Terrain(windowWidth, windowHeight);
}

Scene::~Scene()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Scene::DrawScene(float deltaTime, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, Camera& camera, Scenes scenes)
{
	switch (scenes)
	{
		case Scenes::TERRAIN:
		{
			terrain.Draw(projection, view, model, camera);
			break;
		}

		case Scenes::VOLUMETRIC_LIGHT:
		{
			volumeLight.CalcMovement(deltaTime);
			volumeLight.DrawNormal(projection, view, model);
			volumeLight.DrawOcclusion(projection, view, model);
			volumeLight.DrawLightShafts(projection, view);
			break;
		}
	}
}