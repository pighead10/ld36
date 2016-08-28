#include "stdafx.h"
#include "Mummy.h"

Mummy::Mummy(ResourceManager<sf::Texture, std::string>* resource_manager, 
	EntityManager* entity_manager, sfld::Vector2f position,std::string sprite_name,float speed,Entity* player, int damage)
	:speed_(speed),player_(player),damage_(damage){
	constructEntity(resource_manager, "mummy", entity_manager, position, false, SHAPE_CIRCLE, DYNAMIC_MOVING, TYPE_MUMMY);
	health_ = 100;
}

void Mummy::update(int frameTime) {
	sfld::Vector2f dir = sfld::Vector2f(player_->getPosition() - getPosition()).normalise();
	move(dir, frameTime, speed_);
}

void Mummy::collided(Entity* other) {
	if (other->getType() == TYPE_PLAYER) {
		other->damaged(damage_);
	}
}