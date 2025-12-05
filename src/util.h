#ifndef HACK_GAME__UTIL_H
#define HACK_GAME__UTIL_H

#include <glm/vec3.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace hack_game {

	class Level;


	/// @brief Вектор нормали угла в 2d. От него сичитается угол
	constexpr glm::vec2 ANGLE_NORMAL = {0.0f, 1.0f};

	/// @brief Эпсилон для рассчётов коллизии
	constexpr float EPSILON = 1e-6f;


	/**
	 * @brief
	 * Переводит значение value из диапазона [srcStart; srcEnd] в диапазон [dstStart; dstEnd].
	 * Работает со значениями за пределами исходного диапазона
	 * @return Масштабированное значение в диапазоне [dstStart; dstEnd], если исходное в диапазоне [srcStart; srcEnd]
	 */
	constexpr float zoom(float value, float srcStart, float srcEnd, float dstStart, float dstEnd) {
		return dstStart + (value - srcStart) * (dstEnd - dstStart) / (srcEnd - srcStart);
	}

	/**
	 * Переводит значение value из диапазона [srcStart; srcEnd] в диапазон [0.0f; 1.0f].
	 * Работает со значениями за пределами исходного диапазона
	 * @return Масштабированное значение в диапазоне [0.0f; 1.0f], если исходное в диапазоне [srcStart; srcEnd]
	 */
	constexpr float invLerp(float a, float b, float t) {
		return (t - a) / (b - a);
	}


	constexpr glm::vec3 colorAsVec3(uint32_t color) {
		return glm::vec3(
			((color >> 16) & 0xFF) * (1.f / 255.f),
			((color >>  8) & 0xFF) * (1.f / 255.f),
			((color      ) & 0xFF) * (1.f / 255.f)
		);
	}

	constexpr bool isPointInsideSphere(const glm::vec3& point, const glm::vec3& center, float radius) {
		const glm::vec3 d = point - center;
		return d.x * d.x + d.y * d.y + d.z * d.z <= radius * radius;
	}

	/**
	 * @return Угол в 2d от ANGLE_NORMAL до вектора между pos и lookAt.
	 * Если pos и lookAt находятся в одной точке, то возвращает NaN
	 */
	float horizontalAngleBetween(const glm::vec3& pos, const glm::vec3& lookAt);


	/**
	 * @brief Разрешает коллизию точки с блоками на карте
	 * @param level уровень для получения блока
	 * @param pos позиция точки на плоскости
	 * @param offset смещение точки
	 * @return Новое смещение
	 */
	glm::vec2 resolveBlockCollision(const Level& level, const glm::vec2& pos, glm::vec2 offset);


	/**
	 * Генерирует рандомное значение в диапазоне от low до high.
	 * Обязательные условия: low <= high, low >= 0, high <= RAND_MAX.
	 * При невыполнении условий ошибка в Debug Mode и UB в Release
	 * @return Рандомное значение
	 */
	int randomBetween(int low, int high);

	/// @brief Генерирует рандомное значение в диапазоне от low до high.
	float randomBetween(float low, float high);
	
	/// @brief Генерирует рандомный вектор в диапазоне от low до high.
	glm::vec3 randomBetween(const glm::vec3& low, const glm::vec3& high);

	/// @brief Генерирует рандомный int32_t с лучшим разпределением, чем у стандартного rand().
	/// Гарантируется, что все 32 бита заполнены случайно
	int32_t randomInt32();
}

#endif