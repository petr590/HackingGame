#include "system_message_popup.h"
#include "menu.h"
#include "imgui_util.h"

namespace hack_game {

	using Selectable = SystemMessagePopup::Selectable;
	using callback_t = SystemMessagePopup::callback_t;

	using std::min;
	using std::max;


	static constexpr const char* POPUP_NAME = "SystemMessagePopup";
	static constexpr const char* TITLE      = "С И С Т Е М Н О Е   С О О Б Щ Е Н И Е";
	static constexpr const char* MESSAGE    = "Начать игру?";
	static constexpr const char* YES_LABEL  = "\t  Да";
	static constexpr const char* NO_LABEL   = "\t  Нет";

	static constexpr ImVec4 TRANSPARENT = {0, 0, 0, 0};

	static constexpr ImVec2 WINDOW_SIZE    = {781, 356};
	static constexpr ImVec2 TITLE_BAR_SIZE = {WINDOW_SIZE.x, 48};

	static constexpr ImVec2 MESSAGE_OFFSET = {20, 16};

	static constexpr float SELECTABLE_PADDING_XY = 13;
	static constexpr ImVec2 SELECTABLE_SIZE = {180, 48};

	static constexpr ImVec2 SQUARE_SIZE     = {24, 24};
	static constexpr ImVec2 TITLE_BAR_SQUARE_OFFSET  = {18, 0.5f * (TITLE_BAR_SIZE.y - SQUARE_SIZE.y)};
	static constexpr ImVec2 SELECTABLE_SQUARE_OFFSET = 0.5f * (ImVec2(SELECTABLE_SIZE.y, SELECTABLE_SIZE.y) - SQUARE_SIZE);

	static constexpr ImVec2 LINE_OFFSET = {12, 240};
	static constexpr float LINE_THICKNESS = 2;

	static constexpr float POINTER_TEXTURE_OFFSET_X = 18;

	static constexpr float POPUP_FADE_TIME = 0.15f;
	static constexpr float SELECT_FADE_TIME = 0.2f;


	SystemMessagePopup::SystemMessagePopup() noexcept:
			FadingObject(POPUP_FADE_TIME),
			yes(false), no(true), yesSelected(false) {}


	void SystemMessagePopup::open(callback_t callback) {
		this->callback = std::move(callback);
	}

	void SystemMessagePopup::close() {
		callback = nullptr;
	}


	static void addRectFilledAt(const ImVec2& pos, const ImVec2& size, ImU32 color) {
		const ImVec2 start = ImGui::GetWindowPos() + pos;
		ImGui::GetWindowDrawList()->AddRectFilled(start, start + size, ImGui::GetColorU32(color));
	}

	static void drawTitleBar(const GuiContext& context);
	static bool drawButtons(const GuiContext& context, Selectable& yes, Selectable& no, bool yesSelected, const callback_t& selectedYes, const callback_t& selectedNo);


	void SystemMessagePopup::draw(const GuiContext& context) {
		if (callback == nullptr && getFadeTime() == 0) {
			return;
		}

		updateFadeTime(context.getDeltaTime(), callback != nullptr);

		const float alpha = getFadeProgress();
		const ImVec2 windowSize = context.scaleVec(WINDOW_SIZE);
		const ImVec2 windowPos = 0.5f * (ImGui::GetWindowSize() - windowSize);

		ImGui::SetNextWindowPos(windowPos);
		ImGui::SetNextWindowSize(windowSize);
		ImGui::PushStyleColor(ImGuiCol_PopupBg, Menu::LIGHT_COLOR);
		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, hack_game::alpha(colorAsImVec4(0x3F'000000), alpha));
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

		ImGui::OpenPopup(POPUP_NAME);

		if (ImGui::BeginPopupModal(POPUP_NAME, nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
			if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {
				yesSelected = true;

			} else if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {
				yesSelected = false;
			}

			drawTitleBar(context);

			ImGui::SetCursorPos(ImGui::GetCursorPos() + context.scaleVec(MESSAGE_OFFSET));
			ImGui::Text("%s", MESSAGE);

			addRectFilledAt(
				context.scaleVec(LINE_OFFSET),
				context.scaleVec(WINDOW_SIZE.x - 2 * LINE_OFFSET.x, LINE_THICKNESS),
				Menu::SHADOW_COLOR
			);

			yesSelected = drawButtons(
					context, yes, no, yesSelected,
					[this] () { callback(); close(); },
					[this] () { close(); }
			);

			if (callback != nullptr && isEnterPressedOnce()) {
				if (yesSelected) {
					callback();
				}

				close();
			}

			if (callback == nullptr && getFadeTime() == 0) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleColor(2);
	}


	static void drawTitleBar(const GuiContext& context) {
		const ImVec2 squarePos = context.scaleVec(TITLE_BAR_SQUARE_OFFSET);

		addRectFilledAt({0, 0}, context.scaleVec(TITLE_BAR_SIZE), Menu::DARK_COLOR);
		addRectFilledAt(squarePos, context.scaleVec(SQUARE_SIZE), Menu::LIGHT_COLOR);

		const ImVec2 textSize = ImGui::CalcTextSize(TITLE);

		ImGui::SetCursorPos(ImVec2(
			context.scaleX(2.0f * TITLE_BAR_SQUARE_OFFSET.x + SQUARE_SIZE.x),
			0.5f * (context.scaleY(TITLE_BAR_SIZE.y) - textSize.y)
		));
		
		ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(Menu::LIGHT_COLOR), "%s", TITLE);
	}


	static bool drawButtons(const GuiContext& context, Selectable& yes, Selectable& no, bool yesSelected, const callback_t& selectedYes, const callback_t& selectedNo) {
		const float paddingX = (1.0f / 3.0f) * (WINDOW_SIZE.x - SELECTABLE_SIZE.x * 2);
		const float startY = 0.5f * (WINDOW_SIZE.y - LINE_OFFSET.y - SELECTABLE_SIZE.y);

		const ImVec2 pos1(paddingX,                     LINE_OFFSET.y + startY);
		const ImVec2 pos2(paddingX * 2 + SELECTABLE_SIZE.x, LINE_OFFSET.y + startY);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, context.scaleY(0, SELECTABLE_PADDING_XY));
		ImGui::PushStyleColor(ImGuiCol_Header,        TRANSPARENT);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, TRANSPARENT);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive,  TRANSPARENT);
		ImGui::PushStyleColor(ImGuiCol_NavHighlight,  TRANSPARENT);
		
		const bool isYes = yes.draw(context, YES_LABEL, context.scaleVec(pos1), yesSelected, selectedYes);
		const bool isNo = no.draw(context, NO_LABEL, context.scaleVec(pos2), !yesSelected, selectedNo);

		ImGui::PopStyleColor(4);
		ImGui::PopStyleVar();

		return isYes ? true : isNo ? false : yesSelected;
	}



	SystemMessagePopup::Selectable::Selectable(bool isSelected) noexcept:
			FadingObject(SELECT_FADE_TIME, isSelected) {}
	

	static void drawPointerImage(const GuiContext& context, const ImVec2& pos, float progress) {
		const ImVec2 size = context.scaleVec(Menu::POINTER_TEXTURE_SIZE);
		const ImVec4 tintColor = {1, 1, 1, progress};

		ImGui::SetCursorPos(pos - ImVec2(size.x + POINTER_TEXTURE_OFFSET_X, 0));
		ImGui::Image(context.getPointerTextureId(), size, {0, 0}, {1, 1}, tintColor, TRANSPARENT);
	}

	
	bool SystemMessagePopup::Selectable::draw(const GuiContext& context, const char* label, const ImVec2& pos, bool isSelected, const callback_t& callback) {
		updateFadeTime(context.getDeltaTime(), isSelected);

		const ImVec2 size = context.scaleVec(SELECTABLE_SIZE);
		const ImVec2 squareSize = context.scaleVec(SQUARE_SIZE);
		const ImVec2 squarePos = pos + context.scaleVec(SELECTABLE_SQUARE_OFFSET);

		const float progress = getFadeProgress();
		
		drawPointerImage(context, pos, progress);

		const ImU32 selectableColor = mixImU32(Menu::DARKER_COLOR, Menu::DARK_COLOR, progress);
		const ImU32 textColor       = mixImU32(ImGui::GetStyleColorVec4(ImGuiCol_Text), Menu::LIGHT_COLOR, progress);
		const ImU32 shadowColor     = alphaImU32(Menu::SHADOW_COLOR, progress);

		addRectFilledAt(pos + Menu::SHADOW_OFFSET, size, shadowColor);
		addRectFilledAt(pos, size, selectableColor);
		addRectFilledAt(squarePos, squareSize, textColor);

		ImGui::PushStyleColor(ImGuiCol_Text, textColor);
		ImGui::SetCursorPos(pos + context.scaleY(0, SELECTABLE_PADDING_XY * 0.5f));
		const bool clicked = ImGui::Selectable(label, isSelected, 0, {size.x, 0});
		ImGui::PopStyleColor();

		if (clicked) {
			callback();
		}

		return clicked || ImGui::IsItemHovered();
	}
}