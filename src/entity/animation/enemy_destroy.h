#ifndef HACK_GAME__ENTITY__ANIMATION__ENEMY_DESTROY_H
#define HACK_GAME__ENTITY__ANIMATION__ENEMY_DESTROY_H

#include "flat_and_billboard_animation.h"
#include <vector>

namespace hack_game {

	class EnemyDestroyAnimation: public FlatAndBillboardAnimation {
	public:
		class Cube;

	private:
		Shader& particleShader;

		std::vector<Cube> fadingCubes;
		std::vector<Cube> solidCubes;
		std::vector<Cube> frameCubes;
		
		const GLint seed;

	public:
		EnemyDestroyAnimation(std::shared_ptr<const EntityWithPos>&&, ShaderManager&) noexcept;
		~EnemyDestroyAnimation();

		void tick(Level&) override;
		void draw() const override;
	
	protected:
		void onRemove(Level&) override;
		void setFlatShaderUniforms() const override;
	};
}

#endif