#ifndef HACK_GAME__MAIN__GLOBALS_H
#define HACK_GAME__MAIN__GLOBALS_H

namespace hack_game {
	
	extern bool enemyDestroyed, playerDestroyed;

	/// Количество анимаций уничтожения Player и Enemy.
	/// Пока оно больше 0, экран конца игры не показывается
	extern int destroyAnimationCount;

	inline bool gameEnded() {
		return (playerDestroyed || enemyDestroyed) && destroyAnimationCount == 0;
	}
}

#endif