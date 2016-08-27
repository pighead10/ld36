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

void GameState::sfmlEvent(sf::Event evt){
	if (evt.type == sf::Event::Closed || (evt.type == sf::Event::KeyPressed && evt.key.code == sf::Keyboard::Escape)){
		getParent().pop();
	}
}

void GameState::start(){
	entity_manager_ = std::unique_ptr<EntityManager>(new EntityManager(&resourceManager_));

	resourceManager_.setDirectory("media/images/");
	resourceManager_.load("player", "pig.png");
	resourceManager_.load("wall", "wall.png");

	player_ = new Player(&resourceManager_, entity_manager_.get(), sfld::Vector2f(50, 50));
	entity_manager_->addEntity(player_);

	generateRooms();
	rooms_[0]->add(new StaticObj(&resourceManager_, "wall", entity_manager_.get(), sfld::Vector2f(0,0), Entity::SHAPE_SQUARE, Entity::TYPE_WALL), sfld::Vector2f(50,50));
	rooms_[2]->add(new StaticObj(&resourceManager_, "wall", entity_manager_.get(), sfld::Vector2f(0, 0), Entity::SHAPE_SQUARE, Entity::TYPE_WALL), sfld::Vector2f(50, 50));
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
}

void GameState::render(sf::RenderTarget* target){
	//TODO: only render things on the player's screen so it won't lag to shit
	entity_manager_->render(target);
}

void GameState::generateRooms() {
	int room_root = 2; //so there are room_root^2 rooms total, so it can be arranged in a square
	int room_size = 8*TILE_SIZE;
	for (int x = 0; x < room_root; x++) {
		for (int y = 0; y < room_root; y++) {
			rooms_.push_back(std::unique_ptr<Room>(new Room(sfld::Vector2f(x*room_size, y*room_size),
				room_size, entity_manager_.get(), player_, &resourceManager_)));
		}
	}
}