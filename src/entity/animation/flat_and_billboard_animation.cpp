#include "flat_and_billboard_animation.h"
#include "shader/shader_manager.h"

namespace hack_game {

	using glm::vec3;
	using glm::mat4;

	FlatAndBillboardAnimation::FlatAndBillboardAnimation(
				std::shared_ptr<const EntityWithPos>&& entity, ShaderManager& shaderManager, Shader& flatShader, Shader& billboardShader,
				float duration, float size, float yOffset, Model& model
	):
			BillboardAnimation(std::move(entity), shaderManager.nullShader, duration, size, yOffset, model),
			flatShader(flatShader),
			billboardShader(billboardShader) {}
	

	void FlatAndBillboardAnimation::draw() const {
		const vec3 pos = getPos();
		const float progress = getProgress();

		flatShader.use();
		flatShader.setView(getView());
		flatShader.setModel(getFlatShaderModelTransform());
		flatShader.setUniform("centerPos", pos);
		flatShader.setUniform("progress", progress);
		setFlatShaderUniforms();
		model.draw(flatShader);

		billboardShader.use();
		billboardShader.setView(getView());
		billboardShader.setModel(getBillboardShaderModelTransform());
		billboardShader.setUniform("centerPos", pos);
		billboardShader.setUniform("progress", progress);
		setBillboardShaderUniforms();
		model.draw(billboardShader);
	}

	mat4 FlatAndBillboardAnimation::getFlatShaderModelTransform() const {
		return Animation::getModelTransform();
	}

	mat4 FlatAndBillboardAnimation::getBillboardShaderModelTransform() const {
		return BillboardAnimation::getModelTransform();
	}

}