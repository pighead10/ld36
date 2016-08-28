#pragma once

#include "Entity.h"
#include "PlayerInfo.h"

class TrapChest : public Entity {
public:
	TrapChest(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager, sfld::Vector2f position, std::string sprite_name, MessageType message_type, std::string trap_name);
	~TrapChest();

	void collided(Entity* other);
private:
	std::string trap_name_;
	MessageType message_type_;
};