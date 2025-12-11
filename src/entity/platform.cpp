#include "platform.h"
#include "model/models.h"
#include <glm/gtc/matrix_transform.hpp>

namespace hack_game {

	using glm::vec2;
	using glm::vec3;
	using glm::mat4;

	static constexpr float INFINITE_SCALE = 5.0f;

	Platform::Platform(Shader& shader, const glm::vec2& scale, bool isInfinite) noexcept:
			SimpleEntity(shader, models::platform),
			scale(scale),
			isInfinite(isInfinite) {}
	
	mat4 Platform::getModelTransform() const {
		if (isInfinite) {
			const vec2 offset = (scale - INFINITE_SCALE) * 0.5f;

			mat4 model(1.0f);
			model = glm::translate(model, vec3(offset.x, 0.0f, offset.y));
			return  glm::scale(model, vec3(INFINITE_SCALE));
		}

		return glm::scale(mat4(1.0f), vec3(scale.x, 0.0f, scale.y));
	}
}