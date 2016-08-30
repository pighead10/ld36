#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include "ResourceManager.h"
#include "Vector.h"
#include "PlayerInfo.h"

class EntityManager;
class Player;
class Entity;
class Door;

struct DoorConditions {
	DoorConditions(int p, int r) : room(r), players(p) {}
	int room;
	int players;
};

typedef std::vector<DoorConditions> DoorConditionsList;

class Room {
public:
	enum DoorPosition {
		DOOR_TOP,
		DOOR_LEFT,
		DOOR_RIGHT,
		DOOR_BOT
	};

	enum DoorOrientation {
		ORIENTATION_TOP,
		ORIENTATION_LEFT,
		ORIENTATION_RIGHT,
		ORIENTATION_BOT,
		ORIENTATION_NONE,
		ORIENTATION_TOPLEFT,
		ORIENTATION_TOPRIGHT,
		ORIENTATION_BOTLEFT,
		ORIENTATION_BOTRIGHT,
	};

	Room(int room_num, sfld::Vector2f world_coords, int room_size, EntityManager* entity_manager, Player* player,
		ResourceManager<sf::Texture, std::string>* resource_manager, std::vector<PlayerInfo>* player_infos,DoorOrientation orientation);
	~Room();

	void setLit(bool lit);
	bool getLit() const;

	void add(Entity* entity, sfld::Vector2f local_position);
	void addDoor(DoorPosition position, DoorConditionsList conditions);

	sf::Text getRoomText() const;

	void update(int frame_time);
	void playerTouchedWall(Entity* door);
	void renderBackground(sf::RenderTarget* target);

	void doCotm(int frame_time);
	void setCotmReady();

	bool containsDoors() const;
	void addCotp();
	void render(sf::RenderTarget* target);

	void addLocust();
	int getRoomNum() const;

	void addHealthPack();
private:
	bool cotp_;
	bool cotp_started_;
	int cotp_timer_;
	sf::Text cotp_text_;
	sf::Sprite cotp_spr_;

	int cotm_it;
	int cotm_timer;
	bool cotm_ready;

	DoorOrientation orientation_;
	DoorConditionsList getConditionsFromDoor(Entity* door);

	sf::Text room_text_;
	sf::FloatRect room_area_;

	void generateRoom();

	EntityManager* entity_manager_;
	Player* player_;
	ResourceManager<sf::Texture, std::string>* resource_manager_;

	bool lit_;
	sfld::Vector2f world_coords_;
	int room_size_;
	int room_num_;
	std::vector<PlayerInfo>* player_infos_;

	std::vector<Entity*> entities_;
	std::vector<std::pair<Door*, DoorConditionsList>> doors_;

	int midx_;
	int midy_;
	sf::Texture background_;
};