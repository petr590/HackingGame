#ifndef HACK_GAME__IMGUI_UTIL_H
#define HACK_GAME__IMGUI_UTIL_H

#include "nowarn_imgui.h"
#include <cstdint>

namespace hack_game {

	/// @brief Устанавливает вектор масштабирования
	void setGlobalScale(const ImVec2&);

	/// @brief Отрисовывает прямоугольник с масштабированными координатами
	void addRectFilledScaled(const ImVec2& pMin, const ImVec2& pMax, ImU32 color, float rounding = 0, ImDrawFlags flags = 0);

	/// @return true для первого вызова, если нажата клавиша Enter. Для всех
	/// остальных вызовов вернёт false, пока не будет вызвана функция imGuiUtilUpdate
	bool isEnterPressedOnce();

	/// @brief Сбрасывает внутреннее состояние для функции isEnterPressedOnce
	void imGuiUtilUpdate();

	static constexpr ImVec2 operator+(const ImVec2& vec1, const ImVec2& vec2) noexcept { return ImVec2(vec1.x + vec2.x, vec1.y + vec2.y); }
	static constexpr ImVec2 operator-(const ImVec2& vec1, const ImVec2& vec2) noexcept { return ImVec2(vec1.x - vec2.x, vec1.y - vec2.y); }
	static constexpr ImVec2 operator*(const ImVec2& vec1, const ImVec2& vec2) noexcept { return ImVec2(vec1.x * vec2.x, vec1.y * vec2.y); }
	static constexpr ImVec2 operator/(const ImVec2& vec1, const ImVec2& vec2) noexcept { return ImVec2(vec1.x / vec2.x, vec1.y / vec2.y); }

	static constexpr ImVec2 operator+(const ImVec2& vec, float scalar) noexcept { return vec + ImVec2(scalar, scalar); }
	static constexpr ImVec2 operator-(const ImVec2& vec, float scalar) noexcept { return vec - ImVec2(scalar, scalar); }
	static constexpr ImVec2 operator*(const ImVec2& vec, float scalar) noexcept { return vec * ImVec2(scalar, scalar); }
	static constexpr ImVec2 operator/(const ImVec2& vec, float scalar) noexcept { return vec / ImVec2(scalar, scalar); }

	static constexpr ImVec2 operator+(float scalar, const ImVec2& vec) noexcept { return ImVec2(scalar, scalar) + vec; }
	static constexpr ImVec2 operator-(float scalar, const ImVec2& vec) noexcept { return ImVec2(scalar, scalar) - vec; }
	static constexpr ImVec2 operator*(float scalar, const ImVec2& vec) noexcept { return ImVec2(scalar, scalar) * vec; }
	static constexpr ImVec2 operator/(float scalar, const ImVec2& vec) noexcept { return ImVec2(scalar, scalar) / vec; }


	/// @brief Преобразует цвет из формата ARGB в формат ImGui
	static constexpr ImU32 colorAsImU32(uint32_t argb) {
		return  ((static_cast<ImU32>(argb >> 16) & 0xFF) << IM_COL32_R_SHIFT) |
				((static_cast<ImU32>(argb >>  8) & 0xFF) << IM_COL32_G_SHIFT) |
				((static_cast<ImU32>(argb >>  0) & 0xFF) << IM_COL32_B_SHIFT) |
				((static_cast<ImU32>(argb >> 24) & 0xFF) << IM_COL32_A_SHIFT);
	}

	/// @brief Преобразует цвет из формата ARGB в вектор с 4 компонентами ImGui
	static constexpr ImVec4 colorAsImVec4(uint32_t argb) {
		return ImVec4(
			((argb >> 16) & 0xFF) * (1.f / 255.f),
			((argb >>  8) & 0xFF) * (1.f / 255.f),
			((argb      ) & 0xFF) * (1.f / 255.f),
			((argb >> 24) & 0xFF) * (1.f / 255.f)
		);
	}

	/// @brief Накладывает второй цвет на первый с учётом альфа-канала
	static constexpr ImVec4 mix(const ImVec4& c1, const ImVec4& c2, float a) {
		return ImVec4(
			c1.x + (c2.x - c1.x) * a,
			c1.y + (c2.y - c1.y) * a,
			c1.z + (c2.z - c1.z) * a,
			c1.w + (c2.w - c1.w) * a
		);
	}

	/// @brief Накладывает второй цвет на первый с учётом альфа-канала
	static inline ImVec4 mix(ImU32 c1, ImU32 c2, float a) {
		return mix(ImGui::ColorConvertU32ToFloat4(c1), ImGui::ColorConvertU32ToFloat4(c2), a);
	}

	/// @brief Накладывает второй цвет на первый с учётом альфа-канала
	static inline ImVec4 mix(const ImVec4& c1, ImU32 c2, float a) {
		return mix(c1, ImGui::ColorConvertU32ToFloat4(c2), a);
	}

	/// @brief Накладывает второй цвет на первый с учётом альфа-канала
	static inline ImVec4 mix(ImU32 c1, const ImVec4& c2, float a) {
		return mix(ImGui::ColorConvertU32ToFloat4(c1), c2, a);
	}


	/// @brief Накладывает второй цвет на первый с учётом альфа-канала
	template<typename C1, typename C2>
	static inline ImU32 mixImU32(const C1& c1, const C2& c2, float a) {
		return ImGui::ColorConvertFloat4ToU32(mix(c1, c2, a));
	}


	/// @brief Умножает w-компоненту цвета на альфа-канал
	static constexpr ImVec4 alpha(const ImVec4& color, float alpha) {
		return ImVec4(color.x, color.y, color.z, color.w * alpha);
	}

	/// @brief Умножает w-компоненту цвета на альфа-канал
	static constexpr ImU32 alphaImU32(ImU32 color, float alpha) {
		uint8_t a = (color & IM_COL32_A_MASK) >> IM_COL32_A_SHIFT;
		return (color & ~IM_COL32_A_MASK) | ((static_cast<ImU32>(a * alpha) << IM_COL32_A_SHIFT) & IM_COL32_A_MASK);
	}
}

#endif