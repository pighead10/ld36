#include "stdafx.h"
#include "Trap.h"
#include "EntityManager.h"

Trap::Trap(std::string name, MessageType message_type, EntityManager* entity_manager) : name_(name), entity_manager_(entity_manager), message_type_(message_type) {}

Trap::~Trap() = default;

std::string Trap::getName() const{
	return name_;
}

void Trap::doTrap(int room_no) {
	entity_manager_->sendTrap(message_type_, room_no);
}