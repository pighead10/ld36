#include "stdafx.h"
#include "Player.h"
#include "EntityManager.h"

Player::Player(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager, sfld::Vector2f position) : speed_(0.5f){
	constructEntity(resource_manager, "player", entity_manager, position, false, Entity::SHAPE_CIRCLE, Entity::DYNAMIC_MOVING, Entity::TYPE_PLAYER);
}

void Player::setRoomNum(int num) {
	room_num_ = num;
}

int Player::getRoomNum() const {
	return room_num_;
}

void Player::update(int frame_time) {
	using namespace sf;
	sfld::Vector2f dir(0, 0);

	if (Keyboard::isKeyPressed(Keyboard::W)) {
		dir.y -= 1;
	}
	if (Keyboard::isKeyPressed(Keyboard::S)) {
		dir.y += 1;
	}
	if (Keyboard::isKeyPressed(Keyboard::A)) {
		dir.x -= 1;
	}
	if (Keyboard::isKeyPressed(Keyboard::D)) {
		dir.x += 1;
	}

	dir = dir.normalise();
	move(dir, frame_time, speed_);
	entityManager_->setViewFocus(this);
}