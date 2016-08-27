#pragma once

#include "BaseState.h"
#include "ResourceManager.h"

class EntityManager;

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
	std::unique_ptr<EntityManager> entity_manager_;

	ResourceManager<sf::Texture, std::string> resourceManager_;
};