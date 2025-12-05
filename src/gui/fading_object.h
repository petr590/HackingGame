#ifndef HACK_GAME__GUI__FADING_OBJECT_H
#define HACK_GAME__GUI__FADING_OBJECT_H

namespace hack_game {

	class FadingObject {
		const float maxTime;
		float time;
	
	public:
		constexpr FadingObject(float maxTime, bool isMax = false) noexcept:
				maxTime(maxTime), time(isMax ? maxTime : 0.0f) {}
		
		void updateFadeTime(float deltaTime, bool add);

		constexpr float getFadeTime() const noexcept {
			return time;
		}
		
		constexpr float getFadeProgress() const noexcept {
			return time / maxTime;
		}
	};
}

#endif