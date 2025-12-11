#ifndef HACK_GAME__ENTITY__WALLS_H
#define HACK_GAME__ENTITY__WALLS_H

#include "simple_entity.h"

namespace hack_game {

	class Walls: public SimpleEntity {
	public:
		Walls(Shader& shader) noexcept;
		
		glm::mat4 getModelTransform() const override;
	};
}

#endif