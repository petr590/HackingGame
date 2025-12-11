#ifndef HACK_GAME__ENTITY__PLATFORM_H
#define HACK_GAME__ENTITY__PLATFORM_H

#include "simple_entity.h"

namespace hack_game {

	class Platform: public SimpleEntity {
		const glm::vec2 scale;
		const bool isInfinite;

	public:
		Platform(Shader& shader, const glm::vec2& scale, bool isInfinite = false) noexcept;
		
		glm::mat4 getModelTransform() const override;
	};
}

#endif