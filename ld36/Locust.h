#pragma once

#include "Entity.h"

class Locust : public Entity {
public:
	Locust(ResourceManager<sf::Texture, std::string>* resource_manager,
		EntityManager* entity_manager, sfld::Vector2f position, std::string sprite_name, float speed, Entity* player, int damage);
	void collided(Entity* other);
	void update(int frame_time);
private:
	int sound_timer_;
	int cooldown_;
	float speed_;
	Entity* player_;
};