#pragma once

#include "Entity.h"

class Mummy : public Entity {
public:
	Mummy(ResourceManager<sf::Texture, std::string>* resource_manager,
		EntityManager* entity_manager, sfld::Vector2f position, std::string sprite_name, float speed, Entity* player,int damage);
	void update(int frameTime);
	void collided(Entity* entity);
	void damaged(int amount);
protected:
	void destroy();
private:
	int sound_timer_;

	int damage_;
	float speed_;
	Entity* player_;
};