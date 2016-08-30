#include "stdafx.h"
#include "Room.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Player.h"
#include "StaticObj.h"
#include "Door.h"
#include "Mummy.h"
#include <iomanip>
#include <sstream>
#include "ParticleEngine.h"
#include "Locust.h"
#include "SoundManager.h"
#include "HealthChest.h"

Room::Room(int room_num, sfld::Vector2f world_coords, int room_size, EntityManager* entity_manager,
	Player* player, ResourceManager<sf::Texture, std::string>* resource_manager, std::vector<PlayerInfo>* player_infos,DoorOrientation orientation) :
	world_coords_(world_coords), room_size_(room_size), entity_manager_(entity_manager), player_(player),
	resource_manager_(resource_manager), room_num_(room_num), player_infos_(player_infos),cotm_it(0),cotm_timer(0),cotm_ready(false),
	cotp_(false),cotp_timer_(0),cotp_started_(false), orientation_(orientation) {
	room_area_ = sf::FloatRect(world_coords, sfld::Vector2f(room_size, room_size));
	room_text_.setFont(*entity_manager->getFont());
	room_text_.setCharacterSize(room_size - 8*TILE_SIZE);
	room_text_.setString(std::to_string(room_num));
	room_text_.setOrigin(room_text_.getLocalBounds().left + room_text_.getLocalBounds().width / 2, room_text_.getLocalBounds().top + room_text_.getLocalBounds().height / 2);
	room_text_.setPosition(room_area_.left + room_area_.width / 2, room_area_.top + room_area_.height / 2);
	room_text_.setFillColor(sf::Color(255,255,255,100));
	generateRoom();
	background_.create(100, 100);
}

Room::~Room() = default;

void Room::playerTouchedWall(Entity* door) {
	sf::Text text;
	text.setFont(*entity_manager_->getFont());
	text.setCharacterSize(16);
	std::string str= "Door requires ";
	DoorConditionsList list = getConditionsFromDoor(door);
	for (int i = 0; i < list.size(); i++) {
		std::string pl = list[i].players > 1 ? "players" : "player";
		str += std::to_string(list[i].players) + " " + pl + " in room " + std::to_string(list[i].room);
		if (i != list.size() - 1) {
			str += " and ";
		}
	}
	text.setString(str);
	text.setFillColor(sf::Color::White);
	text.setOrigin(text.getGlobalBounds().left + text.getGlobalBounds().width / 2, text.getGlobalBounds().top + text.getGlobalBounds().height / 2);
	text.setPosition(door->getPosition());
	entity_manager_->renderText(text);
}

DoorConditionsList Room::getConditionsFromDoor(Entity* door) {
	for (auto& it : doors_) {
		if (it.first == door) {
			return it.second;
		}
	}
	std::cout << "ERROR: Attempting to get conditions from door that doesn't exist" << std::endl;
	return DoorConditionsList();
}

void Room::setLit(bool lit) {
	lit_ = lit;
	for (auto& it : entities_) {
		if (it->getType() != Entity::TYPE_PLAYER) {
			it->setActive(lit);
		}
	}
}

bool Room::containsDoors() const{
	return (doors_.size() > 0);
}

bool Room::getLit() const {
	return lit_;
}

sf::Text Room::getRoomText() const {
	return room_text_;
}

void Room::setCotmReady() {
	if (!cotm_ready) {
		SoundManager::play("angryzombie");
		cotm_ready = true;
	}
}

void Room::addHealthPack() {
	HealthChest* chest = new HealthChest(resource_manager_, entity_manager_, sfld::Vector2f(0, 0));
	int x = rand() % (room_size_ - 2 * TILE_SIZE) + 2 * TILE_SIZE;
	int y = rand() % (room_size_ - 2 * TILE_SIZE) + 2 * TILE_SIZE;
	add(chest, sfld::Vector2f(x,y));
}

void Room::addCotp() {
	cotp_ = true;
	cotp_spr_.setTexture(resource_manager_->get("cotp"));
	cotp_spr_.setOrigin(cotp_spr_.getLocalBounds().width / 2, cotp_spr_.getLocalBounds().height / 2);
	sfld::Vector2f centre(room_area_.left + room_area_.width / 2, room_area_.top + room_area_.height / 2);
	cotp_spr_.setPosition(centre);
	cotp_text_.setFont(*entity_manager_->getFont());
	cotp_text_.setCharacterSize(30);
	cotp_text_.setFillColor(sf::Color::White);
}

int Room::getRoomNum() const{
	return room_num_;
}

void Room::update(int frame_time) {
	if (player_ != NULL) {
		if (player_->getRoomNum() == room_num_) {
			if (player_->getCotm()) {
				doCotm(frame_time);
			}
			else if (cotm_ready) {
				player_->doCotm();
			}
			if (cotp_ && !cotp_started_) {
				SoundManager::play("scaryscream");
				cotp_started_ = true;
				cotp_timer_ = 1200;

			}
			if (cotp_started_) {
				cotp_timer_ -= frame_time;
				std::stringstream stream;
				stream << std::fixed << std::setprecision(3) << (float)(cotp_timer_ / 1000.0f);
				cotp_text_.setString(stream.str());
				cotp_text_.setOrigin(cotp_text_.getLocalBounds().width / 2, cotp_text_.getLocalBounds().height / 2);
				sfld::Vector2f centre(room_area_.left + room_area_.width / 2, room_area_.top + room_area_.height / 2);
				cotp_text_.setPosition(centre);

				if (cotp_timer_ <= 0) {
					cotp_ = false;
					cotp_started_ = false;
					entity_manager_->getParticleEngine()->generatePurpleExplosionEffect(centre);
					SoundManager::play("explosionfuture");
					player_->damaged(40);
				}
			}
		}
		else {
			cotp_timer_ = 0;
			cotp_started_ = false;
		}

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

		for (auto& it : doors_) {
			int count = 0;
			std::vector<int> counts(1000, 0); //TODO change 1000...
			for (auto& info : *player_infos_) {
				counts[info.room_no]++;
			}
			bool open = true;
			for (auto& it : it.second) {
				if (counts[it.room] < it.players) {
					open = false;
					break;
				}
			}
			if (open) {
				if (!it.first->isOpen()) {
					SoundManager::play("dooropen");
					entity_manager_->displayTemporaryMessage("Door to room " + std::to_string(room_num_) + " opened!");
					it.first->setOpen(true);
				}
			} 
		}

		//Scan player infos for traps etc.
		for (auto& it = player_infos_->begin(); it != player_infos_->end();) {
			if (it->msg_type != MESSAGE_INFO && it->room_no == room_num_) {
				//Pop traps once read
				if (it->room_no == room_num_) {
					if (it->msg_type == MESSAGE_TRAP_RED && player_->getRoomNum() == room_num_) {
						std::cout << "Blinding light activated" << std::endl;
						SoundManager::play("trap");
						entity_manager_->doRedTrap();
					}
					else if (it->msg_type == MESSAGE_TRAP_COTM) {
						std::cout << "Curse of the mummy activated" << std::endl;
						setCotmReady();
					}
					else if (it->msg_type == MESSAGE_TRAP_COTP) {
						std::cout << "Curse of the pharaoh activated" << std::endl;
						addCotp();
					}
					else if (it->msg_type == MESSAGE_TRAP_LOCUST) {
						std::cout << "Locust trap activated" << std::endl;
						addLocust();					
					}
					else if (it->msg_type == MESSAGE_PACK_HEALTH) {
						std::cout << "Health pack activated" << std::endl;
						addHealthPack();
					}
				}
				it = player_infos_->erase(it);
			}
			else {
				it++;
			}
		}
	}
}

void Room::addLocust() {
	Locust* locust = new Locust(resource_manager_, entity_manager_, sfld::Vector2f(0, 0), "locust", 0.5f, player_, 5);
	add(locust, sfld::Vector2f(room_size_ / 2, room_size_ / 2));
}

void Room::doCotm(int frame_time) {
	cotm_timer += frame_time;
	int threshold = std::max(3000 - (333 * cotm_it), 500);
	if (cotm_timer > threshold) {
		cotm_it++;
		cotm_timer = 0;
		Entity* mummy = new Mummy(resource_manager_, entity_manager_, sfld::Vector2f(0,0), "mummy", 0.1f, player_, 20);
		add(mummy, sfld::Vector2f(room_size_ / 2, room_size_ / 2));
	}
}

void Room::add(Entity* entity, sfld::Vector2f local_position) {
	entity->setPosition(world_coords_ + local_position);
	entity_manager_->addEntity(entity);
	entities_.push_back(entity);
}

void Room::generateRoom() {
	midx_ = (room_area_.left * 2 + room_area_.width) / 2;
	midy_ = (room_area_.top * 2 + room_area_.height) / 2;
	for (int x = room_area_.left; x <= room_area_.left + room_area_.width; x += TILE_SIZE) {
		if (orientation_ == ORIENTATION_TOP || orientation_ == ORIENTATION_TOPLEFT || orientation_ == ORIENTATION_TOPRIGHT ||(x != midx_ && x != (midx_ + TILE_SIZE))) {
			entity_manager_->addEntity(new StaticObj(resource_manager_, "wall", entity_manager_, sfld::Vector2f(x, room_area_.top), Entity::SHAPE_SQUARE, Entity::TYPE_WALL));
		}
		if (orientation_ == ORIENTATION_BOT || orientation_ == ORIENTATION_BOTLEFT || orientation_ == ORIENTATION_BOTRIGHT || (x != midx_ && x != (midx_ + TILE_SIZE))) {
			entity_manager_->addEntity(new StaticObj(resource_manager_, "wall", entity_manager_, sfld::Vector2f(x, room_area_.top + room_area_.height), Entity::SHAPE_SQUARE, Entity::TYPE_WALL));
		}
	}
	
	for (int y = room_area_.top+TILE_SIZE; y <= room_area_.top + room_area_.height-TILE_SIZE; y += TILE_SIZE) {
		if (orientation_ == ORIENTATION_LEFT || orientation_ == ORIENTATION_BOTLEFT || orientation_ == ORIENTATION_TOPLEFT || (y != midy_ && y != (midy_ + TILE_SIZE))) {
			entity_manager_->addEntity(new StaticObj(resource_manager_, "wall", entity_manager_, sfld::Vector2f(room_area_.left, y), Entity::SHAPE_SQUARE, Entity::TYPE_WALL));
		}
		if (orientation_ == ORIENTATION_RIGHT || orientation_ == ORIENTATION_BOTRIGHT || orientation_ == ORIENTATION_TOPRIGHT || (y != midy_ && y != (midy_ + TILE_SIZE))) {
			entity_manager_->addEntity(new StaticObj(resource_manager_, "wall", entity_manager_, sfld::Vector2f(room_area_.left + room_area_.width, y), Entity::SHAPE_SQUARE, Entity::TYPE_WALL));
		}
	}
}

void Room::render(sf::RenderTarget* target) {
	if (lit_) {
		if (cotp_) {
			target->draw(cotp_spr_);
			target->draw(cotp_text_);
		}
	}
}

void Room::renderBackground(sf::RenderTarget* target) {
	sf::Sprite s;
	if (lit_) {
		s.setTexture(entity_manager_->getLitTexture()->getTexture());
	}
	else {
		s.setTexture(entity_manager_->getUnlitTexture()->getTexture());
	}
	s.setPosition(world_coords_);
	target->draw(s);
}

void Room::addDoor(DoorPosition position, DoorConditionsList conditions) {
	//For now, just add 2 doors to fill up the entire gap.
	sfld::Vector2f coords;
	sfld::Vector2f coords1;
	switch (position) {
	case DOOR_TOP:
		coords = sfld::Vector2f(midx_, room_area_.top);
		coords1 = coords + sfld::Vector2f(TILE_SIZE, 0);
		break;
	case DOOR_BOT:
		coords = sfld::Vector2f(midx_, room_area_.top+room_area_.height);
		coords1 = coords + sfld::Vector2f(TILE_SIZE, 0);
		break;
	case DOOR_LEFT:
		coords = sfld::Vector2f(room_area_.left, midy_);
		coords1 = coords + sfld::Vector2f(0, TILE_SIZE);
		break;
	case DOOR_RIGHT:
		coords = sfld::Vector2f(room_area_.left+room_area_.width, midy_);
		coords1 = coords + sfld::Vector2f(0, TILE_SIZE);
		break;
	}
	Door* door = new Door(resource_manager_, entity_manager_, coords, false, this);
	Door* door2 = new Door(resource_manager_, entity_manager_, coords1, false, this);
	doors_.push_back(std::make_pair(door, conditions));
	doors_.push_back(std::make_pair(door2, conditions));
	entity_manager_->addEntity(door);
	entity_manager_->addEntity(door2);
}