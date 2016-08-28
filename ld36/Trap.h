#pragma once

#include "stdafx.h"
#include "PlayerInfo.h"

class EntityManager;

class Trap {
public:
	Trap(std::string name, MessageType message_type, EntityManager* entity_manager);
	~Trap();

	std::string getName() const;
	void doTrap(int room_no);
private:
	EntityManager* entity_manager_;
	MessageType message_type_;
	std::string name_;
};