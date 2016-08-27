#include "stdafx.h"
#include "GameState.h"
#include "StateManager.h"
#include "SoundManager.h"
#include "StaticObj.h"
#include "EntityManager.h"
#include "Player.h"

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

	entity_manager_->addEntity(new StaticObj(&resourceManager_, "wall", entity_manager_.get(), sfld::Vector2f(50, 50), Entity::SHAPE_SQUARE, Entity::TYPE_WALL));
	entity_manager_->addEntity(new Player(&resourceManager_, entity_manager_.get(), sfld::Vector2f(0, 0)));
}

void GameState::pause(){	
}

void GameState::resume(){
}

void GameState::exit(){
}

void GameState::update(int frameTime){
	entity_manager_->update(frameTime);
}

void GameState::render(sf::RenderTarget* target){
	entity_manager_->render(target);
}