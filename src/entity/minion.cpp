#include "minion.h"
#include "player.h"
#include "bullet.h"
#include "animation/minion_destroy.h"
#include "model/models.h"
#include "level/level.h"
#include "shader/shader_manager.h"
#include "util.h"
#include <glm/gtc/matrix_transform.hpp>

namespace hack_game {
	using std::isnan;
	using std::make_shared;
	using glm::vec2;
	using glm::vec3;
	using glm::mat4;

	static const float BULLET_PERIOD = 2.0f;
	static const float MINION_RADIUS = 0.015f;
	static const float MINION_SPEED = TILE_SIZE * 0.5f;

	Minion::Minion(ShaderManager& shaderManager, const glm::vec3& pos) noexcept:
			SimpleEntity(shaderManager.mainShader, models::minion),
			Damageable(Side::ENEMY, 1),
			shaderManager(shaderManager),
			pos(pos) {}
	

	std::shared_ptr<const Minion> Minion::shared_from_this() const {
		return std::dynamic_pointer_cast<const Minion>(std::enable_shared_from_this<Entity>::shared_from_this());
	}

	
	void Minion::tick(Level& level) {
		if (!level.getPlayer()->destroyed()) {
			float newAngle = horizontalAngleBetween(pos, level.getPlayer()->getPos());
			if (!isnan(newAngle))
				angle = newAngle;
			
			vec2 offset = glm::rotate(level.getDeltaTime() * MINION_SPEED * ANGLE_NORMAL, angle);
			offset = resolveBlockCollision(level, vec2(pos.x, pos.z), offset);
			pos += vec3(offset.x, 0, offset.y);
		}

		if (level.getPlayer()->destroyed()) return;

		time += level.getDeltaTime();

		if (time >= BULLET_PERIOD) {
			time -= BULLET_PERIOD;

			vec2 velocity = glm::rotate(ANGLE_NORMAL * EnemyBullet::DEFAULT_SPEED, angle);
			
			level.addEntity(make_shared<EnemyBullet>(
				shaderManager.getShader("light"), false, vec3(velocity.x, 0, velocity.y), pos
			));
		}
	}


	mat4 Minion::getModelTransform() const {
		mat4 model(1.0f);
		model = glm::translate(model, pos);
		return  glm::rotate(model, angle, vec3(0.0f, -1.0f, 0.0f));
	}


	bool Minion::hasCollision(const vec3& point) const {
		return isPointInsideSphere(point, pos, MINION_RADIUS);
	}

	void Minion::onDestroy(Level& level) {
		Damageable::onDestroy(level);
		level.addEntity(make_shared<MinionDestroyAnimation>(shared_from_this(), level, shaderManager));
	}
}