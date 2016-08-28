#include "stdafx.h"
#include "Trap.h"

Trap::Trap(std::string name) : name_(name) {}

Trap::~Trap() = default;

std::string Trap::getName() const{
	return name_;
}

void Trap::doTrap(int room_no) {
	//TODO do trap
	std::cout << "doing trap " << getName() << " in room " << room_no << std::endl;
}