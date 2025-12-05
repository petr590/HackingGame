#include "imgui_util.h"

namespace hack_game {

	static ImVec2 globalScale = {0, 0};

	void setGlobalScale(const ImVec2& newGlobalScale) {
		globalScale = newGlobalScale;
	}

	void addRectFilledScaled(const ImVec2& pMin, const ImVec2& pMax, ImU32 color, float rounding, ImDrawFlags flags) {
		ImGui::GetWindowDrawList()->AddRectFilled(
				pMin * globalScale,
				pMax * globalScale,
				color, rounding, flags
		);
	}


	static bool enterPressed = false;

	bool isEnterPressedOnce() {
		if (!enterPressed && ImGui::IsKeyPressed(ImGuiKey_Enter)) {
			enterPressed = true;
			return true;
		}

		return false;
	}

	void imGuiUtilUpdate() {
		enterPressed = false;
	}
}