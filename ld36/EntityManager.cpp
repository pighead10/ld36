#include "stdafx.h"
#include "EntityManager.h"
#include "Entity.h"
#include "SFLD.h"
#include "ResourceManager.h"
#include "TrapInferface.h"

EntityManager::EntityManager(ResourceManager<sf::Texture, std::string>* resourceManager, std::vector<PlayerInfo>* player_infos):
	resourceManager_(resourceManager), player_infos_(player_infos),red_trap_(false){
	view_ = SFLD::window_->getDefaultView();
	red_tex.create(1024, 768);
	red_tex.clear(sf::Color::Red);
	font_.loadFromFile("media/Futura_ICG.ttf");
}

EntityManager::~EntityManager() = default;

void EntityManager::renderText(sf::Text text) {
	texts_.push_back(text);
}

sf::Font* EntityManager::getFont() {
	return &font_;
}

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

void EntityManager::doRedTrap() {
	red_trap_ = true;
	red_timer_ = 0;
}

void EntityManager::update(int frameTime){
	texts_.clear();
	if (red_trap_) {
		red_timer_ += frameTime;
		if (red_timer_ >= 2000) {
			red_trap_ = false;
		}
	}

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

void EntityManager::renderTrapInterface(TrapInterface* trap_interface,sf::RenderTarget* target) {
	SFLD::window_->setView(SFLD::window_->getDefaultView());
	trap_interface->render(target);
	SFLD::window_->setView(view_);
}

void EntityManager::render(sf::RenderTarget* target){
	SFLD::window_->setView(view_);
	for (auto& it : entities_){
		if (it->isActive()) {
			it->render(target);
		}
	}
	if (red_trap_) {
		SFLD::window_->setView(SFLD::window_->getDefaultView());
		sf::Sprite red(red_tex.getTexture());
		target->draw(red);
		SFLD::window_->setView(view_);
	}
	for (auto& it : texts_) {
		target->draw(it);
	}
}
