#include "stdafx.h"
#include "EyeChest.h"
#include "EntityManager.h"

EyeChest::EyeChest(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager, sfld::Vector2f position) {
	constructEntity(resource_manager, "eye", entity_manager, position, false, SHAPE_SQUARE, DYNAMIC_STATIC, TYPE_CHEST);
	awarded_ = false;
}

void EyeChest::doChest(Entity* player) {
	entityManager_->addEye();
	destroy();
}