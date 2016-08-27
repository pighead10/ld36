#pragma once

#include "stdafx.h"
#include "ResourceManager.h"

#define TILE_SIZE 32

class Entity;
class ParticleEngine;

typedef std::vector<std::unique_ptr<Entity>> EntityList;

class EntityManager{
public:
	EntityManager(ResourceManager<sf::Texture,std::string>* resourceManager);
	~EntityManager();

	void addEntity(Entity* entity);
	EntityList* getEntities();

	void update(int frametime);
	void render(sf::RenderTarget* target);

	void clear();
private:
	sf::View view;
	ResourceManager<sf::Texture, std::string>* resourceManager_;

	EntityList push_queue_;
	EntityList entities_;
};