#pragma once

#include "Chest.h"

class EyeChest : public Chest {
public:
	EyeChest(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager, sfld::Vector2f position);
protected:
	void doChest(Entity* player);
};