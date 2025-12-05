#include "menu_select.h"
#include "imgui_util.h"
#include "menu.h"
#include "texture.h"
#include "dir_paths.h"
#include <string>
#include <cmath>

namespace hack_game {

	using std::min;
	using std::clamp;
	using std::exp;
	using std::string;
	using std::to_string;

	using Selectable = MenuSelect::Selectable;


	static constexpr ImVec4 TRANSPARENT = {0, 0, 0, 0};

	static constexpr ImVec2 MARGIN = {65, 220};
	static constexpr ImVec2 SIZE   = {575, 575};

	static constexpr ImVec2 BAR_SIZE = {65, SIZE.y};
	static constexpr ImU32 BAR_COLOR        = colorAsImU32(0xFF'AEAA92);
	static constexpr ImU32 BAR_STRIPE_COLOR = colorAsImU32(0xFF'908B78);

	static constexpr float POINTER_TEXTURE_OFFSET_X = 4;

	static constexpr ImVec2 PANEL_START = MARGIN;
	static constexpr ImVec2 PANEL_END = MARGIN + SIZE;


	static constexpr float SCROLLBAR_WIDTH     = 6;
	static constexpr float SCROLLBAR_PADDING_X = 21;

	static constexpr ImVec2 TOP_LEFT_PADDING = {0, 20};
	static constexpr ImVec2 BOTTOM_RIGHT_PADDING = {SCROLLBAR_PADDING_X * 2 + SCROLLBAR_WIDTH, 21};

	static constexpr float STRIPE_HEIGHT           = 2;
	static constexpr float STRIPE_OFFSET_X         = 10;
	static constexpr float SELECTABLE_MARGIN_Y     = 6;
	static constexpr float SELECTABLE_PADDING_XY   = 5;
	static constexpr ImVec2 SQUARE_SIZE = {24, 24};

	static constexpr float SELECT_APPEAR_TIME = 0.45f;
	static constexpr float SELECT_FADE_TIME   = 0.15f;


	class MenuSelect::Selectable {
		const string label;
		const string path;
		float time;
	
	public:
		Selectable(size_t num, bool isSelected):
				label("\t  Мини-игра по взлому #" + to_string(num)),
				path(LEVELS_DIR "level" + to_string(num) + ".json"),
				time(isSelected ? SELECT_APPEAR_TIME : 0.0f) {}
		
		const string& getLabel() const noexcept { return label; }
		const string& getPath() const noexcept { return path; }
		float getTime() const noexcept { return time; }

		bool draw(MenuSelect&, const GuiContext&, const ImVec2& region, bool isSelected);
		void drawAnimatedSelectable(MenuSelect&, const GuiContext&, const ImVec2& region, bool isSelected);
		bool drawPlainSelectable(MenuSelect&, const GuiContext&, bool isSelected);
	};


	MenuSelect::MenuSelect(Menu& menu, size_t count) noexcept: menu(menu) {
		selectables.reserve(count);

		for (size_t i = 0; i < count; i++) {
			selectables.emplace_back(i + 1, i == selected);
		}
	}

	MenuSelect::~MenuSelect() {}


	void MenuSelect::draw(const GuiContext& context) {

		constexpr float OFFSET = Menu::SHADOW_OFFSET;
		addRectFilledScaled(PANEL_START + OFFSET, PANEL_END + OFFSET, Menu::SHADOW_COLOR);
		addRectFilledScaled(PANEL_START,          PANEL_END,          Menu::LIGHT_COLOR);

		drawContent(context);
		popup.draw(context);
	}

	static void drawScrollbar(const GuiContext&, float regionY, float scrollY, float maxScrollY);
	static void drawDecorations();

	void MenuSelect::drawContent(const GuiContext& context) {
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, context.scaleY(0, SELECTABLE_PADDING_XY));
		ImGui::PushStyleColor(ImGuiCol_Header,        TRANSPARENT);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, TRANSPARENT);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive,  TRANSPARENT);
		ImGui::PushStyleColor(ImGuiCol_NavHighlight,  TRANSPARENT);

		constexpr ImVec2 selectStart = PANEL_START + TOP_LEFT_PADDING;
		constexpr ImVec2 selectSize = PANEL_END - BOTTOM_RIGHT_PADDING - selectStart;

		ImGui::SetCursorPos(context.scaleVec(selectStart));
		ImGui::BeginChild("MenuSelectContent", context.scaleVec(selectSize), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);

		ImGui::Indent(context.scaleX(BAR_SIZE.x));
		ImGui::SetItemAllowOverlap(); // Отключить рамку при фокусе на Child

		const size_t count = selectables.size();

		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow)) {
			if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))   selected = (selected + count - 1) % count;
			if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) selected = (selected + 1) % count;
		}

		const ImVec2 region = ImGui::GetContentRegionAvail();

		for (size_t i = 0; i < count; i++) {
			bool isSelected = selectables[i].draw(*this, context, region, i == selected);

			if (isSelected) {
				selected = i;
			}
		}

		// Необходимо получить scrollY и maxScrollY до вызова EndChild.
		// Но скроллбар отрисовывается только после вызова EndChild, так как он за пределами текущего Child.
		const float scrollY = ImGui::GetScrollY();
		const float maxScrollY = ImGui::GetScrollMaxY();

		ImGui::Unindent(context.scaleX(BAR_SIZE.x));
		ImGui::EndChild();
		ImGui::PopStyleColor(4);
		ImGui::PopStyleVar();

		drawScrollbar(context, region.y, scrollY, maxScrollY);
		drawDecorations();
	}


	static void drawScrollbar(const GuiContext& context, float regionY, float scrollY, float maxScrollY) {
		const float scrollbarHeight = regionY * regionY / maxScrollY;
		const float scrollbarY = (regionY - scrollbarHeight) * scrollY / maxScrollY;

		const ImVec2 start = {
				context.scaleX(PANEL_END.x - BOTTOM_RIGHT_PADDING.x + SCROLLBAR_PADDING_X),
				context.scaleY(PANEL_START.y + TOP_LEFT_PADDING.y) + scrollbarY
		};

		ImGui::GetWindowDrawList()->AddRectFilled(
				start,
				start + context.scaleX(SCROLLBAR_WIDTH, scrollbarHeight),
				Menu::DARK_COLOR
		);
	}


	static void drawDecorations() {
		constexpr ImVec2 stripe1Start = PANEL_START + TOP_LEFT_PADDING * ImVec2(1.0f, 0.5f) + ImVec2(STRIPE_OFFSET_X, -0.5f * STRIPE_HEIGHT);
		constexpr ImVec2 stripe1Size = {PANEL_END.x - PANEL_START.x - TOP_LEFT_PADDING.x - BOTTOM_RIGHT_PADDING.x - STRIPE_OFFSET_X * 2, STRIPE_HEIGHT};
		constexpr ImVec2 stripe2Offset = {0, PANEL_END.y - PANEL_START.y - TOP_LEFT_PADDING.y};
		constexpr ImVec2 square1Start = {
				PANEL_END.x - BOTTOM_RIGHT_PADDING.x + SCROLLBAR_PADDING_X,
				stripe1Start.y - 0.5f * (SCROLLBAR_WIDTH - STRIPE_HEIGHT)
		};

		addRectFilledScaled( // stripe 1
				stripe1Start,
				stripe1Start + stripe1Size,
				BAR_STRIPE_COLOR
		);

		addRectFilledScaled( // stripe 2
				stripe1Start + stripe2Offset,
				stripe1Start + stripe2Offset + stripe1Size,
				BAR_STRIPE_COLOR
		);

		addRectFilledScaled( // square 1
				square1Start,
				square1Start + SCROLLBAR_WIDTH,
				BAR_STRIPE_COLOR
		);

		addRectFilledScaled( // square 2
				square1Start + stripe2Offset,
				square1Start + stripe2Offset + SCROLLBAR_WIDTH,
				BAR_STRIPE_COLOR
		);


		addRectFilledScaled(PANEL_START, PANEL_START + BAR_SIZE, BAR_COLOR);

		addRectFilledScaled(
			ImVec2(PANEL_START.x + Menu::STRIPE1_START, PANEL_START.y),
			ImVec2(PANEL_START.x + Menu::STRIPE1_END,   PANEL_END.y),
			BAR_STRIPE_COLOR
		);

		addRectFilledScaled(
			ImVec2(PANEL_START.x + Menu::STRIPE2_START, PANEL_START.y),
			ImVec2(PANEL_START.x + Menu::STRIPE2_END,   PANEL_END.y),
			BAR_STRIPE_COLOR
		);
	}


	// ------------------------------- MenuSelect::Selectable::draw -------------------------------

	bool MenuSelect::Selectable::draw(MenuSelect& parent, const GuiContext& context, const ImVec2& region, bool isSelected) {
		if (isSelected) {
			time = min(time + context.getDeltaTime(), SELECT_APPEAR_TIME);
		} else {
			time = clamp(time - context.getDeltaTime(), 0.0f, SELECT_FADE_TIME);
		}

		if (time > 0) {
			drawAnimatedSelectable(parent, context, region, isSelected);
			return false;
		}

		return drawPlainSelectable(parent, context, isSelected);
	}



	static ImVec2 getRectSize(const GuiContext& context, float progress, float fadeProgress, float textHeight, bool isSelected) {
		float maxWidth = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		float xPercent = !isSelected ? fadeProgress :
				(1.0f - exp(-6.0f * progress)) * (1.0f / (1.0f - exp(-6.0f)));

		return {
				xPercent * maxWidth,
				textHeight + context.scaleY(SELECTABLE_PADDING_XY * 2)
		};
	}


	static void drawPointerImage(const GuiContext& context, float fadeProgress) {
		ImVec2 pos = ImGui::GetCursorPos();
		ImVec4 tintColor = {1, 1, 1, fadeProgress};

		ImGui::SetCursorPosX(pos.x - context.scaleX(BAR_SIZE.x - POINTER_TEXTURE_OFFSET_X));
		ImGui::Image(context.getPointerTextureId(), context.scaleVec(Menu::POINTER_TEXTURE_SIZE), {0, 0}, {1, 1}, tintColor, TRANSPARENT);
		ImGui::SetCursorPos(pos + context.scaleY(0, SELECTABLE_PADDING_XY));
	}

	static void addRectFilledAtCursor(const ImVec2& size, ImU32 color, const ImVec2& offset = {0, 0}) {
		ImVec2 pos = ImGui::GetCursorScreenPos() + offset;
		ImGui::GetWindowDrawList()->AddRectFilled(pos, pos + size, color);
	}


	static void drawSquare(const GuiContext& context, float textHeight, ImU32 textColor) {
		float textHeightScaledX = context.scaleX(textHeight / context.getScale().y);
		ImVec2 squareSize = context.scaleVec(SQUARE_SIZE);
		ImVec2 offset = {
				0.5f * (textHeightScaledX + context.scaleX(SELECTABLE_PADDING_XY * 2) - squareSize.x),
				-textHeight
		};

		addRectFilledAtCursor(squareSize, textColor, offset);
	}


	static void scrollToSelf(const GuiContext& context, const ImVec2& region, const ImVec2& padding, const ImVec2& rectSize, float top) {
		float scrollY = ImGui::GetScrollY();
		float height = rectSize.y + 2 * (padding.y + context.scaleY(SELECTABLE_PADDING_XY));
		
		if (top < scrollY) {
			ImGui::SetScrollY(top);
		} else if (top + height > scrollY + region.y) {
			ImGui::SetScrollY(top - region.y + height);
		}
	}


	void MenuSelect::Selectable::drawAnimatedSelectable(MenuSelect& parent, const GuiContext& context, const ImVec2& region, bool isSelected) {
		const float fadeProgress = clamp(time * (1.0f / SELECT_FADE_TIME), 0.0f, 1.0f);

		const ImU32 stripeColor = alphaImU32(Menu::DARK_COLOR, fadeProgress);
		const ImU32 textColor   = mixImU32(ImGui::GetStyleColorVec4(ImGuiCol_Text), Menu::LIGHT_COLOR, fadeProgress);

		const float top = ImGui::GetCursorPosY();
		const float textHeight = ImGui::CalcTextSize(label.c_str(), nullptr, true).y;

		const ImVec2 padding = context.scaleY(0, STRIPE_HEIGHT + SELECTABLE_MARGIN_Y - SELECTABLE_PADDING_XY);
		const ImVec2 stripeSize = context.scaleY(region.x, STRIPE_HEIGHT);
		const ImVec2 rectSize = getRectSize(context, time * (1.0f / SELECT_APPEAR_TIME), fadeProgress, textHeight, isSelected);


		addRectFilledAtCursor(stripeSize, stripeColor);
		ImGui::Dummy(padding);
		addRectFilledAtCursor(rectSize, Menu::DARK_COLOR);

		drawPointerImage(context, fadeProgress);

		ImGui::PushStyleColor(ImGuiCol_Text, textColor);
		const bool clicked = ImGui::Selectable(label.c_str(), true);
		ImGui::PopStyleColor();

		if (clicked || (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow) && isSelected && isEnterPressedOnce())) {
			parent.popup.open([&] () { parent.menu.loadLevel(path); });
		}

		drawSquare(context, textHeight, textColor);

		if (isSelected) {
			scrollToSelf(context, region, padding, rectSize, top);
		}

		ImGui::Dummy(padding);
		addRectFilledAtCursor(stripeSize, stripeColor, ImVec2(0, -stripeSize.y));
	}


	bool MenuSelect::Selectable::drawPlainSelectable(MenuSelect& parent, const GuiContext& context, bool isSelected) {
		const ImVec2 padding = context.scaleY(0, STRIPE_HEIGHT + SELECTABLE_MARGIN_Y);
		ImGui::Dummy(padding);

		const bool clicked = ImGui::Selectable(label.c_str(), false);
		const bool hover = ImGui::IsItemHovered();
		

		if (clicked) {
			parent.popup.open([&] () { parent.menu.loadLevel(path); });
		}

		if (clicked || hover) {
			isSelected = true;
		}

		const float textHeight = ImGui::CalcTextSize(label.c_str(), nullptr, true).y;
		const ImU32 textColor = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Text));
		drawSquare(context, textHeight, textColor);

		ImGui::Dummy(padding - context.scaleY(0, SELECTABLE_PADDING_XY));
		return isSelected;
	}
}