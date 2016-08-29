#include "stdafx.h"
#include "SwordChest.h"
#include "BasicSword.h"]

SwordChest::SwordChest(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager, sfld::Vector2f position) {
	constructEntity(resource_manager, "wepchest", entity_manager, position, false, SHAPE_SQUARE, DYNAMIC_STATIC, TYPE_CHEST);
	awarded_ = false;
}

void SwordChest::doChest(Entity* player) {
	BasicSword* w = new BasicSword(player, entityManager_);
	player->setWeapon(w);
	destroy();
}