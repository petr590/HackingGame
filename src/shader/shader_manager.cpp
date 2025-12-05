#include "shader_manager.h"
#include "util.h"
#include <glm/gtc/matrix_transform.hpp>

namespace hack_game {

	using glm::vec2;
	using glm::vec3;
	using glm::mat4;

	static constexpr vec3 lightColor(1.0f, 1.0f, 1.0f);
	static constexpr vec3 lightPos(0.5f, 0.6f, 0.5f);


	Shader& ShaderManager::getShader(const char* name) {
		return shaders.at(name);
	}

	Shader& ShaderManager::getShader(GLuint id) {
		return *shadersById.at(id);
	}

	void ShaderManager::addShader(Shader&& shader) {
		Shader& newShader = shaders.emplace(shader.getName(), std::move(shader)).first->second;
		shadersById.emplace(shader.getId(), &newShader);
	}

	void ShaderManager::initShaders(int windowWidth, int windowHeight) {
		const mat4 projection = glm::perspective(45.0f, float(windowWidth) / float(windowHeight), 0.1f, 100.0f);

		mainShader.use();
		mainShader.setUniform("projection", projection);
		mainShader.setUniform("lightColor", lightColor);
		mainShader.setUniform("lightPos",   lightPos);

		Shader& postprocessing = shaders.at("postprocessing");
		postprocessing.use();
		postprocessing.setUniform("sceneTexture", 0);
		postprocessing.setUniform("guiTexture", 1);
		postprocessing.setUniform("seed", randomInt32());

		for (auto& entry : shaders) {
			entry.second.use();
			entry.second.setUniform("projection", projection);
		}

		shadersById.emplace(nullShader.getId(), &nullShader);
		shadersById.emplace(mainShader.getId(), &mainShader);
	}

	void ShaderManager::updateWindowSize(GLint width, GLint height) {
		Shader& postprocessing = shaders.at("postprocessing");
		postprocessing.use();
		postprocessing.setUniform("pixelSize", vec2(1.0f / width, 1.0f / height));
	}
}