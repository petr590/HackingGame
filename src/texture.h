#ifndef HACK_GAME__TEXTURE_H
#define HACK_GAME__TEXTURE_H

#include "gl_fwd.h"

namespace hack_game {

	/// @brief Данный класс отвечает за загрузку текстуры и генерацию объекта текстуры в OpenGL
	class Texture {
		int width, height;
		uint8_t* data;

	public:
		/// @brief Загружает текстуру из файла
		/// @param path путь к файлу относительно текущей папки или абсолютный путь
		explicit Texture(const char* path);

		/// @brief Очищает все данные текстуры в памяти (но не в OpenGL!)
		~Texture();

		/// @brief Работает также, как genGlTexture(GLint). filter по умолчанию GL_LINEAR
		/// @return ID созданного объекта текстуры
		GLuint genGlTexture() const;

		/// @brief Создаёт новый объект текстуры в OpenGL
		/// @param filter Режим фильтрации текстур OpenGL
		/// @return ID созданного объекта текстуры
		GLuint genGlTexture(GLint filter) const;
		
		/// @brief Работает также, как bindGlTexture(GLuint, GLint). filter по умолчанию GL_LINEAR
		/// @param textureId ID объекта текстуры в OpenGL
		void bindGlTexture(GLuint textureId) const;
		
		/// @brief Привязывает данные текстуры к объекту OpenGL
		/// @param textureId ID объекта текстуры в OpenGL
		/// @param filter Режим фильтрации текстур OpenGL
		void bindGlTexture(GLuint textureId, GLint filter) const;
	};
}

#endif