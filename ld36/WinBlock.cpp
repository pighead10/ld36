#include "stdafx.h"
#include "WinBlock.h"
#include "EntityManager.h"

WinBlock::WinBlock(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager, sfld::Vector2f position) {
	constructEntity(resource_manager, "treasure", entity_manager, position, false, SHAPE_SQUARE, DYNAMIC_STATIC, TYPE_WIN);
}

void WinBlock::doChest(Entity* player) {
	entityManager_->won();
}