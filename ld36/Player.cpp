#include "stdafx.h"
#include "Player.h"
#include "EntityManager.h"
#include "Weapon.h"
#include "ParticleEngine.h"
#include "SoundManager.h"
#include "Collision.h"

Player::Player(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager, sfld::Vector2f position):
cotm_(false),timer_(0),slow_timer_(0),slowed_(false),base_speed_(0.3f),animation_timer_(0),animation_(false){
	constructEntity(resource_manager, "player", entity_manager, position, false, Entity::SHAPE_CIRCLE, Entity::DYNAMIC_MOVING, Entity::TYPE_PLAYER);
	health_ = 100;
	entityManager_->changeHealth(100);
	speed_ = base_speed_;
	animation_spr_.setTexture(resourceManager_->get("player_wep_attack"));
	sfld::Vector2f local_centre(animation_spr_.getLocalBounds().width / 2, animation_spr_.getLocalBounds().height / 2);
	animation_spr_.setOrigin(local_centre);
}

void Player::setRoomNum(int num) {
	room_num_ = num;
}

int Player::getRoomNum() const {
	return room_num_;
}

void Player::doCotm() {
	cotm_ = true;
	if (weapon_ != NULL) {
		sprite_.setTexture(resourceManager_->get("curseplayer_wep"));
	}
	else {
		sprite_.setTexture(resourceManager_->get("curseplayer"));
	}
	animation_spr_.setTexture(resourceManager_->get("curseplayer_wep_attack"));
	sfld::Vector2f local_centre(animation_spr_.getLocalBounds().width / 2, animation_spr_.getLocalBounds().height / 2);
	animation_spr_.setOrigin(local_centre);
	centreOrigin();
	sprite_.setPosition(getPosition());
}

bool Player::getCotm() const {
	return cotm_;
}

void Player::setSpeed(float speed) {
	speed_ = speed;
	slowed_ = true;
	slow_timer_ = 0;
}

void Player::setWeapon(Weapon* weapon) {
	weapon_ = weapon;
	sprite_.setTexture(resourceManager_->get("player_wep"));
	centreOrigin();
	sprite_.setPosition(getPosition());
}

void Player::damaged(int amount) {
	SoundManager::play("takedamage");
	health_ -= amount;
	entityManager_->changeHealth(health_);
	entityManager_->getParticleEngine()->generateBloodEffect(getPosition());
}

void Player::render(sf::RenderTarget* target) {
	if (health_ > 0) {
		if (!animation_) {
			target->draw(sprite_);
		}
		else {
			target->draw(animation_spr_);
		}
	}
}

void Player::gainHealth(int amount) {
	health_ += amount;
	entityManager_->changeHealth(health_);
	if (health_ > 100) health_ = 100;
	SoundManager::play("coinrattle");
}

void Player::update(int frame_time) {
	using namespace sf;
	if (health_ > 0) {
		if (animation_) {
			animation_spr_.setPosition(getPosition());
			animation_timer_ += frame_time;
			if (animation_timer_ <= 400) {
				rotating_ = true;
				animation_spr_.rotate(20); // setRotation(maths::toDegrees(atan2(direction.y, direction.x)) - 90);
			}
			else {
				rotating_ = false;
				animation_timer_ = 0;
				animation_ = false;
			}
		}

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
					animation_ = true;
					rotating_ = true;
					weapon_->attack(sfld::Vector2f(0, 0)); //TODO change direction for future weapons
					timer_ = 0;
				}
			}
		}

		dir = dir.normalise();
		move(dir, frame_time, speed_);
		entityManager_->setViewFocus(this);
	}
}