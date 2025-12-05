#ifndef HACK_GAME__MAIN__RENDER_H
#define HACK_GAME__MAIN__RENDER_H

#include <ostream>
#include <memory>

namespace hack_game {

	class RenderContext;
	class ShaderManager;
	class Menu;

	void render(const RenderContext&, ShaderManager&, Menu&, const std::unique_ptr<std::ostream>& fpsFile, float deltaTime);
}

#endif