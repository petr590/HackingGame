#ifndef HACK_GAME__ENTITY__DAMAGEABLE_H
#define HACK_GAME__ENTITY__DAMAGEABLE_H

#include "entity.h"
#include <memory>
#include <glm/vec3.hpp>

namespace hack_game {

	/// Тип хитпоинтов сущностей
	using hp_t = int32_t;

	/// Сторона, которой принадлежит сущность
	enum class Side: uint32_t {
		PLAYER, ENEMY
	};

	/**
	 * Сущность, которая имеет хп и может получать урон.
	 * Сущность может быть неуязвимой - в таком случае по ней можно попасть, но урон нанесён не будет
	 */
	class Damageable: public virtual Entity, public virtual std::enable_shared_from_this<Entity> {
	protected:
		const Side side;
		hp_t hitpoints;
	
	public:
		/// Это значение хп делает сущность неуязвимой к любому урону
		static constexpr hp_t MAX_HP = 0x7FFFFFFF;

		/**
		 *  Создаёт новую сущность.
		 * @param side сторона, которой принадлежит сущность
		 * @param hitpoints изначальное количество хп. Для неуязвимости укажите MAX_HP
		 */
		Damageable(Side side, hp_t hitpoints) noexcept;

		/**
		 * Проверяет коллизию с точкой
		 * @param point позиция точки для проверки
		 * @return true, если есть коллизия с точкой, иначе false
		 */
		virtual bool hasCollision(const glm::vec3& point) const = 0;

		/// @brief Наносит урон по сущности, если она не неуязвимая
		virtual void damage(Level&, hp_t damage);

		bool destroyed() const noexcept {
			return hitpoints <= 0;
		}

		bool invulnerable() const noexcept {
			return hitpoints == MAX_HP;
		}

		Side getSide() const noexcept {
			return side;
		}
	
	protected:
		/// Вызывается при уничтожении сущности только 1 раз, когда this->hitpoints <= 0. Не путать с деструктором!
		virtual void onDestroy(Level&);
	};
}

#endif