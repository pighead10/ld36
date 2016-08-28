#pragma once

#include "Entity.h"

class Chest : public Entity{
public:
	Chest();
	~Chest();
	void collided(Entity* other);
protected:
	virtual void doChest(Entity* player);

};