#ifndef HACK_GAME__CONTEXT__SHADER_H
#define HACK_GAME__CONTEXT__SHADER_H

#include "gl_fwd.h"
#include <vector>
#include <string>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace hack_game {

	class Shader {
	public:
		struct Uniform;
		using uniforms_t = std::vector<std::pair<std::string, Uniform>>;

	private:
		const char* const name;
		const GLuint id;
		const uniforms_t uniforms;
	
	public:
		explicit Shader(const char* name) noexcept;
		Shader(const char* name, GLuint shaderProgram);
		Shader(Shader&&);
		~Shader();

		const char* getName() const noexcept {
			return name;
		}

		GLuint getId() const noexcept {
			return id;
		}

		void use() noexcept;

		void setUniform(const char* name, const glm::mat4& val, bool warn = true);
		void setUniform(const char* name, const glm::vec3& val, bool warn = true);
		void setUniform(const char* name, const glm::vec2& val, bool warn = true);
		void setUniform(const char* name, float val, bool warn = true);
		void setUniform(const char* name, GLint val, bool warn = true);
		void setUniform(const char* name, GLuint val, bool warn = true);

		void setModel(const glm::mat4&);
		void setView(const glm::mat4&);
		void setModelColor(const glm::vec3&);
	};
}

#endif