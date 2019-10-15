#include "Camera.h"
#include "Helpers.h"

Camera::Camera(const int windowWidth, const int windowHeight, float velocity)
{
	enterScreen = true;
	fill = true;
	previousX = windowWidth / 2.0f;
	previousY = windowWidth / 2.0f;
	yaw = -63.8f;
	pitch = -23.5f;
	fov = 45.0f;
	cameraPos = glm::vec3(-8.4f, 7.6f, 14.2f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	lock = false;
}

void Camera::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov <= 45.0f && fov >= 1.0f)
		fov -= (float)yoffset;
	if (fov >= 45.0f)
		fov = 45.0f;
	if (fov <= 1.0f)
		fov = 1.0f;
}

void Camera::cursorCallback(GLFWwindow * window, double xpos, double ypos)
{
	if (!lock)
	{
		float xposf = (float)xpos;
		float yposf = (float)ypos;

		if (enterScreen)
		{
			previousX = xposf;
			previousY = yposf;
			enterScreen = false;
		}

		float xOffset = xposf - previousX;
		float yOffset = previousY - yposf;

		previousX = xposf;
		previousY = yposf;

		float sensitivityFactor = 0.1f;
		xOffset *= 0.1f;
		yOffset *= 0.1f;

		yaw += xOffset;
		pitch += yOffset;

		if (pitch >= 89.0f)
			pitch = 89.0f;
		if (pitch <= -89.0f)
			pitch = -89.0f;

		glm::vec3 direction;
		float radius = 1.0f;
		direction.x = radius * cos(glm::radians(pitch)) * radius * cos(glm::radians(yaw));
		direction.y = radius * sin(glm::radians(pitch));
		direction.z = radius * cos(glm::radians(pitch)) * radius * sin(glm::radians(yaw));
		cameraFront = glm::normalize(direction);
	}
}

void Camera::processInput(GLFWwindow* window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	Fill(window);
	Lock(window);

	float velocity = 10.0f;
	float distance = deltaTime * velocity;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += distance * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos += distance * -cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos += distance * -glm::normalize(glm::cross(cameraFront, cameraUp));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += distance * glm::normalize(glm::cross(cameraFront, cameraUp));
}

void Camera::Fill(GLFWwindow* window)
{
	bool currentlyPressed = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS;

	if (currentlyPressed && !previouslyPressed)
	{
		if (fill)
			Helpers::Globals::toggleTerrain = false;
		else
			Helpers::Globals::toggleTerrain = true;
		fill = !fill;
	}

	previouslyPressed = currentlyPressed;
}

void Camera::Lock(GLFWwindow* window)
{
	bool currentlyPressed = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;

	if (currentlyPressed && !previouslyPressed_locked)
		lock = !lock;

	previouslyPressed_locked = currentlyPressed;
}

void Camera::ShowCursor(GLFWwindow* window)
{
	bool currentlyPressed = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;

	if (currentlyPressed && !previouslyPressed_cursor)
		Helpers::Globals::showCursor = !Helpers::Globals::showCursor;

	previouslyPressed_cursor = currentlyPressed;
}

float& Camera::Getfov() { return fov; }
const glm::vec3& Camera::GetCameraPos() { return cameraPos; }
const glm::vec3& Camera::GetCameraFront() { return cameraFront; }
const glm::vec3& Camera::GetCameraUp() { return cameraUp; }
void Camera::SetCameraPos(glm::vec3 pos) { cameraPos = pos; }
void Camera::InvertPitch() { pitch = -pitch; }
