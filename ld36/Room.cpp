#include "stdafx.h"
#include "Room.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Player.h"
#include "StaticObj.h"

Room::Room(int room_num, sfld::Vector2f world_coords, int room_size, EntityManager* entity_manager,
	Player* player, ResourceManager<sf::Texture, std::string>* resource_manager, std::vector<PlayerInfo>* player_infos) :
	world_coords_(world_coords), room_size_(room_size), entity_manager_(entity_manager), player_(player),
	resource_manager_(resource_manager), room_num_(room_num), player_infos_(player_infos){
	room_area_ = sf::FloatRect(world_coords, sfld::Vector2f(room_size, room_size));
	generateRoom();
}

Room::~Room() = default;

void Room::setLit(bool lit) {
	lit_ = lit;
	for (auto& it : entities_) {
		it->setActive(lit);
	}
}

bool Room::getLit() const {
	return lit_;
}

void Room::update(int frame_time) {
	//Remove all entities that have been destroyed by EntityManager, which will be null pointers
	for (auto& it = entities_.begin(); it != entities_.end();) {
		if (*it == NULL) {
			it = entities_.erase(it);
		}
		else {
			it++;
		}
	}

	//Determine whether player is in this room
	if (room_area_.contains(player_->getPosition())) {
		setLit(true);
		player_->setRoomNum(room_num_);
	}
	else {
		setLit(false);
	}
}

void Room::add(Entity* entity, sfld::Vector2f local_position) {
	entity->setPosition(world_coords_ + local_position);
	entity_manager_->addEntity(entity);
	entities_.push_back(entity);
}

void Room::generateRoom() {
	for (int x = room_area_.left; x <= room_area_.left + room_area_.width; x += TILE_SIZE) {
		int mid0 = (room_area_.left * 2 + room_area_.width) / 2;
		int mid1 = (room_area_.left * 2 + room_area_.width) / 2 + TILE_SIZE;
		if (x != mid0 && x != mid1) {
			entity_manager_->addEntity(new StaticObj(resource_manager_, "wall", entity_manager_, sfld::Vector2f(x, room_area_.top), Entity::SHAPE_SQUARE, Entity::TYPE_WALL));
			entity_manager_->addEntity(new StaticObj(resource_manager_, "wall", entity_manager_, sfld::Vector2f(x, room_area_.top + room_area_.height), Entity::SHAPE_SQUARE, Entity::TYPE_WALL));
		}
	}
	for (int y = room_area_.top+TILE_SIZE; y <= room_area_.top + room_area_.height-TILE_SIZE; y += TILE_SIZE) {
		int mid0 = (room_area_.top * 2 + room_area_.height) / 2;
		int mid1 = (room_area_.top * 2 + room_area_.height) / 2 + TILE_SIZE;
		if (y != mid0 && y != mid1) {
			entity_manager_->addEntity(new StaticObj(resource_manager_, "wall", entity_manager_, sfld::Vector2f(room_area_.left, y), Entity::SHAPE_SQUARE, Entity::TYPE_WALL));
			entity_manager_->addEntity(new StaticObj(resource_manager_, "wall", entity_manager_, sfld::Vector2f(room_area_.left + room_area_.width, y), Entity::SHAPE_SQUARE, Entity::TYPE_WALL));
		}
	}
}