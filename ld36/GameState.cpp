#include "stdafx.h"
#include "GameState.h"
#include "StateManager.h"
#include "SoundManager.h"
#include "StaticObj.h"
#include "EntityManager.h"
#include "Player.h"
#include "Room.h"
#include <fstream>

sf::Packet& operator << (sf::Packet& packet, const PlayerInfo& m)
{
	return packet << m.msg_type <<m.player_no << m.room_no << m.name;
}
sf::Packet& operator >> (sf::Packet& packet, PlayerInfo& m)
{
	return packet >> m.msg_type >> m.player_no >> m.room_no >> m.name;
}

GameState::GameState() = default;
GameState::~GameState() = default;

void GameState::sfmlEvent(sf::Event evt){
	if (evt.type == sf::Event::Closed || (evt.type == sf::Event::KeyPressed && evt.key.code == sf::Keyboard::Escape)) {
		getParent().pop();
	}
	else if (evt.type == sf::Event::KeyPressed && evt.key.code == sf::Keyboard::T) {
		//temp: Press T to detonate red trap in room 1
		sendTrap(MESSAGE_TRAP_RED, 1);
	}
}

void GameState::sendTrap(MessageType type, int room_no) {
	PlayerInfo info;
	info.name = "test"; //todo change to actual name
	info.msg_type = type;
	info.room_no = room_no;
	info.player_no = player_no_;
	sf::Packet s;
	s << info;
	socket_.send(s);
}

void GameState::connectAndWait() {
	std::string ip;
	std::string port;
	std::ifstream fin("serverinfo.txt");
	fin >> ip;
	fin >> port;

	//Connect to server and wait for start signal.
	socket_.connect(ip, stoi(port));
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
	entity_manager_ = std::unique_ptr<EntityManager>(new EntityManager(&resourceManager_, &player_infos_));

	resourceManager_.setDirectory("media/images/");
	resourceManager_.load("player", "pig.png");
	resourceManager_.load("wall", "wall.png");
	resourceManager_.load("door_closed", "door_closed.png");
	resourceManager_.load("door_open", "door_open.png");

	player_ = new Player(&resourceManager_, entity_manager_.get(), sfld::Vector2f(50, 50));
	entity_manager_->addEntity(player_);

	int room_root = 10;
	generateRooms(room_root);
	rooms_[0]->add(new StaticObj(&resourceManager_, "wall", entity_manager_.get(), sfld::Vector2f(0, 0), Entity::SHAPE_SQUARE, Entity::TYPE_WALL), sfld::Vector2f(50, 50));
	rooms_[2]->add(new StaticObj(&resourceManager_, "wall", entity_manager_.get(), sfld::Vector2f(0, 0), Entity::SHAPE_SQUARE, Entity::TYPE_WALL), sfld::Vector2f(50, 50));

	connectAndWait();
	//player_no_ = 3;
	generateMap(room_root*room_root);
}

void GameState::pause(){	
}

void GameState::resume(){
}

void GameState::exit(){
}

void GameState::generateDoor(DoorConditionsList condition, int room) {
	//Generate all doors on room
	for (int i = 0; i < 4; i++) {
		Room::DoorPosition pos = static_cast<Room::DoorPosition>(i);
		rooms_[room]->addDoor(pos, condition);
	}
}

void GameState::generateMap(int max_rooms) {
	//test solution
	ListCondition c1(1, 10);
	ListCondition c2(2, 11);
	ConditionList l1 = { c1 , c2};
	ListCondition c3(0, 4);
	ConditionList l2 = { c3 };
	ListCondition c4(1, 5);
	ListCondition c5(2, 3);
	ConditionList l3 = { c4, c5 };
	solution_.push_back(l1);
	solution_.push_back(l2);
	solution_.push_back(l3);

	for (int i = 1; i < solution_.size(); i++) {
		ConditionList list = solution_[i];
		for (auto& cond : list) {
			if (cond.player_no == player_no_) {
				ConditionList prev_list = solution_[i - 1];
				std::vector<int> players(max_rooms, 0);
				for (auto& prev : prev_list) {
					players[prev.room_no]++;
				}
				DoorConditionsList list;
				for (int n = 0; n < players.size(); n++) {
					if (players[n] != 0) {
						list.push_back(DoorConditions(players[n], n));
					}
				}
				generateDoor(list, cond.room_no);
			}
		}
	}
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
	for (auto& it : rooms_) {
		target->draw(it->getRoomText());
	}
	entity_manager_->render(target);
}

void GameState::generateRooms(int room_root) {
	int room_size = 16*TILE_SIZE;
	int room_num = 0;
	for (int x = 0; x < room_root; x++) {
		for (int y = 0; y < room_root; y++) {
			rooms_.push_back(std::unique_ptr<Room>(new Room(room_num, sfld::Vector2f(x*room_size, y*room_size),
				room_size, entity_manager_.get(), player_, &resourceManager_,&player_infos_)));
			room_num++;
		}
	}
}

void GameState::sendData() {
	PlayerInfo info;
	info.msg_type = MESSAGE_INFO;
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
		if (info.msg_type == MESSAGE_INFO) {
			if (info.player_no >= 0 && info.player_no < player_infos_.size()) {
				std::cout << "Client received info: Player number: " << info.player_no << " Name: " << info.name << " Room: " << info.room_no << std::endl;
				player_infos_[info.player_no].name = info.name;
				player_infos_[info.player_no].room_no = info.room_no;
			}
			else {
				std::cout << "WTF? Received player info num: " << info.player_no << std::endl;
			}
		}
		else {
			player_infos_.push_back(info);
		}
	}
}

/*
Feature list to create next:
-Display room numbers X
-Display conditions for doors opening
-Add chests with give you the ability to place traps or gain weapons
-First trap to add: Curse trap, zombies spawn in rooms you are in but don't follow you to other rooms.
-Add weapon
-Then add randomly spawning enemies in rooms sometimes
-At the end of the day, make the random generation thing, with a ending. Ends after last step in certain room.
*/

