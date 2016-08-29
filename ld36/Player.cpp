#include "stdafx.h"
#include "Player.h"
#include "EntityManager.h"
#include "Weapon.h"
#include "ParticleEngine.h"
#include "SoundManager.h"

Player::Player(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager, sfld::Vector2f position):
cotm_(false),timer_(0),slow_timer_(0),slowed_(false),base_speed_(0.5f){
	constructEntity(resource_manager, "player", entity_manager, position, false, Entity::SHAPE_CIRCLE, Entity::DYNAMIC_MOVING, Entity::TYPE_PLAYER);
	speed_ = base_speed_;
}

void Player::setRoomNum(int num) {
	room_num_ = num;
}

int Player::getRoomNum() const {
	return room_num_;
}

void Player::doCotm() {
	//TODO display cursed indicator. Change sprite?
	cotm_ = true;
}

bool Player::getCotm() const {
	return cotm_;
}

void Player::setSpeed(float speed) {
	speed_ = speed;
	slowed_ = true;
	slow_timer_ = 0;
}

void Player::damaged(int amount) {
	SoundManager::play("takedamage");
	health_ -= amount;
	entityManager_->getParticleEngine()->generateBloodEffect(getPosition());
}

void Player::update(int frame_time) {
	using namespace sf;
	sfld::Vector2f dir(0, 0);

	timer_ += frame_time;

	if (slowed_) {
		slow_timer_ += frame_time;
		if (slow_timer_ >= 1000) {
			speed_ = base_speed_;
			slow_timer_ = 0;
			slowed_ = 0;
		}
	}

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
	if (Keyboard::isKeyPressed(Keyboard::Space)) {
		if (weapon_ != NULL) {
			if (timer_ >= weapon_->getReloadTime()) {
				weapon_->attack(sfld::Vector2f(0, 0)); //TODO change direction for future weapons
				timer_ = 0;
			}
		}
	}

	dir = dir.normalise();
	move(dir, frame_time, speed_);
	entityManager_->setViewFocus(this);
}