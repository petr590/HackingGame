#include "animation.h"
#include "entity/enemy.h"
#include "entity/player.h"
#include "entity/entity_with_pos.h"
#include "shader/shader.h"
#include "level/level.h"
#include <glm/gtc/matrix_transform.hpp>

namespace hack_game {
	using std::shared_ptr;
	using std::move;

	using glm::vec3;
	using glm::vec4;
	using glm::mat3;
	using glm::mat4;

	Animation::Animation(shared_ptr<const EntityWithPos>&& entity, Shader& shader, float duration, float size, float yOffset, Model& model) noexcept:
			SimpleEntity(shader, model), entity(move(entity)), size(size), duration(duration), yOffset(yOffset) {}
	

	void Animation::tick(Level& level) {
		time += level.getDeltaTime();

		if (time >= duration) {
			level.removeEntity(shared_from_this());
			onRemove(level);
		}
	}


	void Animation::draw() const {
		shader.setUniform("centerPos", getPos());
		shader.setUniform("progress", getProgress());
		SimpleEntity::draw();
	}


	mat4 Animation::getModelTransform() const {
		mat4 modelMat(1.0f);
		modelMat = glm::translate(modelMat, getPos());
		return glm::scale(modelMat, vec3(size));
	}

	vec3 Animation::getPos() const noexcept {
		vec3 pos = entity->getPos();
		pos.y += yOffset;
		return pos;
	}
}