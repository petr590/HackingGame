#ifndef HACK_GAME__ENTITY__ANIMATION__MINION_DESTROY_H
#define HACK_GAME__ENTITY__ANIMATION__MINION_DESTROY_H

#include "flat_and_billboard_animation.h"
#include <vector>

namespace hack_game {

	class MinionDestroyAnimation: public FlatAndBillboardAnimation {
	public:
		class Cube;
	
	private:
		std::vector<std::shared_ptr<Cube>> cubes;
		Shader& particleShader;
		glm::vec3 angleNormal;
		int32_t seed;

	public:
		MinionDestroyAnimation(std::shared_ptr<const EntityWithPos>&&, Level&, ShaderManager&) noexcept;
		~MinionDestroyAnimation();

		void tick(Level&) override;
	
	protected:
		glm::mat4 getFlatShaderModelTransform() const override;
		void setBillboardShaderUniforms() const override;
		void onRemove(Level&) override;
	};
}

#endif