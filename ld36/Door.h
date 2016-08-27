#pragma once

#include "Entity.h"

class Door : public Entity {
public:
	Door(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager, sfld::Vector2f position);
	void open();
};