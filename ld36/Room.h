#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include "ResourceManager.h"
#include "Vector.h"

class EntityManager;
class Player;
class Entity;

class Room {
public:
	Room(sfld::Vector2f world_coords, int room_size, EntityManager* entity_manager, Player* player, ResourceManager<sf::Texture, std::string>* resource_manager);
	~Room();

	void setLit(bool lit);
	bool getLit() const;

	void add(Entity* entity, sfld::Vector2f local_position);

	void update(int frame_time);
private:
	sf::FloatRect room_area_;

	void generateRoom();

	EntityManager* entity_manager_;
	Player* player_;
	ResourceManager<sf::Texture, std::string>* resource_manager_;

	bool lit_;
	sfld::Vector2f world_coords_;
	int room_size_;

	std::vector<Entity*> entities_;
};