#include "enemy.h"
#include "player.h"
#include "bullet.h"
#include "animation/enemy_damage.h"
#include "animation/enemy_destroy.h"
#include "model/models.h"
#include "shader/shader_manager.h"
#include "level/level.h"
#include "main/globals.h"
#include "util.h"

#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace hack_game {
	using std::isnan;
	using std::clamp;
	using std::make_shared;

	using glm::vec2;
	using glm::vec3;
	using glm::mat4;

	using shared_entity = std::enable_shared_from_this<Entity>;


	static const float BRIGHT_DURATION = 0.04f;


	Enemy::Enemy(ShaderManager& shaderManager, float bulletSpawnPeriod, const vec3& pos) noexcept:
			SimpleEntity(shaderManager.mainShader, models::sphere),
			Damageable(Side::ENEMY, 7),
			shaderManager(shaderManager),
			coloredModel(models::sphere),
			bulletSpawnPeriod(bulletSpawnPeriod),
			pos(pos) {}


	bool Enemy::hasCollision(const vec3& point) const {
		return isPointInsideSphere(point, pos, RADIUS);
	}

	std::shared_ptr<const Enemy> Enemy::shared_from_this() const {
		return std::dynamic_pointer_cast<const Enemy>(shared_entity::shared_from_this());
	}


	void Enemy::damage(Level& level, hp_t damage) {
		Damageable::damage(level, damage);

		if (destroyed()) {
			enemyDestroyed = true;

			animation = make_shared<EnemyDestroyAnimation>(std::move(shared_from_this()), shaderManager);
			level.addEntity(animation);
			level.removeEntity(shared_entity::shared_from_this());

		} else if (animation == nullptr || animation->isFinished()) {

			animation = make_shared<EnemyDamageAnimation>(std::move(shared_from_this()), shaderManager);
			level.addEntity(animation);
		}
	}

	void Enemy::tick(Level& level) {
		time += level.getDeltaTime();

		if (time >= bulletSpawnPeriod) {
			time -= bulletSpawnPeriod;

			// spawnBullets(level); // DEBUG
		}
	}

	void Enemy::draw() const {
		bool bright = animation != nullptr && animation->getTime() <= BRIGHT_DURATION;

		if (bright) {
			shader.setUniform("modelBrightness", 1.5f);
		}
		
		shader.setModel(getModelTransform());
		coloredModel.draw(shader);

		if (bright) {
			shader.setUniform("modelBrightness", 1.0f);
		}
	}

	mat4 Enemy::getModelTransform() const {
		return glm::translate(mat4(1.0f), pos);
	}


	static const float ROTATE_PER_SEC = glm::radians(45.0f);

	Enemy1::Enemy1(ShaderManager& shaderManager, const vec3& pos) noexcept:
			Enemy(shaderManager, 0.5f, pos) {}
	
	
	void Enemy1::tick(Level& level) {
		float targetAngle = horizontalAngleBetween(pos, level.getPlayer()->getPos());

		if (!isnan(targetAngle)) {
			angle += clamp(targetAngle - angle, -ROTATE_PER_SEC, ROTATE_PER_SEC);
		}

		Enemy::tick(level);
	}


	void Enemy1::spawnBullets(Level& level) {
		vec2 velocity0 = ANGLE_NORMAL * EnemyBullet::DEFAULT_SPEED;

		for (int i = 0; i < 5; i++) {
			vec2 velocity = glm::rotate(velocity0, angle + glm::radians(-90.0f + i * 45));

			level.addEntity(make_shared<EnemyBullet>(
				shaderManager.getShader("light"), spawnUnbreakable, vec3(velocity.x, 0.0f, velocity.y), pos
			));
		}

		spawnUnbreakable = !spawnUnbreakable;
	}
}
