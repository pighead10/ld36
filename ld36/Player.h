#pragma once

#include "Entity.h"

class Player : public Entity {
public:
	Player(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager, sfld::Vector2f position);
	void update(int frame_time);
private:
	float speed_;
};