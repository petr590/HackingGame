#include "minion_destroy.h"
#include "entity/player.h"
#include "entity/entity_with_pos.h"
#include "model/models.h"
#include "shader/shader_manager.h"
#include "level/level.h"
#include "cube_particle_mode.h"
#include "util.h"

namespace hack_game {
	using std::sin;
	using std::cos;
	using std::shared_ptr;
	using std::make_shared;

	using glm::vec3;
	using glm::mat4;

	using Cube = MinionDestroyAnimation::Cube;


	static const float DURATION = 0.35f;
	static const float SIZE     = 1.0f;
	static const float Y_OFFSET = 0.5f * TILE_SIZE;

	static const int MAX_CUBES = 10;

	static const float SKIP_CUBE_CHANCE = 0.15f;
	static const float FRAME_MODE_CHANCE = 0.15f;

	static const float MIN_CUBE_DISTANCE = TILE_SIZE * 0.5f;
	static const float MAX_CUBE_DISTANCE = TILE_SIZE;
	
	static const float MIN_CUBE_SPEED = TILE_SIZE;
	static const float MAX_CUBE_SPEED = TILE_SIZE * 3.0f;

	static constexpr float CUBE_ROTATE_AXIS_ANGLE = glm::radians(60.0f);
	static constexpr vec3 CUBE_ROTATE_AXIS_NORMAL (0.0f, sin(CUBE_ROTATE_AXIS_ANGLE), cos(CUBE_ROTATE_AXIS_ANGLE));


	class MinionDestroyAnimation::Cube: public Entity {
	public:
		MinionDestroyAnimation& parent;
		vec3 offset;
		vec3 speed;
		float scale;
		bool isFrame;

		Cube(MinionDestroyAnimation& parent, const vec3& offset, float speed, float scale, bool isFrame) noexcept:
				parent(parent), offset(offset), speed(glm::normalize(offset) * speed), scale(scale), isFrame(isFrame) {}
		
		mat4 getModelTransform() const {
			mat4 modelMat(1.0f);
			modelMat = glm::translate(modelMat, parent.entity->getPos() + offset);

			const float angle = parent.time * (glm::radians(360.0f) / DURATION);
			modelMat = glm::rotate(modelMat, angle, CUBE_ROTATE_AXIS_NORMAL);
			modelMat = glm::rotate(modelMat, CUBE_ROTATE_AXIS_ANGLE, vec3(-1.0f, 0.0f, 0.0f));
			modelMat = glm::scale(modelMat, vec3(scale));
			return     glm::translate(modelMat, vec3(0.0f, -0.02f, 0.0f));
		}

		GLuint getShaderProgram() const noexcept override {
			return parent.particleShader.getId();
		}

		void tick(Level& level) override {
			offset += speed * level.getDeltaTime();
		}

		bool isTransparent() const noexcept override {
			return true;
		}

		void draw() const override {
			Shader& shader = parent.particleShader;
			shader.setModel(getModelTransform());
			shader.setUniform("alpha", std::min(1.0f, 2.0f - parent.time * (2.0f / DURATION)));
			shader.setUniform("mode", static_cast<GLuint>(Mode::FADING));

			const Model& model = isFrame ? models::cubeFrame : models::blackCube;
			model.draw(shader);
		}
	};


	MinionDestroyAnimation::MinionDestroyAnimation(shared_ptr<const EntityWithPos>&& entity, Level& level, ShaderManager& shaderManager) noexcept:
			FlatAndBillboardAnimation(
				std::move(entity), shaderManager,
				shaderManager.getShader("minionDestroyFlat"),
				shaderManager.getShader("minionDestroyBillboard"),
				DURATION, SIZE, Y_OFFSET,
				models::minionDestroyBillboard
			),
			particleShader  (shaderManager.getShader("particleCube")),
			angleNormal     (0.0f, 1.0f, 0.0f),
			seed            (randomInt32()) {
		

		cubes.reserve(MAX_CUBES);

		for (int i = 0; i < MAX_CUBES; i++) {
			if (randomBetween(0.0f, 1.0f) < SKIP_CUBE_CHANCE) {
				continue;
			}

			const float angle    = i * (glm::radians(360.0f) / MAX_CUBES);
			const float distance = randomBetween(MIN_CUBE_DISTANCE, MAX_CUBE_DISTANCE);
			const float speed    = randomBetween(MIN_CUBE_SPEED, MAX_CUBE_SPEED);
			const float scale    = randomBetween(0.25f, 0.5f);
			const bool isFrame   = randomBetween(0.0f, 1.0f) < FRAME_MODE_CHANCE;

			const vec3 offset = vec3(0.0f, TILE_SIZE, 0.0f) + glm::rotate(vec3(distance, 0.0f, 0.0f), angle, vec3(0.0f, 1.0f, 0.0f));

			auto cube = make_shared<Cube>(*this, offset, speed, scale, isFrame);
			level.addEntity(cube);
			cubes.push_back(std::move(cube));
		}
	}

	MinionDestroyAnimation::~MinionDestroyAnimation() {}


	void MinionDestroyAnimation::tick(Level& level) {
		BillboardAnimation::tick(level);

		const Camera& camera = level.getPlayer()->getCamera();
		angleNormal = glm::normalize(glm::rotate(camera.getPos() - camera.getTarget(), glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)));
	}

	void MinionDestroyAnimation::onRemove(Level& level) {
		for (const shared_ptr<Cube>& cube : cubes) {
			level.removeEntity(cube);
		}
	}


	mat4 MinionDestroyAnimation::getFlatShaderModelTransform() const {
		return glm::scale(FlatAndBillboardAnimation::getFlatShaderModelTransform(), vec3(0.3f));
	}

	void MinionDestroyAnimation::setBillboardShaderUniforms() const {
		billboardShader.setUniform("angleNormal", angleNormal);
		billboardShader.setUniform("seed", seed);
	}
}