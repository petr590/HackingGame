#include "menu_bottom_panel.h"
#include "menu.h"

namespace hack_game {

	static constexpr ImVec2 PADDING = {55, 20};
	static constexpr ImVec2 MARGIN  = {65, 90};


	void MenuBottomPanel::draw(const GuiContext& context) {
		const char* const text = "Выберите уровень.";
		const ImVec2 textSize = ImGui::CalcTextSize(text);

		const ImVec2 panelStart = {
				MARGIN.x,
				NORMAL_WINDOW_SIZE.y - textSize.y - MARGIN.y - PADDING.y * 2
		};

		const ImVec2 panelEnd = NORMAL_WINDOW_SIZE - MARGIN;

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		constexpr float OFFSET = Menu::SHADOW_OFFSET;
		drawList->AddRectFilled(context.scaleVec(panelStart + OFFSET), context.scaleVec(panelEnd + OFFSET), Menu::SHADOW_COLOR);
		drawList->AddRectFilled(context.scaleVec(panelStart),          context.scaleVec(panelEnd),          Menu::LIGHT_COLOR);

		drawList->AddRectFilled(
			context.scaleVec(panelStart.x + Menu::STRIPE1_START, panelStart.y),
			context.scaleVec(panelStart.x + Menu::STRIPE1_END,   panelEnd.y),
			Menu::DARK_COLOR
		);

		drawList->AddRectFilled(
			context.scaleVec(panelStart.x + Menu::STRIPE2_START, panelStart.y),
			context.scaleVec(panelStart.x + Menu::STRIPE2_END,   panelEnd.y),
			Menu::DARK_COLOR
		);

		ImGui::SetCursorPos(context.scaleVec(panelStart + PADDING));
		ImGui::Text("%s", text);
	}
}