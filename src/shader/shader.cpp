#include "shader.h"
#include <string_view>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <algorithm>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

namespace hack_game {
	using std::cerr;
	using std::endl;
	using std::string;
	using std::string_view;
	using std::function;

	using glm::vec2;
	using glm::vec3;
	using glm::mat4;

	using Uniform = Shader::Uniform;
	using uniforms_t = Shader::uniforms_t;

	struct Shader::Uniform {
		Uniform() noexcept {}

		#ifndef NDEBUG
		GLint id;
		GLenum type;
		constexpr Uniform(GLint id, GLenum type) noexcept: id(id), type(type) {}
		#else
		GLint id;
		constexpr Uniform(GLint id, GLenum) noexcept: id(id) {}
		#endif
	};


	static uniforms_t createUniforms(GLuint id) {
		GLint count, maxNameLen;

		glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count);
		glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLen);

		uniforms_t uniforms;
		uniforms.reserve(count);

		// Некоторые реализации OpenGL не принимают nullptr в качестве параметров.
		// Поэтому на всякий случай лучше создать переменные для этого.
		GLsizei unused1;
		GLint unused2;

		for (GLint i = 0; i < count; i++) {
			GLchar name[maxNameLen];
			GLenum type;
			
			glGetActiveUniform(id, static_cast<GLuint>(i), maxNameLen, &unused1, &unused2, &type, name);
			uniforms.emplace_back(string(name), Uniform(i, type));
		}

		assert(uniforms.size() == static_cast<size_t>(count));
		return uniforms;
	}


	Shader::Shader(const char* name) noexcept:
			name(name), id(0), uniforms() {}

	Shader::Shader(const char* name, GLuint id):
			name(name), id(id), uniforms(createUniforms(id)) {
		
		use();
		setUniform("modelBrightness", 1.0f, false);
		setUniform("texture0", 0, false);
		setUniform("texture1", 1, false);
		setUniform("texture2", 2, false);
		setUniform("texture3", 3, false);
	}

	Shader::Shader(Shader&& shader):
			name(shader.name), id(shader.id), uniforms(std::move(shader.uniforms)) {}
	
	Shader::~Shader() {}


	#ifndef NDEBUG
	static GLuint lastUsedId = 0;

	void Shader::use() noexcept {
		glUseProgram(id);
		lastUsedId = id;
	}
	#else
	void Shader::use() noexcept {
		glUseProgram(id);
	}
	#endif


	template<GLenum... types, class Func>
	static void setUniform0(const uniforms_t& uniforms, const char* uniformName, Shader& shader, bool warn, Func func, const char* typeNames) {
		(void)typeNames;


		#ifndef NDEBUG
		if (shader.getId() != lastUsedId) {
			fprintf(stderr, "Warning: shader \"%s\" was not used before setUniform\n", shader.getName());
		}
		#endif

		
		string_view name = uniformName;

		auto it = std::find_if(
				uniforms.begin(), uniforms.end(),
				[name] (const auto& p) { return p.first == name; }
		);

		if (it != uniforms.end()) {
		
			#ifndef NDEBUG
			const GLenum uniformType = it->second.type;

			if (((uniformType != types) && ...)) {
				fprintf(stderr, "Warning: uniform \"%s\" is incompatible with types %s\n", uniformName, typeNames);
			}
			#endif

			func(it->second.id);
		} else if (warn) {
			fprintf(stderr, "Warning: uniform \"%s\" is not found for shader \"%s\"\n", uniformName, shader.getName());
		}
	}

	#ifndef NDEBUG
	#define TEMPLATED_CALL(...) setUniform0<__VA_ARGS__>
	#define SET_UNIFORM(TEMPLATE_PARAMS, ...) TEMPLATED_CALL TEMPLATE_PARAMS (__VA_ARGS__, #TEMPLATE_PARAMS)
	#else
	#define SET_UNIFORM(TEMPLATE_PARAMS, ...) setUniform0(__VA_ARGS__, nullptr)
	#endif


	void Shader::setUniform(const char* uniformName, const mat4& val, bool warn) {
		SET_UNIFORM((GL_FLOAT_MAT4), uniforms, uniformName, *this, warn, [&] (GLint uid) { glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(val)); });
	}

	void Shader::setUniform(const char* uniformName, const vec3& val, bool warn) {
		SET_UNIFORM((GL_FLOAT_VEC3), uniforms, uniformName, *this, warn, [&] (GLint uid) { glUniform3fv(uid, 1, glm::value_ptr(val)); });
	}

	void Shader::setUniform(const char* uniformName, const vec2& val, bool warn) {
		SET_UNIFORM((GL_FLOAT_VEC2), uniforms, uniformName, *this, warn, [&] (GLint uid) { glUniform2fv(uid, 1, glm::value_ptr(val)); });
	}

	void Shader::setUniform(const char* uniformName, float val, bool warn) {
		SET_UNIFORM((GL_FLOAT), uniforms, uniformName, *this, warn, [=] (GLint uid) { glUniform1f(uid, val); });
	}

	void Shader::setUniform(const char* uniformName, GLint val, bool warn) {
		SET_UNIFORM((GL_INT, GL_SAMPLER_1D, GL_SAMPLER_2D, GL_SAMPLER_3D), uniforms, uniformName, *this, warn, [=] (GLint uid) { glUniform1i(uid, val); });
	}

	void Shader::setUniform(const char* uniformName, GLuint val, bool warn) {
		SET_UNIFORM((GL_UNSIGNED_INT), uniforms, uniformName, *this, warn, [=] (GLint uid) { glUniform1ui(uid, val); });
	}


	void Shader::setModel(const mat4& mat) {
		setUniform("model", mat);
	}

	void Shader::setView(const mat4& mat) {
		setUniform("view", mat);
	}

	void Shader::setModelColor(const vec3& vec) {
		setUniform("modelColor", vec, false);
	}
}
