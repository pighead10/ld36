#pragma once

#include "Entity.h"

class StaticObj : public Entity {
public:
	StaticObj(ResourceManager<sf::Texture, std::string>* resourceManager, std::string sprite_name, EntityManager* entity_manager, 
			sfld::Vector2f position, ENTITY_SHAPE shape, ENTITY_TYPE type);
};