#include "shader_loader.h"
#include "dir_paths.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>

namespace hack_game {
	using std::cout;
	using std::cerr;
	using std::endl;
	using std::string;
	using std::ios;
	using std::ifstream;
	using std::streamsize;
	using std::make_unique;

	#define COMMON_FILE_NAME "common.glsl"
	#define COMMON_INCLUDE "#include \"" COMMON_FILE_NAME "\""

	const size_t LOG_SIZE = 512;

	static const string& getCommonContent();

	static string readFile(const char* path) {
		ifstream file(path, ios::ate);

		if (file.fail()) {
			cerr << "Cannot open file \"" << path << "\"" << endl;
			exit(EXIT_FAILURE);
		}

		const streamsize fsize = file.tellg();
		
		string content(fsize, '\0');
		
		file.seekg(0);
		file.read(content.data(), fsize);
		file.close();

		return content;
	}

	static GLuint compileShader(GLenum type, const char* path) {
		string source = readFile(path);
		size_t includePos = source.find(COMMON_INCLUDE);

		if (includePos != string::npos) {
			source.replace(includePos, std::size(COMMON_INCLUDE) - 1, getCommonContent());
		}

		GLuint shader = glCreateShader(type);

		const char* str = source.c_str();
		glShaderSource(shader, 1, &str, nullptr);

		source.clear();
		
		
		glCompileShader(shader);

		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

		string log(size_t(logLength), '\0');
		glGetShaderInfoLog(shader, logLength, nullptr, log.data());

		GLint success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (success) {
			if (logLength > 0) {
				cout << log << endl;
			}

		} else {
			const char* strType =
					type == GL_VERTEX_SHADER ? "vertex" :
					type == GL_FRAGMENT_SHADER ? "fragment" : "(unknown type)";
			
			cerr << "Compilation failed for " << strType << " shader \"" << path << "\":\n" << log << endl;
			exit(EXIT_FAILURE);
		}
		
		return shader;
	}


	static GLuint createShaderProgram0(const char* vertexShaderPath, const char* fragmentShaderPath) {
		// Читаем и компилируем шейдеры
		GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderPath);
		GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderPath);

		if (vertexShader == 0 || fragmentShader == 0) {
			exit(EXIT_FAILURE);
		}
		
		
		// Связываем шейдеры в программу
		GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);

		glLinkProgram(shaderProgram);
		
		GLint success;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			GLchar infoLog[LOG_SIZE];
			glGetProgramInfoLog(shaderProgram, LOG_SIZE, NULL, infoLog);
			cerr << "ERROR::PROGRAM::LINKAGE_FAILED\n" << infoLog << endl;
			return 0;
		}
		
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return shaderProgram;
	}


	GLuint createShaderProgram(const char* vertexShaderName, const char* fragmentShaderName) {
		string vertexShaderPath   = string(SHADERS_DIR) + vertexShaderName;
		string fragmentShaderPath = string(SHADERS_DIR) + fragmentShaderName;
		return createShaderProgram0(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
	}

	GLuint createAnimationShaderProgram(const char* vertexShaderName, const char* fragmentShaderName) {
		string vertexShaderPath   = string(SHADERS_ANIMATION_DIR) + vertexShaderName;
		string fragmentShaderPath = string(SHADERS_ANIMATION_DIR) + fragmentShaderName;
		return createShaderProgram0(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
	}


	static string commonContent;

	static const string& getCommonContent() {
		static bool loaded = false;

		if (!loaded) {
			commonContent = readFile(SHADERS_DIR "common.glsl");
			loaded = true;
		}

		return commonContent;
	}

	void onShadersLoaded() {
		commonContent.clear();
	}
}
