#include "enemy_destroy.h"
#include "cube_particle_mode.h"
#include "entity/player.h"
#include "entity/enemy.h"
#include "shader/shader_manager.h"
#include "level/level.h"
#include "main/globals.h"
#include "util.h"
#include <glm/gtc/matrix_transform.hpp>

namespace hack_game {
	using std::vector;

	using glm::vec3;
	using glm::mat4;

	using Cube = EnemyDestroyAnimation::Cube;


	// ---------------------------------------- constants -----------------------------------------

	static const float DURATION = 1;
	static const float SIZE = 10;

	static const float CUBES_START = 0.1f * DURATION;
	static const float CUBES_END   = 0.7f * DURATION;

	static const float MIN_SPAWN_SIZE = 4 * TILE_SIZE;
	static const float MAX_SPAWN_SIZE = 10 * TILE_SIZE;


	// ------------------------------------------- Cube -------------------------------------------

	static mat4 cubeModelTransform(const vec3& pos, float angle, const vec3& axis) {
		mat4 modelMat(1.0f);
		modelMat = glm::translate(modelMat, pos);
		return     glm::rotate(modelMat, angle, axis);
	}


	class EnemyDestroyAnimation::Cube {
	public:
		mat4 modelMat;
		float scale;
		float maxLifetime;
		float lifetime = 0;

		Cube(const vec3& pos, float angle, const vec3& axis, float scale, float maxLifetime):
			modelMat(cubeModelTransform(pos, angle, axis)),
			scale(scale),
			maxLifetime(maxLifetime) {}
		
		Cube(Cube&&) = default;
		Cube& operator=(Cube&&) = default;
	};


	// ---------------------------------- EnemyDestroyAnimation -----------------------------------

	EnemyDestroyAnimation::EnemyDestroyAnimation(std::shared_ptr<const EntityWithPos>&& entity, ShaderManager& shaderManager) noexcept:
			FlatAndBillboardAnimation(
				std::move(entity), shaderManager,
				shaderManager.getShader("enemyDestroyFlat"), shaderManager.getShader("enemyDestroyBillboard"),
				DURATION, SIZE, Enemy::RADIUS, models::enemyDestroyBillboard
			),
			particleShader  (shaderManager.getShader("particleCube")),
			seed            (randomInt32()) {
		
		destroyAnimationCount += 1;
	}

	EnemyDestroyAnimation::~EnemyDestroyAnimation() {}


	void EnemyDestroyAnimation::onRemove(Level&) {
		destroyAnimationCount -= 1;
	}


	// ------------------------------------------- tick -------------------------------------------

	static void updateCubes(vector<Cube>& cubes, float deltaTime) {
		if (cubes.empty()) return;

		for (Cube& cube : cubes) {
			cube.lifetime += deltaTime;
		}

		std::erase_if(cubes, [&] (const Cube& cube) { return cube.lifetime >= cube.maxLifetime; });
	}


	static void addCube(float time, const vec3& pos, vector<Cube>& fadingCubes, vector<Cube>& solidCubes, vector<Cube>& frameCubes) {
		const float spawnSize = zoom(time, CUBES_START, CUBES_END, MIN_SPAWN_SIZE, MAX_SPAWN_SIZE);

		const vec3 cubePos = randomBetween(
				vec3(pos.x - spawnSize, pos.y, pos.z - spawnSize),
				pos + spawnSize
		);

		const float angle = randomBetween(0.0f, glm::radians(360.0f));
		const vec3 axis = glm::normalize(randomBetween(vec3(-1.0f), vec3(1.0f)));

		const float minScale = zoom(time, CUBES_START, CUBES_END, 0.25f, 0.05f);
		const float maxScale = zoom(time, CUBES_START, CUBES_END, 0.5f, 0.1f);
		const float scale = randomBetween(minScale, maxScale);

		float lifetime;
		vector<Cube>* cubes;

		switch ((rand() >> 8) & 0x7) {
			case 0: case 1: case 2: case 3: case 4:
				lifetime = randomBetween(0.05f, 0.3f);
				cubes = &fadingCubes;
				break;
			
			case 5: case 6:
				lifetime = randomBetween(0.05f, 0.2f);
				cubes = &solidCubes;
				break;
			
			case 7:
				lifetime = randomBetween(0.05f, 0.1f);
				cubes = &frameCubes;
				break;
			
			default:
				return;
		}

		cubes->emplace_back(cubePos, angle, axis, scale, lifetime);
	}
	

	void EnemyDestroyAnimation::tick(Level& level) {
		BillboardAnimation::tick(level);

		if (time >= CUBES_START && time <= CUBES_END) {

			const int newCubes = static_cast<int>(randomBetween(1, 20) * randomBetween(1, 20) * level.getDeltaTime());

			for (int i = 0; i < newCubes; i++) {
				addCube(time, getPos(), fadingCubes, solidCubes, frameCubes);
			}
		}
		
		updateCubes(fadingCubes, level.getDeltaTime());
		updateCubes(solidCubes, level.getDeltaTime());
		updateCubes(frameCubes, level.getDeltaTime());
	}


	// ------------------------------------------- draw -------------------------------------------


	static void drawCubes(Shader& particleShader, const vector<Cube>& cubes, Mode mode, Model& model, float minScale) {
		particleShader.setUniform("mode", static_cast<GLuint>(mode));
			
		for (const Cube& cube : cubes) {
			float progress = cube.lifetime / cube.maxLifetime;
			mat4 modelMat = glm::scale(cube.modelMat, vec3(cube.scale * std::lerp(1.0f, minScale, progress)));
			
			particleShader.setModel(modelMat);
			particleShader.setUniform("alpha", std::exp(-6.0f * progress));

			model.draw(particleShader);
		}
	}
	

	void EnemyDestroyAnimation::draw() const {
		FlatAndBillboardAnimation::draw();

		if (time >= CUBES_START && (!fadingCubes.empty() || !solidCubes.empty() || !frameCubes.empty())) {
			particleShader.use();
			particleShader.setView(getView());

			drawCubes(particleShader, fadingCubes, Mode::FADING, models::blackCube, 1.0f);
			drawCubes(particleShader, solidCubes,  Mode::SOLID,  models::blackCube, 0.9f);
			drawCubes(particleShader, frameCubes,  Mode::SOLID,  models::cubeFrame, 0.8f);
		}
	}

	void EnemyDestroyAnimation::setFlatShaderUniforms() const {
		flatShader.setUniform("seed", seed);
	}
}