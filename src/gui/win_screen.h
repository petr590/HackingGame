#ifndef HACK_GAME__GUI__WIN_SCREEN_H
#define HACK_GAME__GUI__WIN_SCREEN_H

#include "gui_context.h"

namespace hack_game {

	class WinScreen {
	public:
		void draw(const GuiContext&, float winScreenTime);
	};
}

#endif