#include "engine/creature/creature.hpp"
#include <vector>

// TODO: Pass vertices and construct meshess here
Creature::Creature(std::vector<std::unique_ptr<Mesh>> meshes) {
  meshess = std::move(meshes);
}

void Creature::Draw(Shader &shader) const {
  for (const auto &mesh : meshess)
    mesh->Draw(shader);
}
