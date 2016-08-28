#pragma once

#include "stdafx.h"

class Trap {
public:
	explicit Trap(std::string name);
	~Trap();

	std::string getName() const;
	void doTrap(int room_no);
private:
	std::string name_;
};