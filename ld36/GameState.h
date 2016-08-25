#pragma once

#include "BaseState.h"

#include "ResourceManager.h"

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
	sf::Sprite pig;

	ResourceManager<sf::Texture, std::string> resourceManager_;
};