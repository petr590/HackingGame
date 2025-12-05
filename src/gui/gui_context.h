#ifndef HACK_GAME__GUI__GUI_CONTEXT_H
#define HACK_GAME__GUI__GUI_CONTEXT_H

#include "imgui_util.h"
#include "gl_fwd.h"

namespace hack_game {

	class GuiContext {
		ImVec2 scale = {1, 1};
		float deltaTime = 0;
		const GLuint pointerTextureId;
	
	public:
		GuiContext();

		const ImVec2& getScale() const noexcept {
			return scale;
		}

		float getDeltaTime() const noexcept {
			return deltaTime;
		}

		GLuint getPointerTextureId() const noexcept {
			return pointerTextureId;
		}


		void setScale(const ImVec2& scale) noexcept {
			this->scale = scale;
		}

		void setDeltaTime(float deltaTime) noexcept {
			this->deltaTime = deltaTime;
		}


		float scaleX(float x) const noexcept {
			return x * scale.x;
		}

		float scaleY(float y) const noexcept {
			return y * scale.y;
		}

		ImVec2 scaleX(float x, float y) const noexcept {
			return ImVec2(x * scale.x, y);
		}

		ImVec2 scaleY(float x, float y) const noexcept {
			return ImVec2(x, y * scale.y);
		}

		ImVec2 scaleVec(const ImVec2& vec) const noexcept {
			return vec * scale;
		}

		ImVec2 scaleVec(float x, float y) const noexcept {
			return ImVec2(x, y) * scale;
		}
	};
}

#endif