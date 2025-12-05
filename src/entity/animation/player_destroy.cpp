#include "player_destroy.h"
#include "entity/player.h"
#include "level/level.h"
#include "shader/shader_manager.h"
#include "main/globals.h"
#include "util.h"
#include <glm/common.hpp>

namespace hack_game {
	using Cube = PlayerDestroyAnimation::Cube;

	using glm::vec3;
	using glm::mat4;

	static const float DURATION = 1.3f * 5; //  DEBUG
	static const float SIZE     = 5;

	class PlayerDestroyAnimation::Cube {
		const float scale;
		const float maxLifetime;
		float lifetime = 0;

		Cube(float scale, float maxLifetime) noexcept:
				scale(scale), maxLifetime(maxLifetime) {}
	};

	PlayerDestroyAnimation::PlayerDestroyAnimation(std::shared_ptr<const EntityWithPos>&& entity, ShaderManager& shaderManager):
			FlatAndBillboardAnimation(
				std::move(entity), shaderManager,
				shaderManager.getShader("playerDestroyFlat"),
				shaderManager.getShader("playerDestroyBillboard"),
				DURATION, SIZE, Player::RADIUS
			),
			angleNormal(0.0f, 1.0f, 0.0f),
			seed(randomInt32()) {

		destroyAnimationCount += 1;
	}

	PlayerDestroyAnimation::~PlayerDestroyAnimation() {}

	void PlayerDestroyAnimation::tick(Level& level) {
		FlatAndBillboardAnimation::tick(level);

		const Camera& camera = level.getPlayer()->getCamera();
		angleNormal = glm::normalize(glm::rotate(camera.getPos() - camera.getTarget(), glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)));
	}


	void PlayerDestroyAnimation::onRemove(Level&) {
		destroyAnimationCount -= 1;
	}

	mat4 PlayerDestroyAnimation::getFlatShaderModelTransform() const {
		return glm::scale(FlatAndBillboardAnimation::getFlatShaderModelTransform(), vec3(0.1f));
	}

	void PlayerDestroyAnimation::setFlatShaderUniforms() const {
		flatShader.setUniform("seed", seed);
	}

	void PlayerDestroyAnimation::setBillboardShaderUniforms() const {
		billboardShader.setUniform("angleNormal", angleNormal);
		billboardShader.setUniform("seed", seed);
	}
}