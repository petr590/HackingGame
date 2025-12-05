#ifndef HACK_GAME__ENTITY__ENTITY_H
#define HACK_GAME__ENTITY__ENTITY_H

#include "gl_fwd.h"
#include <glm/vec3.hpp>

namespace hack_game {

	class Model;
	class Shader;
	class ShaderManager;
	class Level;

	/**
	 * @brief Класс сущности. Сущность - это объект на сцене. Она может иметь своё состояние и кастомный код отрисовки
	 */
	class Entity {
	protected:
		constexpr Entity() = default;

	public:
		virtual ~Entity() = default;

		/// @return используемую шейдерную программу или 0. Данное значение используется для группировки сущностей по шейдерам, что нужно для оптимизации.
		/// Если сущность использует более одного шейдера, то данный метод должен вернуть 0. Такие особые сущности будут обработаны вне какой-то группы.
		virtual GLuint getShaderProgram() const noexcept = 0;

		/// @brief Обновляет состояние сущности
		virtual void tick(Level&) = 0;
		
		/// @brief Отрисовывает сущность в текущий фреймбуфер
		virtual void draw() const = 0;

		/// @return true, если у сущности есть (или может быть) альфа-канал. По умолчанию возвращает false.
		/// @note Этот метод должен всегда возвращать одинаковое значение для одной сущности,
		/// так как он используется для определения, в какой список добавить сущность.
		virtual bool isTransparent() const noexcept {
			return false;
		}
	};
}

#endif
