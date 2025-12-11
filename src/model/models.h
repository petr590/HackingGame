#ifndef HACK_GAME__MODEL__MODELS_H
#define HACK_GAME__MODEL__MODELS_H

#include "colored_model.h"

namespace hack_game {
	namespace models {

		extern ColoredModel	plane;    /// Прямоугольник от (-1, 0, -1) до (1, 0, 1)
		extern ColoredModel	platform; /// Прямоугольник от (0, 0, 0) до (1, 0, 1)
		extern ColoredModel	walls;    /// Стены

		extern ColoredModel	blackCube;       /// Чёрный куб
		extern ColoredModel	breakableCube;   /// Куб, который возможно сломать снарядами игрока
		extern ColoredModel	unbreakableCube; /// Куб, который невозможно сломать снарядами игрока
		
		extern ColoredModel	sphere;            /// Сфера, которая представляет Enemy
		extern ColoredModel	breakableSphere;   /// Снаряд Enemy, который может быть уничтожен снарядом игрока
		extern ColoredModel	unbreakableSphere; /// Снаряд Enemy, который не может быть уничтожен снарядом игрока
		extern ColoredModel	playerBullet;      /// Модель снаряда игрока
		extern ColoredModel	minion;            /// Модель Minion

		extern Model& player3hp; /// Модель игрока с 3 HP
		extern Model& player2hp; /// Модель игрока с 2 HP
		extern Model& player1hp; /// Модель игрока с 1 HP
		
		extern Model& cubeFrame;              /// Рамка куба
		extern Model& enemyDestroyBillboard;  /// Модель прямоугольника с текстурами для анимации уничтожения Enemy
		extern Model& minionDestroyBillboard; /// Модель прямоугольника с текстурами для анимации уничтожения Minion
		extern Model& postprocessingModel;    /// Модель прямоугольника для постпроцессинга
	}
}

#endif