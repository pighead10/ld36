#pragma once

#include "stdafx.h"
#include "PlayerInfo.h"

class EntityManager;

class Trap {
public:
	Trap();
	Trap(std::string name, MessageType message_type, EntityManager* entity_manager);
	~Trap();

	std::string getName() const;
	virtual void doTrap(int room_no);
protected:
	std::string name_;
	EntityManager* entity_manager_;
private:
	
	MessageType message_type_;
};