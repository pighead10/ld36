#include "stdafx.h"
#include "TrapChest.h"
#include "Trap.h"
#include "EntityManager.h"

TrapChest::TrapChest(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager,
	sfld::Vector2f position, std::string sprite_name, MessageType message_type, std::string trap_name):
	trap_name_(trap_name), message_type_(message_type){
	constructEntity(resource_manager, sprite_name, entity_manager, position, false, SHAPE_SQUARE, DYNAMIC_STATIC, TYPE_CHEST);
	awarded_ = false;
}

TrapChest::~TrapChest() = default;

void TrapChest::doChest(Entity* player) {
	Trap* trap = new Trap(trap_name_, message_type_, entityManager_);
	entityManager_->addTrap(trap);
	destroy();
}