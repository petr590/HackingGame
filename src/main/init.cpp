#include "init.h"
#include "main.h"
#include "model/models.h"
#include "shader/shader_manager.h"
#include "entity/simple_entity.h"
#include "entity/player.h"
#include "entity/enemy.h"
#include "entity/block.h"
#include "entity/minion.h"

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "nowarn_imgui.h"
#include "nowarn_imgui_backends.h"

namespace hack_game {
	using std::cerr;
	using std::endl;


	static const GLint SAMPLES = 4;
	static const char* const GLSL_VERSION = "#version 330 core";

	static RenderContext* renderContext = nullptr;

	const RenderContext& RenderContext::getInstance() {
		static RenderContext instance;
		renderContext = &instance;
		return instance;
	}


	static const GLFWvidmode* initGLFW();
	static GLFWwindow* initWindow(GLint width, GLint height);
	static void initGLEW();
	static ImGuiContext* createImGuiContext(GLFWwindow*, bool isMain);

	static void shutdownImGui();
	static void shutdownGLFW(GLFWwindow*);
	
	static FramebufferInfo initGL(GLFWwindow* window, GLint windowWidth, GLint windowHeight);
	static void framebufferSizeCallback(GLFWwindow* window, GLint width, GLint height);


	RenderContext::RenderContext() {
		const GLFWvidmode* mode = initGLFW();
		windowWidth = mode->width;
		windowHeight = mode->height;
		refreshRate = mode->refreshRate;

		window = initWindow(windowWidth, windowHeight);
		initGLEW();

		IMGUI_CHECKVERSION();
		imGuiMainContext = createImGuiContext(window, true);
		imGuiFpsContext = createImGuiContext(window, false);
		ImGui::SetCurrentContext(imGuiMainContext);

		fbInfo = initGL(window, windowWidth, windowHeight);
	}

	RenderContext::~RenderContext() {
		shutdownImGui();
		shutdownGLFW(window);
	}


	// ------------------------------------------- init -------------------------------------------

	static const GLFWvidmode* initGLFW() {
		if (!glfwInit()) {
			cerr << "Failed to create initialize GLFW" << endl;
			exit(EXIT_FAILURE);
		}
		
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		return glfwGetVideoMode(glfwGetPrimaryMonitor());
	}


	static GLFWwindow* initWindow(GLint width, GLint height) {
		
		GLFWwindow* window = glfwCreateWindow(width, height, "Hacking Game", nullptr, nullptr);
		if (window == nullptr) {
			cerr << "Failed to create GLFW window" << endl;
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
		
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);
		glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

		return window;
	}

	static void initGLEW() {
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			cerr << "Failed to initialize GLEW" << endl;
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
	}

	
	static ImGuiContext* createImGuiContext(GLFWwindow* window, bool isMain) {
		ImGuiContext* context = ImGui::CreateContext();
		ImGui::SetCurrentContext(context);

		ImGui::StyleColorsDark();
		const float mainScale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());

		ImGuiStyle& style = ImGui::GetStyle();
		style.ScaleAllSizes(mainScale);
		style.FontScaleDpi = mainScale;
		style.FontSizeBase = 35.0f;

		ImFont* font = ImGui::GetIO().Fonts->AddFontFromFileTTF("resources/fonts/TikTok_Sans_Regular.ttf");
    	IM_ASSERT(font != nullptr);

		if (!isMain) {
			ImGui::GetIO().WantCaptureKeyboard = false;
			ImGui::GetIO().WantCaptureMouse = false;
		}

		ImGui_ImplGlfw_InitForOpenGL(window, isMain);
		ImGui_ImplOpenGL3_Init(GLSL_VERSION);

		return context;
	}
	

	static void shutdownImGui() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	static void shutdownGLFW(GLFWwindow* window) {
		glfwDestroyWindow(window);
		glfwTerminate();
	}


	static void generateSceneMsBuffer(FramebufferInfo& fbInfo, GLint windowWidth, GLint windowHeight) {
		GLuint& framebuffer  = fbInfo.sceneFramebuffer;
		GLuint& renderbuffer = fbInfo.sceneRenderbuffer;
		GLuint& texture      = fbInfo.sceneMsTexture;

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, SAMPLES, GL_RGBA, windowWidth, windowHeight, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture, 0);

		glGenRenderbuffers(1, &renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, SAMPLES, GL_DEPTH_COMPONENT24, windowWidth, windowHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	static void generateSceneNoMsBuffer(FramebufferInfo& fbInfo, GLint windowWidth, GLint windowHeight) {
		GLuint& framebuffer = fbInfo.sceneNoMsFramebuffer;
		GLuint& texture     = fbInfo.sceneNoMsTexture;

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	static void generateImGuiBuffer(FramebufferInfo& fbInfo, GLint windowWidth, GLint windowHeight) {
		GLuint& framebuffer = fbInfo.imGuiFramebuffer;
		GLuint& texture     = fbInfo.imGuiTexture;

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	static FramebufferInfo initGL(GLFWwindow* window, GLint windowWidth, GLint windowHeight) {
		for (Model* model : Model::getModels()) {
			model->generateVertexArray();
		}

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		GLint width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		
		FramebufferInfo fbInfo;
		generateSceneMsBuffer(fbInfo, windowWidth, windowHeight);
		generateSceneNoMsBuffer(fbInfo, windowWidth, windowHeight);
		generateImGuiBuffer(fbInfo, windowWidth, windowHeight);
		return fbInfo;
	}


	static void framebufferSizeCallback(GLFWwindow* window, GLint width, GLint height) {
		renderContext->setWindowWidth(width);
		renderContext->setWindowHeight(height);

		const FramebufferInfo& fbInfo = renderContext->getFbInfo();

		GLint fbWidth, fbHeight;
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		glViewport(0, 0, fbWidth, fbHeight);
		
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fbInfo.sceneMsTexture);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, SAMPLES, GL_RGBA, width, height, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, fbInfo.sceneRenderbuffer);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, SAMPLES, GL_DEPTH_COMPONENT24, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, fbInfo.sceneNoMsTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindTexture(GL_TEXTURE_2D, fbInfo.imGuiTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);

		onChangeWindowSize(width, height);
	}
}
