#include "stdafx.h"
#include "GameState.h"
#include "StateManager.h"
#include "SoundManager.h"
#include "StaticObj.h"
#include "EntityManager.h"
#include "Player.h"
#include "Room.h"
#include "Trap.h"
#include "TrapChest.h"
#include "Mummy.h"
#include "SwordChest.h"
#include "WinBlock.h"
#include "ParticleEngine.h"
#include "EyeChest.h"
#include <fstream>

sf::Packet& operator << (sf::Packet& packet, const PlayerInfo& m)
{
	return packet << m.msg_type << m.player_no << m.room_no << m.name;
}
sf::Packet& operator >> (sf::Packet& packet, PlayerInfo& m)
{
	return packet >> m.msg_type >> m.player_no >> m.room_no >> m.name;
}

GameState::GameState() = default;
GameState::~GameState() = default;

Room* GameState::getRoom(int n) {
	return rooms_[n].get();
}

void GameState::sfmlEvent(sf::Event evt){
	if (evt.type == sf::Event::Closed) {
		getParent().pop();
	}

	if (evt.type == sf::Event::KeyPressed) {
		trap_interface_->keyPressed(evt.key.code);
	}
}

void GameState::sendTrap(MessageType type, int room_no) {
	PlayerInfo info;
	info.name = player_name_; //todo change to actual name
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
	fin >> player_name_;

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

	sf::Packet sp;
	socket_.receive(sp);
	sp >> spawn_room_;
	std::cout << "SPAWNING AT " << spawn_room_ << std::endl;

	socket_.setBlocking(false);

}

void GameState::start(){
	game_ended_ = false;
	row_size_ = 12;
	room_size_ = 16 * TILE_SIZE;

	particle_engine_ = std::unique_ptr<ParticleEngine>(new ParticleEngine());
	
	int max_rooms = room_size_*room_size_;
	entity_manager_ = std::unique_ptr<EntityManager>(new EntityManager(&resourceManager_, &player_infos_, this, room_size_, particle_engine_.get()));

	pinfo_text_.setFont(*entity_manager_->getFont());
	pinfo_ = false;

	trap_interface_ = std::unique_ptr<TrapInterface>(new TrapInterface(entity_manager_.get(),max_rooms));
	entity_manager_->setTrapInterface(trap_interface_.get());

	resourceManager_.setDirectory("media/images/");
	resourceManager_.load("player", "player.png");
	resourceManager_.load("wall", "wall.png");
	resourceManager_.load("door_closed", "door_closed.png");
	resourceManager_.load("door_open", "door_open.png");
	resourceManager_.load("trapchest", "trapchest.png");
	resourceManager_.load("mummy", "mummy.png");
	resourceManager_.load("wepchest", "wepchest.png");
	resourceManager_.load("treasure", "treasure.png");
	resourceManager_.load("cotp", "cotp.png");
	resourceManager_.load("locust", "locust.png");
	resourceManager_.load("player_wep", "player_wep.png");
	resourceManager_.load("curseplayer_wep", "curseplayer_wep.png");
	resourceManager_.load("curseplayer_wep_attack", "curseplayer_wep_attack.png");
	resourceManager_.load("player_wep_attack", "player_wep_attack.png");
	resourceManager_.load("curseplayer", "curseplayer.png");
	resourceManager_.load("healthup", "healthup.png");
	resourceManager_.load("healthchest", "healthchest.png");
	resourceManager_.load("eye", "eye.png");

	SoundManager::add("angryzombie", "media/sound/AngryZombie.ogg");
	SoundManager::add("coinrattle", "media/sound/CoinRattle.ogg");
	SoundManager::add("dienoise", "media/sound/DieNoise.ogg");
	SoundManager::add("dooropen", "media/sound/DoorOpen.ogg");
	SoundManager::add("explosionfuture", "media/sound/ExplosionFuture.ogg");
	SoundManager::add("fire", "media/sound/Fire.ogg");
	SoundManager::add("growl", "media/sound/Growl.ogg");
	SoundManager::add("locusts", "media/sound/Locusts.ogg");
	SoundManager::add("metalhit", "media/sound/MetalHit.ogg");
	SoundManager::add("mummy", "media/sound/Mummy.ogg");
	SoundManager::add("pickup", "media/sound/PickUp.ogg");
	SoundManager::add("rockfalling", "media/sound/RockFalling.ogg");
	SoundManager::add("rockhit", "media/sound/RockHit.ogg");
	SoundManager::add("scaryscream", "media/sound/ScaryScream.ogg");
	SoundManager::add("swordswing", "media/sound/SwordSwing.ogg");
	SoundManager::add("takedamage", "media/sound/TakeDamage.ogg");
	SoundManager::add("takedamage2", "media/sound/TakeDamage2.ogg");
	SoundManager::add("trap", "media/sound/Trap.ogg");
	SoundManager::add("win", "media/sound/Win.ogg");
	SoundManager::add("zipping", "media/sound/Zipping.ogg");
	SoundManager::add("lost", "media/sound/Lost.ogg");

	connectAndWait();
	//spawn_room_ = 0;
	//player_no_ = 3;
	player_ = new Player(&resourceManager_, entity_manager_.get(), sfld::Vector2f(0, 0));
	generateRooms(row_size_);
	generateMap(max_rooms);

	rooms_[spawn_room_]->add(player_, sfld::Vector2f(100, 100));
}

void GameState::pause(){	
}

void GameState::resume(){
}

void GameState::exit(){
}

void GameState::displayPinfo() {
	pinfo_timer_ = 0;
	pinfo_ = true;
	pinfo_text_.setPosition(100, 600);
}

void GameState::won() {
	SoundManager::play("win");
	sendTrap(MESSAGE_WIN, 0);
}

void GameState::generateDoor(DoorConditionsList condition, int room) {
	//Generate all doors on room
	for (int i = 0; i < 4; i++) {
		Room::DoorPosition pos = static_cast<Room::DoorPosition>(i);
		rooms_[room]->addDoor(pos, condition);
	}
}

void GameState::generateMap(int max_rooms) {
	srand(time(NULL));
	//Receive solution;
	bool done = false;	
	ConditionList list;

	while (!done) {
		sf::Packet p;
		if (socket_.receive(p) == sf::Socket::Done) {
			std::string cond;
			int player_no, room_no;
			p >> cond >> player_no >> room_no;
			if (cond == "cond") {
				ListCondition condition(player_no, room_no);
				list.push_back(condition);
			}
			else if (cond == "next") {
				list = ConditionList();
			}
			else if (cond == "end") {
				done = true;
			}
			else if (cond == "add") {
				solution_.push_back(list);
			}
		}
	}

	int win_step = 0;
	int win_room = 0;

	for (int i = 1; i < solution_.size(); i++) {
		ConditionList list = solution_[i];
		for (auto& cond : list) {
			if (cond.player_no == player_no_) {
				win_room = cond.room_no;
				win_step = i;
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

	ConditionList win_list = solution_[win_step];

	std::vector<int> players(max_rooms, 0);
	for (auto& prev : win_list) {
		players[prev.room_no]++;
	}
	DoorConditionsList win_cond_list;
	for (int n = 0; n < players.size(); n++) {
		if (players[n] != 0) {
			win_cond_list.push_back(DoorConditions(players[n], n));
		}
	}
	int final_x;
	int final_y;
	float step = 1;
	int final_room = 0;
	do {
		int cap = floor(step);
		if (cap > 10) cap = 10;
		int spawn_dist = rand() % cap + 1;
		int compx = rand() % spawn_dist;
		int compy = spawn_dist - compx;
		int negx = rand() % 2;
		int negy = rand() % 2;
		if (negx) compx = -compx;
		if (negy) compy = -compy;
		final_x = roomNumToCoord(win_room).x + compx;
		final_y = roomNumToCoord(win_room).y + compy;
		final_room = coordToRoomNum(sf::Vector2i(final_x, final_y));
		step+=0.5f;
	} while (final_x < 0 || final_x >= row_size_ || final_y < 0 || final_y >= row_size_ ||
		rooms_[final_room]->containsDoors());

	WinBlock* win = new WinBlock(&resourceManager_, entity_manager_.get(), sfld::Vector2f(0, 0));
	generateDoor(win_cond_list, final_room);
	rooms_[win_room]->add(win, sfld::Vector2f(100, 100));

	for (auto& room : rooms_) {
		if (!room->containsDoors() && room->getRoomNum() != spawn_room_) {
			int r = rand() % 10;
			if (r < 3) {
				int trapn = rand() % 2;
				if (trapn == 0){
					room->addCotp();
				}
				else if (trapn == 1) {
					room->addLocust();
				}
			}
		}
		
		int r = rand() % 5;
		if (r == 0) { //20% chance of trap chest spawning
			int trapn = rand() % 4;
			TrapChest* trap;
			if (trapn == 0) {
				trap = new TrapChest(&resourceManager_, entity_manager_.get(), sfld::Vector2f(0, 0), "trapchest", MESSAGE_TRAP_RED, "Blinding Light");
			}
			else if (trapn == 1) {
				trap = new TrapChest(&resourceManager_, entity_manager_.get(), sfld::Vector2f(0, 0), "trapchest", MESSAGE_TRAP_COTM, "Curse of the Mummy");
			}
			else if (trapn == 2) {
				trap = new TrapChest(&resourceManager_, entity_manager_.get(), sfld::Vector2f(0, 0), "trapchest", MESSAGE_TRAP_COTP, "Curse of the Pharaoh");
			}
			else if (trapn == 3) {
				trap = new TrapChest(&resourceManager_, entity_manager_.get(), sfld::Vector2f(0, 0), "trapchest", MESSAGE_TRAP_LOCUST, "Locust Swarm");
			}
			int x = rand() % (room_size_ - 2 * TILE_SIZE) + 2 * TILE_SIZE;
			int y = rand() % (room_size_ - 2 * TILE_SIZE) + 2 * TILE_SIZE;
			room->add(trap, sfld::Vector2f(x, y));
		}
		r = rand() % 10;
		if (r < 3) { //30% chance of trap chest spawning
			int x = rand() % (room_size_ - 2 * TILE_SIZE) + 2 * TILE_SIZE;
			int y = rand() % (room_size_ - 2 * TILE_SIZE) + 2 * TILE_SIZE;
			TrapChest* healthup = new TrapChest(&resourceManager_, entity_manager_.get(), sfld::Vector2f(0, 0), "healthup", MESSAGE_PACK_HEALTH, "Health Pack");
			room->add(healthup, sfld::Vector2f(x, y));
		}

		r = rand() % 4;
		if (r == 0) {
			SwordChest* chest = new SwordChest(&resourceManager_, entity_manager_.get(), sfld::Vector2f(0, 0));
			int x = rand() % (room_size_ - 2 * TILE_SIZE) + 2 * TILE_SIZE;
			int y = rand() % (room_size_ - 2 * TILE_SIZE) + 2 * TILE_SIZE;
			room->add(chest, sfld::Vector2f(x, y));
		}

		r = rand() % 10;
		if (r == 0) {
			EyeChest* chest = new EyeChest(&resourceManager_, entity_manager_.get(), sfld::Vector2f(0, 0));
			int x = rand() % (room_size_ - 2 * TILE_SIZE) + 2 * TILE_SIZE;
			int y = rand() % (room_size_ - 2 * TILE_SIZE) + 2 * TILE_SIZE;
			room->add(chest, sfld::Vector2f(x, y));
		}
	}
}

sf::Vector2i GameState::roomNumToCoord(int n) const{
	int x = floor(n / (float)row_size_);
	int y = n % row_size_;
	return sf::Vector2i(x, y);
}

int GameState::coordToRoomNum(sf::Vector2i coord) const{
	return coord.x * row_size_ + coord.y;
}

void GameState::update(int frameTime){
	if (!game_ended_) {
		particle_engine_->update(frameTime);
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
		//pinfo
		if (pinfo_) {
			pinfo_timer_ += frameTime;
			if (pinfo_timer_ >= 5000) {
				pinfo_ = false;
			}
			else {
				std::string str = "Players are in rooms: ";
				for (auto& it : player_infos_) {
					if (it.msg_type == MESSAGE_INFO) {
						str += it.name + ": " + std::to_string(it.room_no) + " - ";
					}
				}
				//std::cout << str << std::endl;
				pinfo_text_.setString(str);
			}
		}
	}
}

void GameState::render(sf::RenderTarget* target){
	//TODO: only render things on the player's screen so it won't lag to shit
	for (auto& it : rooms_) {
		it->renderBackground(target);
	}
	particle_engine_->renderStatics(target);
	for (auto& it : rooms_) {
		target->draw(it->getRoomText());
	}
	for (auto& it : rooms_) {
		it->render(target);
	}
	entity_manager_->render(target);
	
	particle_engine_->renderParticles(target);
	entity_manager_->renderTrapInterface(trap_interface_.get(),target);

	if (pinfo_) {
		entity_manager_->renderUnaffected(pinfo_text_,target);
	}
}

void GameState::generateRooms(int room_root) {
	int room_num = 0;
	for (int x = 0; x < room_root; x++) {
		for (int y = 0; y < room_root; y++) {
			Room::DoorOrientation orientation = Room::ORIENTATION_NONE;
			if (x == 0 && y == 0) {
				orientation = Room::ORIENTATION_TOPLEFT;
			}
			else if (x == room_root - 1 && y == room_root - 1) {
				orientation = Room::ORIENTATION_BOTRIGHT;
			}
			else if (x == room_root - 1 && y == 0) {
				orientation = Room::ORIENTATION_TOPRIGHT;
			}
			else if (x == 0 && y == room_root - 1) {
				orientation = Room::ORIENTATION_BOTLEFT;
			}
			else if (x == 0) {
				orientation = Room::ORIENTATION_LEFT;
			}
			else if (x == room_root-1) {
				orientation = Room::ORIENTATION_RIGHT;
			}
			else if (y == 0) {
				orientation = Room::ORIENTATION_TOP;
			}
			else if (y == room_root-1) {
				orientation = Room::ORIENTATION_BOT;
			}
			rooms_.push_back(std::unique_ptr<Room>(new Room(room_num, sfld::Vector2f(x*room_size_, y*room_size_),
				room_size_, entity_manager_.get(), player_, &resourceManager_,&player_infos_,orientation)));
			room_num++;
		}
	}
}

void GameState::sendData() {
	PlayerInfo info;
	info.msg_type = MESSAGE_INFO;
	info.name = player_name_;
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
				player_infos_[info.player_no].name = info.name;
				player_infos_[info.player_no].room_no = info.room_no;
			}
			else {
				std::cout << "WTF? Received player info num: " << info.player_no << std::endl;
			}
		}
		else if(info.msg_type == MESSAGE_WIN){
			game_ended_ = true;
			if (info.player_no == player_no_) {
				entity_manager_->displayTemporaryMessage("You won!");
			}
			else {
				SoundManager::play("lost");
				entity_manager_->displayTemporaryMessage("You lost! " + info.name + " reached the treasure!");
			}
		}
		else if(info.player_no != player_no_){
			player_infos_.push_back(info);
		}
	}
}

