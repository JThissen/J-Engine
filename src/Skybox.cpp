//#include "Skybox.h"
//#include "Helpers.h"
//
//Skybox::Skybox()
//{
//	vs_skybox = "./ShaderFiles/skybox.vs";
//	fs_skybox = "./ShaderFiles/skybox.fs";
//	right = "./Images/skybox/right.jpg";
//	left = "./Images/skybox/left.jpg";
//	top = "./Images/skybox/top.jpg";
//	bottom = "./Images/skybox/bottom.jpg";
//	front = "./Images/skybox/front.jpg";
//	back = "./Images/skybox/back.jpg";
//
//	shaderBuilder.CreateShader()
//		.AttachVertexShader(vs_skybox)
//		.AttachFragmentShader(fs_skybox);
//
//	shader_skybox = shaderBuilder.BuildShader();
//	shader_skybox.UseShader();
//
//	std::vector<std::string> faces = { right, left, top, bottom, front, back };
//	texture_cubemap = Helpers::Load::loadCubemap(faces);
//
//	shader_skybox.UseShader();
//	glUniform1i(glGetUniformLocation(shader_skybox.GetShaderProgram(), "skybox"), 0);
//}
//
//void Skybox::Draw(glm::mat4 projection, glm::mat4 view)
//{
//	glDepthFunc(GL_LEQUAL);
//	shader_skybox.UseShader();
//	view = glm::mat4(glm::mat3(view));
//	glUniformMatrix4fv(glGetUniformLocation(shader_skybox.GetShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
//	glUniformMatrix4fv(glGetUniformLocation(shader_skybox.GetShaderProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
//	glActiveTexture(GL_TEXTURE0 + 0);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cubemap);
//	Helpers::Draw::drawSkyboxCube();
//	glDepthFunc(GL_LESS);
//}
