#include "walls.h"
#include "level/level.h"
#include "model/models.h"
#include <glm/gtc/matrix_transform.hpp>

namespace hack_game {
    Walls::Walls(Shader& shader) noexcept:
            SimpleEntity(shader, models::walls) {}
    
    glm::mat4 Walls::getModelTransform() const {
        return glm::scale(glm::mat4(1.0f), glm::vec3(TILE_SIZE));
    }
}