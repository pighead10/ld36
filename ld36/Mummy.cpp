#include "stdafx.h"
#include "Mummy.h"
#include "EntityManager.h"
#include "ParticleEngine.h"
#include "SoundManager.h"

Mummy::Mummy(ResourceManager<sf::Texture, std::string>* resource_manager, 
	EntityManager* entity_manager, sfld::Vector2f position,std::string sprite_name,float speed,Entity* player, int damage)
	:speed_(speed),player_(player),damage_(damage),sound_timer_(100000){
	constructEntity(resource_manager, "mummy", entity_manager, position, false, SHAPE_CIRCLE, DYNAMIC_MOVING, TYPE_MUMMY);
	health_ = 100;
}

void Mummy::damaged(int amount) {
	health_ -= amount;
	entityManager_->getParticleEngine()->generateBloodEffect(getPosition());
	SoundManager::play("takedamage2");
}

void Mummy::destroy() {
	destroyed_ = true;
	SoundManager::play("dienoise");
}

void Mummy::update(int frameTime) {
	sound_timer_ += frameTime;
	if (sound_timer_ >= 3000) {
		SoundManager::play("mummy");
		sound_timer_ = 0;
	}
	sfld::Vector2f dir = sfld::Vector2f(player_->getPosition() - getPosition()).normalise();
	move(dir, frameTime, speed_);
}

void Mummy::collided(Entity* other) {
	if (other->getType() == TYPE_PLAYER) {
		other->damaged(damage_);
		destroy();
	}
}