#ifndef HACK_GAME__ENTITY__ANIMATION__PLAYER_DESTROY_H
#define HACK_GAME__ENTITY__ANIMATION__PLAYER_DESTROY_H

#include "flat_and_billboard_animation.h"

namespace hack_game {

	class PlayerDestroyAnimation: public FlatAndBillboardAnimation {
	public:
		class Cube;

	private:
		std::vector<Cube> cubes;
		glm::vec3 angleNormal;
		const GLint seed;

	public:
		PlayerDestroyAnimation(std::shared_ptr<const EntityWithPos>&&, ShaderManager&);
		~PlayerDestroyAnimation();

		void tick(Level&) override;

	protected:
		void onRemove(Level&) override;
		glm::mat4 getFlatShaderModelTransform() const override;
		void setFlatShaderUniforms() const override;
		void setBillboardShaderUniforms() const override;
	};
}

#endif