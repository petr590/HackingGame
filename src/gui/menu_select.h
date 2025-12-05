#ifndef HACK_GAME__GUI__MENU_SELECT_H
#define HACK_GAME__GUI__MENU_SELECT_H

#include "gui_context.h"
#include "system_message_popup.h"
#include "gl_fwd.h"
#include <vector>

namespace hack_game {

	class Menu;

	class MenuSelect {
	public:
		class Selectable;

	private:
		Menu& menu;
		SystemMessagePopup popup;

		std::vector<Selectable> selectables;
		size_t selected = 0;

	public:
		MenuSelect(Menu&, size_t count) noexcept;
		~MenuSelect();

		void draw(const GuiContext&);
	
	private:
		void drawContent(const GuiContext&);
	};
}

#endif