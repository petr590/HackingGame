#include "fading_object.h"
#include <algorithm>

namespace hack_game {

	void FadingObject::updateFadeTime(float deltaTime, bool add) {
		if (add) {
			time = std::min(time + deltaTime, maxTime);
		} else {
			time = std::max(time - deltaTime, 0.0f);
		}
	}
}