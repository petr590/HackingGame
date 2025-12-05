#include "menu.h"
#include "level/level.h"
#include "texture.h"
#include "dir_paths.h"

namespace hack_game {

	#define BG_TEXTURE TEXTURES_DIR "menu-bg.png"

	static constexpr ImVec4 TEXT_COLOR = colorAsImVec4(0xFF'454232);
	static constexpr float FADE_DURATION = 0.3f;

	Menu::Menu(ShaderManager& shaderManager, size_t levelsCount) noexcept:
			FadingObject(FADE_DURATION, true),
			shaderManager(shaderManager),
			select(*this, levelsCount),
			bgTextureId(Texture(BG_TEXTURE).genGlTexture()) {}

	std::shared_ptr<Player> Menu::getPlayer() const noexcept {
		return level != nullptr ? level->getPlayer() : nullptr;
	}

	void Menu::setLevelDeltaTime(float deltaTime) noexcept {
		if (level != nullptr) {
			level->setDeltaTime(deltaTime);
		}
	}

	void Menu::loadLevel(const std::string& path) {
		level = std::make_shared<Level>(shaderManager, path);
	}

	bool Menu::draw(const GuiContext& context) {
		if (level != nullptr && getFadeTime() == 0) {
			return false;
		}

		updateFadeTime(context.getDeltaTime(), level == nullptr);

		const ImVec2 windowSize = ImGui::GetIO().DisplaySize;

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(windowSize);

		ImGui::PushStyleColor(ImGuiCol_Text,     TEXT_COLOR);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, NORMAL_COLOR);
		ImGui::PushStyleColor(ImGuiCol_ChildBg,  LIGHT_COLOR);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

		ImGui::GetWindowDrawList()->AddImage(bgTextureId, ImVec2(0, 0), windowSize);

		select.draw(context);
		bottomPanel.draw(context);

		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(3);
		return true;
	}
}