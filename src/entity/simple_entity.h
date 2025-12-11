#ifndef HACK_GAME__ENTITY__SIMPLE_ENTITY_H
#define HACK_GAME__ENTITY__SIMPLE_ENTITY_H

#include "entity.h"
#include <glm/mat4x4.hpp>

namespace hack_game {

	/**
	 * @brief Простая сущность. Содержит шейдер для отрисовки и модель. Подходит для большинства объектов.
	 */
	class SimpleEntity: public virtual Entity {
	protected:
		Shader& shader;
		const Model& model;

	public:
		SimpleEntity(Shader& shader, const Model& model) noexcept:
				shader(shader), model(model) {}

		GLuint getShaderProgram() const noexcept override;
		void tick(Level&) override {}
		void draw() const override;

		/// @return Матрицу трансформации модели. По умолчанию возвращает матрицу, которая никак не изменяет модель.
		virtual glm::mat4 getModelTransform() const;
	};
}

#endif