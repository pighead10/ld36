#pragma once

#include "stdafx.h"

class Weapon{
public:
	Weapon();
	~Weapon();

	virtual void attack(sfld::Vector2f direction);
	int getReloadTime() const;
protected:
	int reload_time_;
};