#include "stdafx.h"
#include "GameState.h"
#include "StateManager.h"
#include "SoundManager.h"
#include "StaticObj.h"
#include "EntityManager.h"
#include "Player.h"
#include "Room.h"

GameState::GameState() = default;
GameState::~GameState() = default;


sf::Packet& operator << (sf::Packet& packet, const PlayerInfo& m)
{
	return packet << m.player_no << m.room_no << m.name;
}
sf::Packet& operator >> (sf::Packet& packet, PlayerInfo& m)
{
	return packet >> m.player_no >> m.room_no >> m.name;
}

void GameState::sfmlEvent(sf::Event evt){
	if (evt.type == sf::Event::Closed || (evt.type == sf::Event::KeyPressed && evt.key.code == sf::Keyboard::Escape)){
		getParent().pop();
	}
}

void GameState::connectAndWait() {
	//Connect to server and wait for start signal.
	socket_.connect("localhost", 5337);
	std::cout << "Connected to server. Waiting for start signal..." << std::endl;
	sf::Packet rec;
	socket_.receive(rec);
	int total;
	rec >> player_no_ >> total;
	std::cout << "Start signal received. You are player number " << player_no_ << ". There are " << total << " players overall." << std::endl;
	//Set up player infos.
	for (int i = 0; i < total; i++) {
		player_infos_.push_back(PlayerInfo());
	}
	socket_.setBlocking(false);
}

void GameState::start(){
	entity_manager_ = std::unique_ptr<EntityManager>(new EntityManager(&resourceManager_));

	resourceManager_.setDirectory("media/images/");
	resourceManager_.load("player", "pig.png");
	resourceManager_.load("wall", "wall.png");

	player_ = new Player(&resourceManager_, entity_manager_.get(), sfld::Vector2f(50, 50));
	entity_manager_->addEntity(player_);

	generateRooms();
	rooms_[0]->add(new StaticObj(&resourceManager_, "wall", entity_manager_.get(), sfld::Vector2f(0, 0), Entity::SHAPE_SQUARE, Entity::TYPE_WALL), sfld::Vector2f(50, 50));
	rooms_[2]->add(new StaticObj(&resourceManager_, "wall", entity_manager_.get(), sfld::Vector2f(0, 0), Entity::SHAPE_SQUARE, Entity::TYPE_WALL), sfld::Vector2f(50, 50));

	connectAndWait();
}

void GameState::pause(){	
}

void GameState::resume(){
}

void GameState::exit(){
}

void GameState::update(int frameTime){
	entity_manager_->update(frameTime);
	for (auto& it : rooms_) {
		it->update(frameTime);
	}
	sendData();
	receiveData();
	//output all data
	//std::cout << "Networked info of all players:" << std::endl;
	for (auto& it : player_infos_) {
		//std::cout << "Num: " << it.player_no << " Name: " << it.name << " Room number: " << it.room_no << std::endl;
	}
}

void GameState::render(sf::RenderTarget* target){
	//TODO: only render things on the player's screen so it won't lag to shit
	entity_manager_->render(target);
}

void GameState::generateRooms() {
	int room_root = 2; //so there are room_root^2 rooms total, so it can be arranged in a square
	int room_size = 8*TILE_SIZE;
	int room_num = 0;
	for (int x = 0; x < room_root; x++) {
		for (int y = 0; y < room_root; y++) {
			rooms_.push_back(std::unique_ptr<Room>(new Room(room_num, sfld::Vector2f(x*room_size, y*room_size),
				room_size, entity_manager_.get(), player_, &resourceManager_)));
			room_num++;
		}
	}
}

void GameState::sendData() {
	
	PlayerInfo info;
	info.name = "test";
	info.room_no = player_->getRoomNum();
	info.player_no = player_no_;
	if (old_data_.room_no != info.room_no) {
		old_data_.room_no = info.room_no;
		sf::Packet s;
		std::cout << "Sending info: Name " << info.name << " Room: " << info.room_no << " Player: " << info.player_no << std::endl;
		s << info;
		socket_.send(s);
	}
	
}

void GameState::receiveData() {
	sf::Packet rec;
	if (socket_.receive(rec) == sf::Socket::Done) {
		PlayerInfo info;
		rec >> info;
		if (info.player_no >= 0 && info.player_no < player_infos_.size()) {
			std::cout << "Client received info: Player number: " << info.player_no << " Name: " << info.name << " Room: " << info.room_no << std::endl;
			player_infos_[info.player_no].name = info.name;
			player_infos_[info.player_no].room_no = info.room_no;
		}
		else {
			std::cout << "WTF? Received player info num: " << info.player_no << std::endl;
		}
	}
}