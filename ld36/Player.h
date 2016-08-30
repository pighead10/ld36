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
	void damaged(int amount);
	void setSpeed(float speed);
	void gainHealth(int amount);

	void render(sf::RenderTarget* target);
	void setWeapon(Weapon* weapon);

private:
	sf::Sprite animation_spr_;
	int animation_timer_;
	bool animation_;

	float base_speed_;
	int timer_;
	int slow_timer_;
	bool slowed_;

	bool cotm_;
	int room_num_;
	float speed_;
};