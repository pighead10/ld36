#include "stdafx.h"
#include "Chest.h"

Chest::Chest() = default;
Chest::~Chest() = default;

void Chest::collided(Entity* other) {
	if (other->getType() == TYPE_PLAYER) {
		doChest(other);
	}
}

void Chest::doChest(Entity* player) {
	//...
}