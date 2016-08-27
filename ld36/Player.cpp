#include "stdafx.h"
#include "Player.h"

Player::Player(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager, sfld::Vector2f position) : speed_(0.1f){
	constructEntity(resource_manager, "player", entity_manager, position, false, Entity::SHAPE_CIRCLE, Entity::DYNAMIC_MOVING, Entity::TYPE_PLAYER);
}

void Player::update(int frame_time) {
	using namespace sf;
	sfld::Vector2f dir(0, 0);

	if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up)) {
		dir.y -= 1;
	}
	if (Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down)) {
		dir.y += 1;
	}
	if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left)) {
		dir.x -= 1;
	}
	if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right)) {
		dir.x += 1;
	}

	dir = dir.normalise();
	move(dir, frame_time, speed_);
}