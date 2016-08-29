#pragma once

#include "BaseState.h"
#include "ResourceManager.h"
#include "PlayerInfo.h"
#include "Room.h"
#include "TrapInferface.h"

class EntityManager;
class Player;
class ParticleEngine;

struct ListCondition {
	ListCondition(int p, int r):room_no(r),player_no(p){}
	int room_no;
	int player_no;
};

typedef std::vector<ListCondition> ConditionList;
typedef std::vector<ConditionList> SolutionList;


class GameState : public BaseState{
public:
	GameState();
	~GameState();
	void sfmlEvent(sf::Event evt);
	void start();
	void pause();
	void resume();
	void exit();

	void update(int frameTime);
	void render(sf::RenderTarget* target);

	void sendTrap(MessageType type, int room_no);
	Room* getRoom(int n);

	void won();
private:
	bool game_ended_;

	void sendData();
	void receiveData();
	void connectAndWait();

	void generateRooms(int room_root);
	void generateMap(int max_rooms);
	void generateDoor(DoorConditionsList condition, int room);

	int player_no_;
	Player* player_;

	int room_size_;
	int spawn_room_;
	int row_size_;


	PlayerInfo old_data_;
	std::vector<PlayerInfo> player_infos_;
	sf::TcpSocket socket_;

	std::unique_ptr<EntityManager> entity_manager_;
	std::vector<std::unique_ptr<Room>> rooms_;
	ResourceManager<sf::Texture, std::string> resourceManager_;

	SolutionList solution_;
	std::unique_ptr<TrapInterface> trap_interface_;

	std::unique_ptr<ParticleEngine> particle_engine_;

	sf::Vector2i roomNumToCoord(int n) const;
	int coordToRoomNum(sf::Vector2i coord) const;
};