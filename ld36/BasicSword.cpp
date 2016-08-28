#include "stdafx.h"
#include "BasicSword.h"
#include "EntityManager.h"
#include "Entity.h"

BasicSword::BasicSword(Entity* parent,EntityManager* entity_manager) :parent_(parent), entity_manager_(entity_manager) {
	reload_time_ = 2000;
}

void BasicSword::attack(sfld::Vector2f direction) {
	std::cout << "sword attack" << std::endl;
	//TODO play 'animation'
	EntityList* list = entity_manager_->getEntities();
	for (auto& it : *list) {
		if (it.get() != parent_ && sfld::Vector2f(it->getPosition() - parent_->getPosition()).length() < TILE_SIZE*1.5f) {
			it->damaged(50);
			std::cout << "sword damaged enemy" << std::endl;
		}
	}
}