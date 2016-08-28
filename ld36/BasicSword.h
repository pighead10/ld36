#pragma once

#include "Weapon.h"

class EntityManager;
class Entity;

class BasicSword : public Weapon {
public:
	BasicSword(Entity* parent,EntityManager* entity_manager);
	void attack(sfld::Vector2f direction); //direction meaningless in this case
private:
	Entity* parent_;
	EntityManager* entity_manager_;
};