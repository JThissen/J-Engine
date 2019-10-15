#ifndef CAMERA_H
#define CAMERA_H

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

class Camera
{
public:
	bool		enterScreen;
	bool		fill;
	bool		previouslyPressed;
	bool		previouslyPressed_locked;
	bool		previouslyPressed_cursor;
	bool		lock;
	float		previousX;
	float		previousY;
	float		yaw;
	float		pitch;
	float		fov;
	float		velocity;
	glm::vec3	cameraPos;
	glm::vec3	cameraFront;
	glm::vec3	cameraUp;

public:
	Camera(const int windowWidth, const int windowHeight, float velocity = 10.0f);
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void cursorCallback(GLFWwindow* window, double xpos, double ypos);
	void processInput(GLFWwindow* window, float deltaTime);
	void Fill(GLFWwindow* window);
	void Lock(GLFWwindow* window);
	void ShowCursor(GLFWwindow* window);

	float& Getfov();
	const glm::vec3& GetCameraPos();
	const glm::vec3& GetCameraFront();
	const glm::vec3& GetCameraUp();
	void SetCameraPos(glm::vec3 pos);
	void InvertPitch();
};


#endif