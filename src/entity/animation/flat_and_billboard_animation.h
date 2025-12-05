#ifndef HACK_GAME__ENTITY__ANIMATION__FLAT_AND_BILLBOARD_ANIMATION_H
#define HACK_GAME__ENTITY__ANIMATION__FLAT_AND_BILLBOARD_ANIMATION_H

#include "billboard_animation.h"

namespace hack_game {

	/**
	 * @brief Отрисовывает модель с помощью двух шейдеров - flatShader и billboardShader.
	 * Класс содержит всего одну модель (как правило, плоский квадрат), но применяет к ней разные матрицы трансформации и шейдеры.
	 * Каждый шейдер рисует на модели свою анимацию.
	 */
	class FlatAndBillboardAnimation: public BillboardAnimation {
	protected:
		Shader& flatShader;
		Shader& billboardShader;
	
	public:
		FlatAndBillboardAnimation(
			std::shared_ptr<const EntityWithPos>&& entity, ShaderManager& shaderManager, Shader& flatShader, Shader& billboardShader,
			float duration, float size, float yOffset, Model& model = models::plane
		);

		/// @brief Отрисовывает модель два раза: сначала с использованием flatShader, потом с использованием billboardShader
		/// Перед отрисовкой устанавливает uniform-переменные view, model, centerPos, progress для каждого шейдера
		void draw() const override;
	
	protected:
		/// @return Матрицу трансформации плоской модели. По умолчанию, возвращает неповёрнутую матрицу (Animation::getModelTransform)
		virtual glm::mat4 getFlatShaderModelTransform() const;

		/// @return Матрицу трансформации модели, обращённой в сторону камеры. По умолчанию, возвращает матрицу BillboardAnimation::getModelTransform
		virtual glm::mat4 getBillboardShaderModelTransform() const;

		/// @brief Устанавливает значения дополнительных uniform-переменные для flatShader. По умолчанию ничего не делает
		virtual void setFlatShaderUniforms() const {}

		/// @brief Устанавливает значения дополнительных uniform-переменные для billboardShader. По умолчанию ничего не делает
		virtual void setBillboardShaderUniforms() const {}
	};
}

#endif