#include "stdafx.h"
#include "Eye.h"
#include "EntityManager.h"

Eye::Eye(EntityManager* entity_manager) {
	entity_manager_ = entity_manager;
	name_ = "Ra's Eye";
}

void Eye::doTrap(int room_no) {
	std::cout << "displaying pfino" << std::endl;
	entity_manager_->displayPinfo();
}