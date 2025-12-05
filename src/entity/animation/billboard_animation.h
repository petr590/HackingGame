#ifndef HACK_GAME__ENTITY__ANIMATION__BILLBOARD_ANIMATION_H
#define HACK_GAME__ENTITY__ANIMATION__BILLBOARD_ANIMATION_H

#include "animation.h"

namespace hack_game {

	/**
	 * @brief Поворачивает модель в сторону камеры
	 */
	class BillboardAnimation: public Animation {
		glm::mat4 view {0.0f};
		
	public:
		using Animation::Animation;

		const glm::mat4& getView() const noexcept {
			return view;
		}
		
		void tick(Level&) override;
		glm::mat4 getModelTransform() const override;
	};
}

#endif