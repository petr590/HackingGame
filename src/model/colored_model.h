#ifndef HACK_GAME__MODEL__SIMPLE_MODEL_H
#define HACK_GAME__MODEL__SIMPLE_MODEL_H

#include "vao_model.h"
#include <string>

namespace hack_game {

	class ColoredModel: public VAOModel {
		struct Vertex;

		std::vector<Vertex> vertices;
		const glm::vec3 color;
	
	public:
		ColoredModel(uint32_t color, const char* relativePath);
		ColoredModel(uint32_t color, const ColoredModel&);
		~ColoredModel();

		const glm::vec3& getColor() const noexcept {
			return color;
		}

		GLuint createVertexArray() override;

		void draw(Shader&) const override;
		void draw(Shader&, const glm::vec3& color) const;
	};
}

#endif