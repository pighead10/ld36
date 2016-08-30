#pragma once

#include "Trap.h"

class Eye : public Trap {
public:
	Eye(EntityManager* entity_manager);
	void doTrap(int room_no);
};