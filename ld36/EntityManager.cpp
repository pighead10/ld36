#include "stdafx.h"
#include "EntityManager.h"
#include "Entity.h"
#include "SFLD.h"
#include "ResourceManager.h"

EntityManager::EntityManager(ResourceManager<sf::Texture, std::string>* resourceManager):resourceManager_(resourceManager){
	view = SFLD::window_->getDefaultView();
}

EntityManager::~EntityManager() = default;

void EntityManager::addEntity(Entity* entity){
	push_queue_.push_back(std::unique_ptr<Entity>(entity));
}

EntityList* EntityManager::getEntities(){
	return &entities_;
}

void EntityManager::clear(){
	entities_.clear();
	push_queue_.clear();
}

void EntityManager::update(int frameTime){
	for (auto& it : entities_){
		it->update(frameTime);
	}
	for (auto& it = entities_.begin(); it != entities_.end();){
		if ((*it)->isDestroyed()){
			it = entities_.erase(it);
		}
		else{
			it++;
		}
	}

	for (auto& it : push_queue_){
		entities_.push_back(std::move(it));
	}
	push_queue_.clear();
}

void EntityManager::render(sf::RenderTarget* target){
	for (auto& it : entities_){
		it->render(target);
	}
}