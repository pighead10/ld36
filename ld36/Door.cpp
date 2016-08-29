#include "stdafx.h"
#include "Door.h"
#include "Room.h"

Door::Door(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager, sfld::Vector2f position, bool open, Room* parent):parent_(parent), open_(open){
	constructEntity(resource_manager, "door_closed", entity_manager, position, false, Entity::SHAPE_SQUARE, Entity::DYNAMIC_STATIC, Entity::TYPE_DOOR);
	setOpen(open);
}

bool Door::isOpen() const {
	return open_;
}

void Door::collided(Entity* entity) {
	if (entity->getType() == TYPE_PLAYER) {
		parent_->playerTouchedWall(this);
	}
}

void Door::setOpen(bool open) {
	open_ = open;
	if (open) {
		sprite_.setTexture(resourceManager_->get("door_open"));
		setWalkthrough(true);
	}
	else {
		sprite_.setTexture(resourceManager_->get("door_closed"));
		setWalkthrough(false);
	}
}

/*
-Server generates list of moves that win the game (e.g. 1 goes to 2, 3 goes to 2, 4 goes to 5, ...)
-Client generates sets of doors that match those instructions

*/