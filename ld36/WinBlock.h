#pragma once

#include "Chest.h"

class WinBlock : public Chest {
public:
	WinBlock(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager, sfld::Vector2f position);
protected:
	void doChest(Entity* player);
};