#pragma once

#include "stdafx.h"
#include "ResourceManager.h"
#include "PlayerInfo.h"

#define TILE_SIZE 32

class Entity;
class ParticleEngine;

typedef std::vector<std::unique_ptr<Entity>> EntityList;

class EntityManager{
public:
	EntityManager(ResourceManager<sf::Texture,std::string>* resourceManager, std::vector<PlayerInfo>* player_infos);
	~EntityManager();

	void addEntity(Entity* entity);
	EntityList* getEntities();

	void update(int frametime);
	void render(sf::RenderTarget* target);

	void setViewFocus(Entity* entity);

	sf::Font* getFont();

	void clear();

	void doRedTrap();
	void renderText(sf::Text text);
private:
	sf::Font font_;

	bool red_trap_;
	int red_timer_;

	sf::RenderTexture red_tex;
	sf::View view_;
	ResourceManager<sf::Texture, std::string>* resourceManager_;

	EntityList push_queue_;
	EntityList entities_;
	std::vector<PlayerInfo>* player_infos_;

	std::vector<sf::Text> texts_;
};