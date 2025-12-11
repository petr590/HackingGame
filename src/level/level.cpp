#include "level.h"
#include "invalid_level_exception.h"
#include "shader/shader_manager.h"
#include "model/models.h"
#include "entity/block.h"
#include "entity/player.h"
#include "entity/enemy.h"
#include "entity/minion.h"
#include "entity/platform.h"
#include "entity/walls.h"

// #include <boost/format.hpp>
#include <fstream>
#include <nlohmann/json.hpp>

namespace hack_game {
	using std::string;
	using std::vector;
	using std::shared_ptr;
	using std::make_shared;
	using std::ifstream;
	using std::clamp;
	using std::move;
	using std::find;
	using std::dynamic_pointer_cast;

	using glm::uvec2;
	using glm::vec2;
	using glm::vec3;

	using nlohmann::json;


	namespace boost {
		class format {
		public:
			format(const char*) {}

			template<class T>
			format& operator%(const T&) {
				return *this;
			}

			string str() {
				return "";
			}
		};
	}


	Map::Map(size_t width, size_t height):
			data(width, vector<ptr_t>(height, nullptr)) {}
	
	void Map::allocate(size_t width, size_t height) {
		data = vector<vector<ptr_t>>(width, vector<ptr_t>(height, nullptr));
	}


	// ------------------------------------------- read -------------------------------------------

	Level::Level(ShaderManager& shaderManager, const string& path) {
		json object;

		{
			ifstream file(path);

			if (!file.is_open()) {
				throw std::ios_base::failure("Cannot open file '" + path + "'");
			}

			file >> object;
		}

		readMap(shaderManager, path, object);
		readEntities(shaderManager, path, object);
		damageableEnemyEntities.push_back(enemy);
	}


	static shared_ptr<Block> readBlock(ShaderManager& shaderManager, size_t x, size_t y, char ch) {
		switch (ch) {
			case ' ': return nullptr;
			case 'B': return Block::breakable(shaderManager.mainShader, uvec2(x, y));
			case 'U': return Block::unbreakable(shaderManager.mainShader, uvec2(x, y));

			default:
				throw InvalidLevelException((boost::format("Expected ' ', 'B' or 'U', found '%c'") % ch).str());
		}
	}


	void Level::readMap(ShaderManager& shaderManager, const string& path, const json& object) {
		const size_t width = object["width"];
		const size_t height = object["height"];
		map.allocate(width, height);

		const vector<string>& mapData = object["map"];

		if (mapData.size() != height) {
			throw InvalidLevelException((boost::format("%1%: map size (%2%) != height (%3%)") % path % mapData.size() % height).str());
		}
		
		for (size_t y = 0; y < height; y++) {
			const string& str = mapData[y];

			if (str.size() != width) {
				throw InvalidLevelException((boost::format("%1%: string size (%2%) != width (%3%)") % path % str.size() % width).str());
			}

			for (size_t x = 0; x < width; x++) {
				auto block = readBlock(shaderManager, x, y, str[x]);

				if (block != nullptr) {
					map[x][y] = block;
					addEntityDirect(move(block));
				}
			}
		}

		const bool infinityPlatform = object["infinityPlatform"];
		addEntityDirect(make_shared<Platform>(shaderManager.mainShader, vec2(map.width(), map.height()) * TILE_SIZE, infinityPlatform));

		if (infinityPlatform) {
			addEntityDirect(make_shared<Walls>(shaderManager.mainShader));
		}
	}


	static vec3 readPos(const json& obj) {
		if (!obj.contains("pos")) return vec3(0.0f);

		const json& pos = obj.at("pos");
		return vec3(
			pos.value("x", 0.0f),
			pos.value("y", 0.0f),
			pos.value("z", 0.0f)
		) * TILE_SIZE;
	}


	static shared_ptr<Entity> readEntity(ShaderManager& shaderManager, const string& path, const json& obj) {
		const string& type = obj["type"];

		if (type == "Player") {
			return make_shared<Player>(
				shaderManager,
				Camera(
					vec3(0.0f, 0.75f, 0.35f),
					vec3(0.0f, 0.0f, -0.05f)
				),
				obj.value("speed", 0.25f),
				readPos(obj)
			);
		}

		if (type == "Enemy1") {
			return make_shared<Enemy1>(shaderManager, readPos(obj));
		}

		if (type == "Minion") {
			return make_shared<Minion>(shaderManager, readPos(obj));
		}

		throw InvalidLevelException((boost::format("%s: unknown type: \"%s\"") % path % type).str());
	}


	void Level::readEntities(ShaderManager& shaderManager, const string& path, const json& object) {
		
		for (const json& obj : object["entities"]) {
			shared_ptr<Entity> entity = readEntity(shaderManager, path, obj);

			if (auto p = dynamic_pointer_cast<Player>(entity)) {
				if (player != nullptr) {
					throw InvalidLevelException((boost::format("%s: more than one Player entity") % path).str());
				}
				
				player = move(p);

			} else if (auto e = dynamic_pointer_cast<Enemy>(entity)) {
				if (enemy != nullptr) {
					throw InvalidLevelException((boost::format("%s: more than one Enemy entity") % path).str());
				}

				enemy = move(e);
			}

			addEntityDirect(move(entity));
		}
	}


	// -------------------------------------- pos, entities ---------------------------------------

	uvec2 Level::getMapPos(const vec2& pos) const noexcept {
		return uvec2(
			clamp(pos.x * (1.0f / TILE_SIZE), 0.0f, float(map.width() - 1)),
			clamp(pos.y * (1.0f / TILE_SIZE), 0.0f, float(map.height() - 1))
		);
	}


	static void addDamageable(const shared_ptr<Entity>& entity, vector<shared_ptr<Damageable>>& damageableEnemyEntities) {
		auto damageable = dynamic_pointer_cast<Damageable>(entity);

		if (damageable != nullptr && damageable->getSide() == Side::ENEMY && !damageable->invulnerable()) {
			damageableEnemyEntities.push_back(move(damageable));
		}
	}


	void Level::addEntityDirect(std::shared_ptr<Entity>&& entity) {
		addDamageable(entity, damageableEnemyEntities);
		getVector(entity).push_back(move(entity));
	}


	void Level::addEntity(const shared_ptr<Entity>& entity) {
		addedEntities.push_back(entity);
		addDamageable(entity, damageableEnemyEntities);
	}


	void Level::removeEntity(const shared_ptr<Entity>& entity) {
		removedEntities.push_back(entity);

		auto damageable = dynamic_pointer_cast<Damageable>(entity);

		if (damageable != nullptr && damageable->getSide() == Side::ENEMY) {
			auto it = find(damageableEnemyEntities.begin(), damageableEnemyEntities.end(), damageable);

			if (it != damageableEnemyEntities.end()) {
				damageableEnemyEntities.erase(it);
			}
		}
	}


	Level::EntityVector& Level::getVector(const shared_ptr<Entity>& entity) noexcept {
		EntityMap& map = entity->isTransparent() ? transparentEntityMap : opaqueEntityMap;
		return map[entity->getShaderProgram()];
	}

	void Level::updateEntities() {
		if (!removedEntities.empty()) {
			for (const auto& entity : removedEntities) {
				EntityVector& vector = getVector(entity);

				const auto it = find(vector.cbegin(), vector.cend(), entity);
				if (it != vector.cend()) {
					vector.erase(it);
				}
			}

			removedEntities.clear();
		}

		if (!addedEntities.empty()) {
			for (auto& entity : addedEntities) {
				getVector(entity).push_back(move(entity));
			}

			addedEntities.clear();
		}
	}
}
