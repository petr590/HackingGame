#ifndef HACK_GAME__ENTITY__ANIMATION__ANIMATION_H
#define HACK_GAME__ENTITY__ANIMATION__ANIMATION_H

#include "entity/simple_entity.h"
#include "model/models.h"
#include <memory>

namespace hack_game {

	class EntityWithPos;

	class Animation: public SimpleEntity, public std::enable_shared_from_this<Entity> {
	protected:
		const std::shared_ptr<const EntityWithPos> entity;
		const float size;
		const float duration;
		const float yOffset;
		float time = 0;

	public:
		/**
		 * @brief Инициализирует объект анимации.
		 * @param entity сущность, к которой привязана анимация. Необхдимо для корректного получения позиции
		 * @param shader шейдер для отрисовки модели. Если используется несколько шейдеров, необходимо указать nullShader
		 * @param duration время жизни анимации
		 * @param size размер модели
		 * @param yOffset смещение позиции модели по y относительно позиции сущности
		 * @param model модель, которая будет отрисовываться
		 */
		Animation(std::shared_ptr<const EntityWithPos>&& entity, Shader& shader, float duration, float size, float yOffset, Model& = models::plane) noexcept;

		float getTime() const noexcept {
			return time;
		}

		float getProgress() const noexcept {
			return time / duration;
		}

		bool isFinished() const noexcept {
			return time >= duration;
		}

		bool isTransparent() const noexcept override {
			return true;
		}

		void tick(Level&) override;
		void draw() const override;
		glm::mat4 getModelTransform() const override;
	
	protected:
		virtual void onRemove(Level&) {}
		glm::vec3 getPos() const noexcept;
	};
}

#endif