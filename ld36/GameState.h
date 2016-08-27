#pragma once

#include "BaseState.h"
#include "ResourceManager.h"

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
	void connectAndWait();

	void generateRooms();

	Player* player_;

	sf::TcpSocket socket_;

	std::unique_ptr<EntityManager> entity_manager_;
	std::vector<std::unique_ptr<Room>> rooms_;
	ResourceManager<sf::Texture, std::string> resourceManager_;
};