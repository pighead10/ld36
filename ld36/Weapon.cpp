#include "stdafx.h"
#include "Weapon.h"

Weapon::Weapon() = default;
Weapon::~Weapon() = default;

void Weapon::attack(sfld::Vector2f direction) {
	//attack...
}

int Weapon::getReloadTime() const {
	return reload_time_;
}