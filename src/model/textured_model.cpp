#include "textured_model.h"
#include "texture.h"
#include "dir_paths.h"

#ifndef NDEBUG
#include "shader/shader.h"
#endif

#include <map>
#include <fstream>
#include <filesystem>

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

namespace hack_game {
	using std::string;
	using std::vector;
	using std::pair;
	using std::map;
	using std::ifstream;
	using std::streamsize;
	using std::numeric_limits;
	using std::initializer_list;

	using glm::vec2;
	using glm::vec3;
	using glm::vec4;

	using Vertex = TexturedModel::Vertex;

	struct TexturedModel::Vertex {
		const vec3 pos;
		const vec2 texCoord;

		constexpr Vertex(const vec3& pos, const vec2& texCoord) noexcept:
				pos(pos), texCoord(texCoord) {}
	};



	static void loadImages(initializer_list<const char*> relativeTexturePaths, vector<Texture>& textures) {
		textures.reserve(relativeTexturePaths.size());

		for (const char* relativePath : relativeTexturePaths) {
			const string path = string(TEXTURES_DIR) + relativePath;
			textures.emplace_back(path.c_str());
		}
	}


	static void loadVertices(const char* relativeModelPath, vector<Vertex>& vertices, vector<GLuint>& indices) {
		const string modelPath = string(MODELS_DIR) + relativeModelPath;

		ifstream file(modelPath);

		if (!file.is_open()) {
			throw std::ios_base::failure("Cannot open file '" + modelPath + "'");
		}

		vector<vec3> positions;
		vector<vec2> texCoords;
		map<pair<uint32_t, uint32_t>, uint32_t> verticesMap; // Ключ: {позиция, текстурная координата}, значение: индекс вершины

		for (string tag; file >> tag;) {

			if (tag == "v") {
				vec3& vec = positions.emplace_back();
				file >> vec.x >> vec.y >> vec.z;
				continue;
			}

			if (tag == "vt") {
				vec2& vec = texCoords.emplace_back();
				file >> vec.x >> vec.y;
				continue;
			}

			if (tag == "f") {
				for (int i = 0; i < 3; i++) {
					uint32_t posIndex, texCoordIndex, unused;
					
					file >> posIndex;
					file.ignore(numeric_limits<streamsize>::max(), '/');
					file >> texCoordIndex;
					file.ignore(numeric_limits<streamsize>::max(), '/');
					file >> unused;

					posIndex--, texCoordIndex--;

					auto it = verticesMap.find({posIndex, texCoordIndex});

					if (it != verticesMap.end()) {
						indices.push_back(it->second);

					} else {
						vertices.emplace_back(positions[posIndex], texCoords[texCoordIndex]);
						
						const uint32_t index = vertices.size() - 1;
						verticesMap[{posIndex, texCoordIndex}] = index;
						indices.push_back(index);
					}
				}

				if (file.get() != '\n') {
					throw std::invalid_argument("File '" + modelPath + "' contains more, than 3 vertices for one face");
				}

				continue;
			}

			file.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}


	static void createTextures(const vector<Texture>& textures, vector<GLuint>& textureIds) {
		const size_t size = textures.size();

		textureIds = vector<GLuint>(size, 0);
		glGenTextures(size, textureIds.data());

		for (size_t i = 0; i < size; i++) {
			textures[i].bindGlTexture(textureIds[i]);
		}
	}


	TexturedModel::TexturedModel(const char* relativeModelPath, initializer_list<const char*> relativeTexturePaths) {
		loadImages(relativeTexturePaths, textures);
		loadVertices(relativeModelPath, vertices, indices);
	}

	// Деструктор определён здесь не просто так. Дело в том, что этот деструктор вызывает деструкторы для векторов, а вектора вызывают
	// деструкторы для своих элементов, что приводит к ошибке, если классы TexturedModel::Image и TexturedModel::Vertex ещё не определены.
	// То есть деструктор должен быть определён в классе реализации, а не в заголовке
	TexturedModel::~TexturedModel() {}


	GLuint TexturedModel::createVertexArray() {
		createTextures(textures, textureIds);
		textures.clear();


		GLuint buffers[2], VAO;
		glGenBuffers(2, buffers);
		glGenVertexArrays(1, &VAO);


		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertices.size() * sizeof(vertices[0])), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indices.size() * sizeof(indices[0])), indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, pos)));
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, texCoord)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		return VAO;
	}


	void TexturedModel::draw(Shader& shader) const {
		for (size_t i = 0; i < textureIds.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textureIds[i]);
		}

		VAOModel::draw(shader);
	}
}