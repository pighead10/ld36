#include "stdafx.h"
#include "EntityManager.h"
#include "Entity.h"
#include "SFLD.h"
#include "ResourceManager.h"

EntityManager::EntityManager(ResourceManager<sf::Texture, std::string>* resourceManager):resourceManager_(resourceManager){
	view_ = SFLD::window_->getDefaultView();
}

EntityManager::~EntityManager() = default;

void EntityManager::setViewFocus(Entity* entity) {
	view_.setCenter(entity->getPosition());
}

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
		if (it->isActive()) {
			it->update(frameTime);
		}
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
	SFLD::window_->setView(view_);
	for (auto& it : entities_){
		if (it->isActive()) {
			it->render(target);
		}
	}
}