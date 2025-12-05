#ifndef HACK_GAME__GUI__MENU_H
#define HACK_GAME__GUI__MENU_H

#include "menu_bottom_panel.h"
#include "menu_select.h"
#include "fading_object.h"
#include "imgui_util.h"
#include <string>
#include <memory>

namespace hack_game {

	static constexpr ImVec2 NORMAL_WINDOW_SIZE = {1920, 1025};

	class Player;
	class Level;
	class ShaderManager;

	class Menu: private FadingObject {
		ShaderManager& shaderManager;
		MenuBottomPanel bottomPanel;
		MenuSelect select;
		std::shared_ptr<Level> level = nullptr;
		const GLuint bgTextureId;

	public:
		static constexpr ImU32 LIGHT_COLOR   = colorAsImU32(0xFF'D5CFAE);
		static constexpr ImU32 NORMAL_COLOR  = colorAsImU32(0xFF'C9C5A8);
		static constexpr ImU32 DARKER_COLOR  = colorAsImU32(0xFF'B1AA90);
		static constexpr ImU32 DARK_COLOR    = colorAsImU32(0xFF'4A4840);
		static constexpr ImU32 SHADOW_COLOR  = colorAsImU32(0x3F'000000);
		static constexpr float SHADOW_OFFSET = 4;

		static constexpr ImVec2 POINTER_TEXTURE_SIZE = {45, 45};

		static constexpr float STRIPE1_START = 0;
		static constexpr float STRIPE1_END   = 15 + STRIPE1_START;
		static constexpr float STRIPE2_START = 7 + STRIPE1_END;
		static constexpr float STRIPE2_END   = 3 + STRIPE2_START;

		Menu(ShaderManager&, size_t levelsCount) noexcept;

		float getFadeProgress() const noexcept {
			return FadingObject::getFadeProgress();
		}

		const std::shared_ptr<Level>& getLevel() const noexcept {
			return level;
		}

		void resetLevel() noexcept {
			level.reset();
		}

		std::shared_ptr<Player> getPlayer() const noexcept;

		void setLevelDeltaTime(float) noexcept;
		void loadLevel(const std::string& path);
		bool draw(const GuiContext&);
	};
}

#endif