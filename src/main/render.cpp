#include "render.h"
#include "init.h"
#include "globals.h"
#include "shader/shader_manager.h"
#include "entity/player.h"
#include "model/models.h"
#include "gui/menu.h"
#include "gui/win_screen.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui_util.h"
#include "nowarn_imgui_backends.h"

namespace hack_game {

	#define RGBA(vec) (vec).x, (vec).y, (vec).z, (vec).w

	using std::min;
	using std::pow;
	using std::ostream;
	using std::unique_ptr;

	static constexpr ImVec4 BACKGROUND = colorAsImVec4(0xFF'636155);
	static constexpr float ENDGAME_DURATION = 1.0f;
	static constexpr float FPS_STROKE_SIZE = 1.5f;


	static void renderScene(const RenderContext&, ShaderManager&, const Level&);
	static void renderImGui(const RenderContext&, GuiContext&, Menu&, float winScreenTime);
	static void renderPostprocess(const RenderContext&, const GuiContext&, ShaderManager&, Menu&, float winScreenTime);


	void render(const RenderContext& renderContext, ShaderManager& shaderManager, Menu& menu, const unique_ptr<ostream>& fpsFile, float deltaTime) {
		static GuiContext guiContext;
		static float endGameTime = 0;

		guiContext.setDeltaTime(deltaTime);

		if ((enemyDestroyed || playerDestroyed) && destroyAnimationCount == 0) {
			endGameTime += deltaTime;

			if (endGameTime >= ENDGAME_DURATION) {
				menu.resetLevel();
				endGameTime = 0;
				enemyDestroyed = false;
				playerDestroyed = false;
			}
		}

		const float winScreenTime = enemyDestroyed ? endGameTime : 0;

		if (fpsFile == nullptr) {
			if (menu.getLevel() != nullptr) {
				renderScene(renderContext, shaderManager, *menu.getLevel().get());
			}

			renderImGui(renderContext, guiContext, menu, winScreenTime);
			renderPostprocess(renderContext, guiContext, shaderManager, menu, winScreenTime);

		} else {
			if (menu.getLevel() != nullptr) {
				const float startTime = glfwGetTime();
				renderScene(renderContext, shaderManager, *menu.getLevel().get());
				glFinish();
				const float endTime = glfwGetTime();

				*fpsFile << (1.0f / (endTime  - startTime)) << " fps (render), ";
			}

			renderImGui(renderContext, guiContext, menu, winScreenTime);

			const float startTime = glfwGetTime();
			renderPostprocess(renderContext, guiContext, shaderManager, menu, winScreenTime);
			glFinish();
			const float endTime = glfwGetTime();

			*fpsFile << (1.0f / (endTime - startTime)) << " fps (postprocessing)\n";
		}
	}



	static void renderEntities(ShaderManager& shaderManager, const Level& level, const Level::EntityMap& entityMap) {
		for (auto& entry : entityMap) {
			if (entry.second.empty()) continue;

			const GLuint shaderId = entry.first;

			if (shaderId > 0) {
				Shader& shader = shaderManager.getShader(shaderId);
				shader.use();
				shader.setView(level.getPlayer()->getCamera().getView());
			}
			

			for (const auto& entity : entry.second) {
				entity->draw();
			}
		}
	}


	static void renderScene(const RenderContext& renderContext, ShaderManager& shaderManager, const Level& level) {
		ImGui::SetCurrentContext(renderContext.getImGuiMainContext());

		glBindFramebuffer(GL_FRAMEBUFFER, renderContext.getFbInfo().sceneFramebuffer);
		glClearColor(RGBA(BACKGROUND));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_MULTISAMPLE);
				
		renderEntities(shaderManager, level, level.getOpaqueEntityMap());

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		renderEntities(shaderManager, level, level.getTransparentEntityMap());

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}


	static ImVec2 setImGuiSizeAndScale(GLFWwindow* window) {
		int fbWidth, fbHeight, winWidth, winHeight;
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		glfwGetWindowSize(window, &winWidth, &winHeight);

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(fbWidth, fbHeight);
		io.DisplayFramebufferScale = ImVec2(
				static_cast<float>(fbWidth) / winWidth,
				static_cast<float>(fbHeight) / winHeight
		);

		const ImVec2 scale = io.DisplaySize * (1.0f / NORMAL_WINDOW_SIZE);
		io.FontGlobalScale = min(scale.x, scale.y);
		return scale;
	}


	static void renderImGui(const RenderContext& renderContext, GuiContext& guiContext, Menu& menu, float winScreenTime) {
		static WinScreen winScreen;

		ImGui::SetCurrentContext(renderContext.getImGuiMainContext());
		ImGui_ImplOpenGL3_NewFrame();

		const ImVec2 scale = setImGuiSizeAndScale(renderContext.getWindow());
		setGlobalScale(scale);
		guiContext.setScale(scale);

		imGuiUtilUpdate();
		ImGui::NewFrame();

		const bool menuRendered = menu.draw(guiContext);
		winScreen.draw(guiContext, winScreenTime);

		ImGui::Render();
		glBindFramebuffer(GL_FRAMEBUFFER, renderContext.getFbInfo().imGuiFramebuffer);

		if (!menuRendered) {
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
		}

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}


	static void renderFPS(const GuiContext& context) {
		static float timeSum = 0;
		static int times = 0;
		static int fps = 0;

		timeSum += context.getDeltaTime();
		times += 1;

		if (timeSum >= 1) {
			fps = static_cast<int>(times / timeSum);
			timeSum = 0;
			times = 0;
		}
		
		const std::string text = "FPS: " + std::to_string(fps);

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin("FpsWindow", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::SetWindowFontScale(0.75f);

		static constexpr ImVec2 STROKE_COORDS[] = {
			{0, 0},
			{0, FPS_STROKE_SIZE * 2},
			{FPS_STROKE_SIZE * 2, 0},
			{FPS_STROKE_SIZE * 2, FPS_STROKE_SIZE * 2}
		};

		for (const ImVec2& pos : STROKE_COORDS) {
			ImGui::SetCursorPos(context.scaleVec(pos));
			ImGui::TextColored(colorAsImVec4(0xFF'000000), "%s", text.c_str());
		}

		ImGui::SetCursorPos(context.scaleVec(FPS_STROKE_SIZE, FPS_STROKE_SIZE));
		ImGui::TextColored(colorAsImVec4(0xFF'FFFFFF), "%s", text.c_str());
		ImGui::End();
	}


	static void renderPostprocess(const RenderContext& renderContext, const GuiContext& guiContext, ShaderManager& shaderManager, Menu& menu, float winScreenTime) {
		const float windowWidth = renderContext.getWindowWidth();
		const float windowHeight = renderContext.getWindowHeight();

		glBindFramebuffer(GL_READ_FRAMEBUFFER, renderContext.getFbInfo().sceneFramebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderContext.getFbInfo().sceneNoMsFramebuffer);
		glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_MULTISAMPLE);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderContext.getFbInfo().sceneNoMsTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, renderContext.getFbInfo().imGuiTexture);

		Shader& postprocessing = shaderManager.getShader("postprocessing");
		postprocessing.use();
		postprocessing.setUniform("winScreenTime", winScreenTime);
		postprocessing.setUniform("guiFadeProgress", menu.getFadeProgress());
		
		models::postprocessingModel.draw(postprocessing);

		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		ImGui::SetCurrentContext(renderContext.getImGuiFpsContext());
		ImGui_ImplOpenGL3_NewFrame();
		setImGuiSizeAndScale(renderContext.getWindow());
		ImGui::NewFrame();
		renderFPS(guiContext);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}