#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "shaderClass.h"
#include <chrono>

class Camera {
private:
	std::chrono::steady_clock::time_point lastActionTime;

	void CheckColisionConditions(glm::vec3 Position);
public:
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(-0.7f, 0.0f, -0.6f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 cameraMatrix = glm::mat4(1.0f);

	int width;
	int height;
	bool firstClick = true;

	float speed = 0.5f;
	float sensitivity = 100.0f;

	Camera(int width, int height, glm::vec3 position);

	// Updates the camera matrix to the Vertex Shader
	void UpdateMatrix(float FOVdeg, float nearPlane, float farPlane);
	// Exports the camera matrix to a shader
	void Matrix(Shader& shader, const char* uniform);
	void Inputs(GLFWwindow* window);
	void ToggleFullscreen(GLFWwindow* window);

};

#endif