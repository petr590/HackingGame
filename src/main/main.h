#ifndef HACK_GAME__MAIN__MAIN_H
#define HACK_GAME__MAIN__MAIN_H

#include "init.h"

namespace hack_game {
	class RenderContext;
	void mainLoop(const RenderContext&, ShaderManager&, bool profile);
}

#endif