#include "texture.h"
#include <string>
#include <fstream>
#include <SOIL/SOIL.h>
#define GLEW_STATIC
#include <GL/glew.h>

namespace hack_game {

	static const float BORDER_COLOR[] = {0, 0, 0, 0};

	Texture::Texture(const char* path) {
		data = SOIL_load_image(path, &width, &height, nullptr, SOIL_LOAD_RGBA);

		if (data == nullptr) {
			throw std::ios_base::failure(std::string("Cannot open file '") + path + "'");
		}
	}

	Texture::~Texture() {
		if (data != nullptr) {
			SOIL_free_image_data(data);
		}
	}


	GLuint Texture::genGlTexture() const {
		return genGlTexture(GL_LINEAR);
	}

	GLuint Texture::genGlTexture(GLint filter) const {
		GLuint textureId;
		glGenTextures(1, &textureId);
		bindGlTexture(textureId, filter);
		return textureId;
	}

	void Texture::bindGlTexture(GLuint textureId) const {
		bindGlTexture(textureId, GL_LINEAR);
	}

	void Texture::bindGlTexture(GLuint textureId, GLint filter) const {
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BORDER_COLOR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}