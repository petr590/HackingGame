#include "bullet.h"
#include "enemy.h"
#include "player.h"
#include "block.h"
#include "model/models.h"
#include "shader/shader.h"
#include "level/level.h"
#include "util.h"

#include <glm/gtc/matrix_transform.hpp>

namespace hack_game {
	using std::min;
	using std::max;
	using std::clamp;
	using std::isinf;

	using glm::uvec2;
	using glm::vec2;
	using glm::vec3;
	using glm::mat4;

	static const float LIMIT = 5.0f;
	static const float ENEMY_BULLET_RADIUS = Enemy::RADIUS;


	Bullet::Bullet(
		Shader& shader,
		const Model& model,
		float angle,
		const glm::vec3& velocity,
		const glm::vec3& pos
	) noexcept:
			SimpleEntity(shader, model),
			angle(angle),
			velocity(velocity),
			pos(pos) {}

	void Bullet::tick(Level& level) {
		pos += velocity * level.getDeltaTime();

		if (checkCollision(level)) {
			level.removeEntity(shared_from_this());
			return;
		}

		if (pos.x > LIMIT || pos.x < -LIMIT ||
			pos.z > LIMIT || pos.z < -LIMIT) {

			level.removeEntity(shared_from_this());
		}
	}

	mat4 Bullet::getModelTransform() const {
		mat4 model(1.0f);
		model = glm::translate(model, pos);
		return  glm::rotate(model, angle, vec3(0.0f, 1.0f, 0.0f));
	}





	static Block* getCollisionWithBlock(Level& level, const vec2& pos2d) {
		const uvec2 mapPos = level.getMapPos(pos2d);

		if (level.map[mapPos] == nullptr) {
			return nullptr;
		}

		const AABB block = level.map[mapPos]->getHitbox();

		if (block.containsInclusive(pos2d)) {
			return level.map[mapPos].get();
		}

		return nullptr;
	}


	// ---------------------------------------- PlayerBullet ----------------------------------------

	PlayerBullet::PlayerBullet(Shader& shader, float angle, const glm::vec3& velocity, const glm::vec3& pos):
			Bullet(shader, models::playerBullet, angle, velocity, pos) {}


	bool PlayerBullet::checkCollision(Level& level) {
		Block* block = getCollisionWithBlock(level, vec2(pos.x, pos.z));
		if (block != nullptr) {
			block->damage(level, 1);
			return true;
		}

		for (const auto& damageable : level.getDamageableEnemyEntities()) {
			if (!damageable->destroyed() && damageable->hasCollision(pos)) {
				damageable->damage(level, 1);
				return true;
			}
		}

		return false;
	}


	// ---------------------------------------- EnemyBullet ----------------------------------------

	EnemyBullet::EnemyBullet(Shader& shader, bool unbreakable, const glm::vec3& velocity, const glm::vec3& pos):
			Bullet(
				shader,
				unbreakable ? models::unbreakableSphere : models::breakableSphere,
				0.0f, velocity, pos
			),
			Damageable(Side::ENEMY, unbreakable ? MAX_HP : 1) {}


	mat4 EnemyBullet::getModelTransform() const {
		mat4 model = Bullet::getModelTransform();
		return glm::scale(model, vec3(0.75f));
	}


	bool EnemyBullet::hasCollision(const vec3& point) const {
		return isPointInsideSphere(point, pos, ENEMY_BULLET_RADIUS);
	}


	void EnemyBullet::onDestroy(Level& level) {
		level.removeEntity(shared_from_this());
	}
	
	
	bool EnemyBullet::checkCollision(Level& level) {
		Block* block = getCollisionWithBlock(level, vec2(pos.x, pos.z));
		if (block != nullptr) {
			return true;
		}

		const auto& player = level.getPlayer();

		if (!player->destroyed() && hasCollision(player->getPos())) {
			player->damage(level, 1);
			return true;
		}

		return false;
	}
}
