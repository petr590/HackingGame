#include "win_screen.h"
#include "imgui_util.h"
#include <cmath>
#include <cstdio>
#include <glm/trigonometric.hpp>

namespace hack_game {

	static constexpr ImU32 TEXT_COLOR = colorAsImU32(0xFF'D0CBC1);

	static constexpr float TEXT_APPEAR_START    = 0.25f;
	static constexpr float TEXT_APPEAR_DURATION = 0.25f;

	static constexpr float LINE_THICKNESS = 3.0f;
	static constexpr float LINE_MARGIN    = 12.0f;
	static constexpr float CIRCLE_RADIUS  = 3.5f;
	static constexpr float TRIANGLE_CIRCLES_OFFSET = 20.0f;

	static constexpr const char* MESSAGE = "В З Л О М   З А В Е Р Ш Ё Н"; // H A C K I N G   C O M P L E T E

	static void drawDecorations(const GuiContext& guiContext, const ImVec2& textStart, const ImVec2& textSize) {
		const ImVec2 lineStart = {
			textStart.x,
			textStart.y + textSize.y
		};

		const ImVec2 lineEnd = {
			lineStart.x + textSize.x,
			lineStart.y
		};

		const ImVec2 margin = guiContext.scaleX(LINE_MARGIN, 0);

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddLine(lineStart + margin, lineEnd - margin, TEXT_COLOR, LINE_THICKNESS);

		const float radius = CIRCLE_RADIUS * std::min(guiContext.getScale().x, guiContext.getScale().y);
		
		drawList->AddCircleFilled(lineStart, radius, TEXT_COLOR);
		drawList->AddCircleFilled(lineEnd, radius, TEXT_COLOR);

		const ImVec2 pos = {
			0.5f * (lineStart.x + lineEnd.x),
			lineStart.y + TRIANGLE_CIRCLES_OFFSET
		};

		drawList->AddCircleFilled(pos - guiContext.scaleX(0.5f * TRIANGLE_CIRCLES_OFFSET, 0), radius, TEXT_COLOR);
		drawList->AddCircleFilled(pos + guiContext.scaleX(0.5f * TRIANGLE_CIRCLES_OFFSET, 0), radius, TEXT_COLOR);
		drawList->AddCircleFilled(pos + guiContext.scaleY(0, TRIANGLE_CIRCLES_OFFSET * std::cos(glm::radians(30.0f))), radius, TEXT_COLOR);
	}

	void WinScreen::draw(const GuiContext& guiContext, float winScreenTime) {
		if (winScreenTime <= TEXT_APPEAR_START) {
			return;
		}

		const ImVec2 windowSize = ImGui::GetIO().DisplaySize;

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(windowSize);
		
		ImGui::Begin("WinScreen", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowFontScale(2.25f);

		const ImVec2 textSize = ImGui::CalcTextSize(MESSAGE);

		const float progress = std::min(1.0f, (winScreenTime - TEXT_APPEAR_START) * (1.0f / TEXT_APPEAR_DURATION));
		const float yOffset = std::pow(1.0f - progress, 2.0f) * textSize.y * 0.25f;

		const ImVec2 textStart = {
			(windowSize.x - textSize.x) * 0.5f,
			(windowSize.y - textSize.y) * 0.5f + yOffset
		};

		ImGui::SetCursorPos(textStart);
		ImGui::TextColored(alpha(ImGui::ColorConvertU32ToFloat4(TEXT_COLOR), progress), "%s", MESSAGE);

		drawDecorations(guiContext, textStart, textSize);
		
		ImGui::End();
	}
}