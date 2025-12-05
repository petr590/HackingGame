#ifndef HACK_GAME__MAIN__INIT_H
#define HACK_GAME__MAIN__INIT_H

#include "level/level.h"

class GLFWwindow;
class ImFont;
class ImGuiContext;

namespace hack_game {
	class ShaderManager;

	struct FramebufferInfo {
		GLuint sceneFramebuffer;  /// Фреймбуфер для рендера сцены
		GLuint sceneMsTexture;    /// Текстура с мультисемплингом, в которую рендерится сцена
		GLuint sceneRenderbuffer; /// Рендербуфер с мультисемплингом для рендера сцены. Хранит глубину

		GLuint sceneNoMsFramebuffer; /// Промежуточный фреймбуфер для блиттинга мультисэмпл-текстуры в обычную
		GLuint sceneNoMsTexture;     /// Текстура без мультисемплинга, в которую копируется содержимое sceneMsTexture

		GLuint imGuiFramebuffer; /// Фреймбуфер для рендера ImGui
		GLuint imGuiTexture;     /// Текстура для рендера ImGui
	};


	class RenderContext {
		GLFWwindow* window;

		ImGuiContext* imGuiMainContext;
		ImGuiContext* imGuiFpsContext;

		FramebufferInfo fbInfo;
		GLint windowWidth;
		GLint windowHeight;
		int refreshRate;

		RenderContext();
		~RenderContext();

		RenderContext(const RenderContext&) = delete;

	public:
		static const RenderContext& getInstance();

		GLFWwindow* getWindow() const noexcept {
			return window;
		}

		ImGuiContext* getImGuiMainContext() const noexcept {
			return imGuiMainContext;
		}

		ImGuiContext* getImGuiFpsContext() const noexcept {
			return imGuiFpsContext;
		}

		const FramebufferInfo& getFbInfo() const noexcept {
			return fbInfo;
		}

		GLint getWindowWidth() const noexcept {
			return windowWidth;
		}

		GLint getWindowHeight() const noexcept {
			return windowHeight;
		}

		int getRefreshRate() const noexcept {
			return refreshRate;
		}

		void setWindowWidth(GLint windowWidth) noexcept {
			this->windowWidth = windowWidth;
		}

		void setWindowHeight(GLint windowHeight) noexcept {
			this->windowHeight = windowHeight;
		}
	};

	void onChangeWindowSize(GLint width, GLint height);
}

#endif