#pragma once

#include "Entity.h"

class Room;

class Door : public Entity {
public:
	Door(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager, sfld::Vector2f position, bool open, Room* parent);
	void setOpen(bool open);
	bool isOpen() const;
	void collided(Entity* other);
private:
	bool open_;
	Room* parent_;
};