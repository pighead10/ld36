#include "stdafx.h"
#include "HealthChest.h"

HealthChest::HealthChest(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager, sfld::Vector2f position) {
	constructEntity(resource_manager, "healthchest", entity_manager, position, false, SHAPE_SQUARE, DYNAMIC_STATIC, TYPE_CHEST);
	awarded_ = false;
}

void HealthChest::doChest(Entity* player) {
	player->gainHealth(40);
	destroy();
}