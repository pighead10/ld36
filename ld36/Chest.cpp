#include "stdafx.h"
#include "Chest.h"
#include "SoundManager.h"

Chest::Chest() = default;
Chest::~Chest() = default;

void Chest::collided(Entity* other) {
	if (!awarded_) {
		if (other->getType() == TYPE_PLAYER) {
			awarded_ = true;
			SoundManager::play("pickup");
			doChest(other);
		}
	}
}

void Chest::doChest(Entity* player) {
	//...
}