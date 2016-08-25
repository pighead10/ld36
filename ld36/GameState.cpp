#include "stdafx.h"
#include "GameState.h"
#include "StateManager.h"
#include "SoundManager.h"

GameState::GameState() = default;
GameState::~GameState() = default;

void GameState::sfmlEvent(sf::Event evt){
	if (evt.type == sf::Event::Closed || (evt.type == sf::Event::KeyPressed && evt.key.code == sf::Keyboard::Escape)){
		getParent().pop();
	}
}

void GameState::start(){
	resourceManager_.setDirectory("media/images/");
	resourceManager_.load("pig", "pig.png");
	pig.setTexture(resourceManager_.get("pig"));
}

void GameState::pause(){
}

void GameState::resume(){
}

void GameState::exit(){
}

void GameState::update(int frameTime){
}

void GameState::render(sf::RenderTarget* target){
	target->draw(pig);
}