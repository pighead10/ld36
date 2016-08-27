#pragma once

#include "BaseState.h"
#include "ResourceManager.h"
#include "PlayerInfo.h"

class EntityManager;
class Room;
class Player;

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
private:
	void sendData();
	void receiveData();
	void connectAndWait();

	void generateRooms();

	int player_no_;
	Player* player_;

	PlayerInfo old_data_;
	std::vector<PlayerInfo> player_infos_;
	sf::TcpSocket socket_;

	std::unique_ptr<EntityManager> entity_manager_;
	std::vector<std::unique_ptr<Room>> rooms_;
	ResourceManager<sf::Texture, std::string> resourceManager_;
};