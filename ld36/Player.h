#pragma once

#include "Entity.h"

class Player : public Entity {
public:
	Player(ResourceManager<sf::Texture, std::string>* resource_manager, EntityManager* entity_manager, sfld::Vector2f position);
	void update(int frame_time);
	void setRoomNum(int num);
	int getRoomNum() const;
	void doCotm();
	bool getCotm() const;
private:
	bool cotm_;
	int room_num_;
	float speed_;
};