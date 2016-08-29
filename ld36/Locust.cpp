#include "stdafx.h"
#include "Locust.h"
#include "SoundManager.h"

Locust::Locust(ResourceManager<sf::Texture, std::string>* resource_manager,
	EntityManager* entity_manager, sfld::Vector2f position, std::string sprite_name, float speed, Entity* player, int damage):player_(player),speed_(speed),cooldown_(0),sound_timer_(10000){
	constructEntity(resource_manager, sprite_name, entity_manager, position, false, SHAPE_SQUARE, DYNAMIC_MOVING, TYPE_LOCUST);
	health_ = 300;
}

void Locust::update(int frameTime) {
	sound_timer_ += frameTime;
	if (sound_timer_ >= 3000) {
		SoundManager::play("locusts");
		sound_timer_ = 0;
	}
	cooldown_ += frameTime;
	sfld::Vector2f dir = sfld::Vector2f(player_->getPosition() - getPosition()).normalise();
	move(dir, frameTime, speed_);
}

void Locust::collided(Entity* other) {
	if (other->getType() == TYPE_PLAYER && cooldown_ >= 1000) {
		other->damaged(10);
		other->setSpeed(0.05f);
		cooldown_ = 0;
	}
}
