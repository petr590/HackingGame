#include "models.h"
#include "frame_model.h"
#include "textured_model.h"
#include "composite_model.h"
#include "postprocessing_model.h"

namespace hack_game {
	namespace models {
		ColoredModel plane    (0xD00000, "plane.obj");
		ColoredModel platform (0xD0C8B5, "platform.obj");
		ColoredModel walls    (0xC0BAB1, "walls.obj");

		ColoredModel blackCube       (0x000000, "cube.obj");
		ColoredModel breakableCube   (0x41403B, blackCube);
		ColoredModel unbreakableCube (0xE0DAD1, blackCube);
		ColoredModel playerBullet (0xFFFFFF, "player-bullet.obj");

		ColoredModel sphere            (0x7A7876, "sphere.obj");
		ColoredModel breakableSphere   (0xF68118, sphere);
		ColoredModel unbreakableSphere (0x3E0065, sphere);

		ColoredModel minion (0x7d746e, "minion.obj");


		static ColoredModel playerBase   (0xE0DAD1, "player/base.obj");
		static ColoredModel playerCenter (0x41403B, "player/center.obj");
		static ColoredModel playerLeft   (0xE0DAD1, "player/left.obj");
		static ColoredModel playerRight  (0xE0DAD1, "player/right.obj");


		static CompositeModel compositeModels[] = {
				CompositeModel { &playerBase, &playerCenter, &playerLeft, &playerRight },
				CompositeModel { &playerBase, &playerCenter, &playerLeft },
				CompositeModel { &playerBase, &playerCenter },
		};

		static FrameModel frameModels[] = {
			FrameModel(0x000000, "cube-frame.obj"),
		};

		static TexturedModel texturedModels[] = {
			TexturedModel("plane.obj", {"enemy-destroy-1.png", "enemy-destroy-2.png", "enemy-destroy-3.png"}),
			TexturedModel("plane.obj", {"minion-destroy-1.png", "minion-destroy-2.png", "minion-destroy-3.png"}),
		};

		static PostprocessingModel postprocessingModels[] = {
			PostprocessingModel(),
		};



		Model& player3hp = compositeModels[0];
		Model& player2hp = compositeModels[1];
		Model& player1hp = compositeModels[2];

		Model& cubeFrame = frameModels[0];
		Model& enemyDestroyBillboard = texturedModels[0];
		Model& minionDestroyBillboard = texturedModels[1];
		Model& postprocessingModel = postprocessingModels[0];
	}
}