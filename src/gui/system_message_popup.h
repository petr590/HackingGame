#ifndef HACK_GAME__GUI__SYSTEM_MESSAGE_POPUP_H
#define HACK_GAME__GUI__SYSTEM_MESSAGE_POPUP_H

#include "gui_context.h"
#include "fading_object.h"
#include <functional>

namespace hack_game {

	class SystemMessagePopup: private FadingObject {
	public:
		using callback_t = std::function<void()>;

		class Selectable: private FadingObject {
		public:
			Selectable(bool isSelected) noexcept;
			bool draw(const GuiContext&, const char*, const ImVec2&, bool, const callback_t&);
		};

	private:
		callback_t callback = nullptr;
		Selectable yes, no;
		bool yesSelected;

	public:
		SystemMessagePopup() noexcept;

		void open(callback_t);
		void draw(const GuiContext&);
	
	private:
		void close();
	};
}

#endif