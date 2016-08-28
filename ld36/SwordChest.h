#pragma once

#include "Chest.h"
#include "PlayerInfo.h"

class SwordChest : public Chest {
public:
	SwordChest(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager, sfld::Vector2f position);
protected:
	void doChest(Entity* player);
};