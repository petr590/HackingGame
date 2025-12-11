#include "main.h"
#include "init.h"
#include "render.h"
#include "shader/shader_manager.h"
#include "entity/player.h"
#include "gui/menu.h"

#include <fstream>
#include <iomanip>
#include <thread>
#include <chrono>

#include <GLFW/glfw3.h>
#include "nowarn_imgui.h"
#include "nowarn_imgui_backends.h"

namespace hack_game {

	using std::unique_ptr;
	using std::shared_ptr;
	using std::ostream;
	using std::ofstream;

	static volatile bool paused = false;
	static volatile bool nextFrame = false;

	static void updateKeys(const RenderContext& renderContext, const shared_ptr<Player>& player);
	static void renderEmptyImGui();

	static void tick(Level& level, const Level::EntityMap& entityMap);
	static void tick(Level& level);


	/// @brief Главный цикл всей игры. Этапы:
	/// 1. Обновление клавиш
	/// 2. Обновление состояния всех сущностей (в том числе просчёт коллизий)
	/// 3. Отрисовка сцены и GUI
	/// При паузе обновляет только состояние клавиш
	void mainLoop(const RenderContext& renderContext, ShaderManager& shaderManager, bool profile) {
		static Menu menu(shaderManager, 48);

		GLFWwindow* const window = renderContext.getWindow();

		unique_ptr<ostream> fpsFile = nullptr;

		if (profile) {
			fpsFile = std::make_unique<ofstream>("/tmp/fps.log");
			*fpsFile << std::fixed << std::setprecision(2) << std::setw(7);
		}

		const float waitTime = 1.0f / renderContext.getRefreshRate();

		for (float lastFrame = 0; !glfwWindowShouldClose(window);) {
			const float currentFrame = glfwGetTime();
			const float deltaTime = currentFrame - lastFrame;
			menu.setLevelDeltaTime(deltaTime);
			lastFrame = currentFrame;

			updateKeys(renderContext, menu.getPlayer());

			if (menu.getLevel() != nullptr) {
				tick(*menu.getLevel());
			}

			render(renderContext, shaderManager, menu, fpsFile, deltaTime);
			glfwSwapBuffers(window);

			// check paused
			if (paused) {
				while (paused && !nextFrame && !glfwWindowShouldClose(window)) {
					std::this_thread::sleep_for(std::chrono::nanoseconds(static_cast<uint64_t>(waitTime * (1e9f / 2))));

					updateKeys(renderContext, menu.getPlayer());
					renderEmptyImGui();
				}

				nextFrame = false;
				lastFrame = glfwGetTime() - waitTime;
			}
		}
	}


	static void updateKeys(const RenderContext& renderContext, const shared_ptr<Player>& player) {
		ImGui::SetCurrentContext(renderContext.getImGuiMainContext());
		glfwPollEvents();
		ImGui_ImplGlfw_NewFrame();

		if (player != nullptr) {
			player->updateKeys();
		}
		
		if (ImGui::IsKeyPressed(ImGuiKey_F1)) {
			paused = !paused;

			ImGuiKeyData& data = ImGui::GetIO().KeysData[ImGuiKey_F1 - ImGuiKey_NamedKey_BEGIN];
			data.Down = false;
			data.DownDuration = -1.0f;
			data.DownDurationPrev = -1.0f;
		}

		if (ImGui::IsKeyPressed(ImGuiKey_F2) && paused) {
			nextFrame = true;
		}
	}


	static void renderEmptyImGui() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}


	static void tick(Level& level, const Level::EntityMap& entityMap) {
		for (auto& entry : entityMap) {
			for (const auto& entity : entry.second) {
				entity->tick(level);
			}
		}
	}


	static void tick(Level& level) {
		tick(level, level.getOpaqueEntityMap());
		tick(level, level.getTransparentEntityMap());
		level.updateEntities();
	}
}