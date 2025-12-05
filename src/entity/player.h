#ifndef HACK_GAME__ENTITY__PLAYER_H
#define HACK_GAME__ENTITY__PLAYER_H

#include "damageable.h"
#include "entity_with_pos.h"
#include "camera.h"

namespace hack_game {

	class Animation;

	class Player final: public Damageable, public EntityWithPos {
	public:
		static constexpr float RADIUS = 0.02f;

	private:
		ShaderManager& shaderManager;
		Camera camera;
		const float speed;

		std::shared_ptr<Animation> animation = nullptr;
		glm::vec3 pos;

		float angle = 0.0f;
		float targetAngle = 0.0f;
		float timeSinceLastBullet = 0.0f;
		
		bool up    = false;
		bool down  = false;
		bool left  = false;
		bool right = false;
		bool fire  = false;

	public:
		Player(ShaderManager&, const Camera&, float speed, const glm::vec3& pos);

		const glm::vec3& getPos() const noexcept override {
			return pos;
		}
		
		const Camera& getCamera() const noexcept {
			return camera;
		}

		GLuint getShaderProgram() const noexcept override;
		std::shared_ptr<const Player> shared_from_this() const;
		
		void updateKeys();
		void tick(Level&) override;
		void draw() const override;
		bool hasCollision(const glm::vec3&) const override;
		void damage(Level&, hp_t) override;
			
	protected:
		void onDestroy(Level&) override;

	private:
		void updateAngle(float targetAngle);
		void move(Level&);
	};
}

#endif
