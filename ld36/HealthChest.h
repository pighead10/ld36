#pragma once

#include "Chest.h"

class HealthChest : public Chest{
public:
	HealthChest(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager, sfld::Vector2f position);
protected:
	void doChest(Entity* player);
};