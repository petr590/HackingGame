#include "gui_context.h"
#include "texture.h"
#include "dir_paths.h"

namespace hack_game {

	#define POINTER_TEXTURE TEXTURES_DIR "pointer.png"

	GuiContext::GuiContext():
			pointerTextureId(Texture(POINTER_TEXTURE).genGlTexture()) {}
}