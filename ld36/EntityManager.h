#pragma once

#include "stdafx.h"
#include "ResourceManager.h"
#include "PlayerInfo.h"

#define TILE_SIZE 32

class Entity;
class ParticleEngine;
class TrapInterface;
class GameState;
class Trap;
class Room;

typedef std::vector<std::unique_ptr<Entity>> EntityList;

class EntityManager{
public:
	EntityManager(ResourceManager<sf::Texture,std::string>* resourceManager, std::vector<PlayerInfo>* player_infos, GameState* game_state,int room_size);
	~EntityManager();

	void addEntity(Entity* entity);
	EntityList* getEntities();

	void update(int frametime);
	void render(sf::RenderTarget* target);

	void setViewFocus(Entity* entity);

	sf::Font* getFont();

	void clear();
	void addTrap(Trap* trap);
	void setTrapInterface(TrapInterface* trap_interface);
	void doRedTrap();
	void renderText(sf::Text text);
	void renderTrapInterface(TrapInterface* trap_interface, sf::RenderTarget* target);

	void sendTrap(MessageType type, int room_no);
	sf::RenderTexture* getLitTexture();
	sf::RenderTexture* getUnlitTexture();

	Room* getRoom(int n);

	void displayTemporaryMessage(std::string text);

	void won();
private:
	TrapInterface* trap_interface_;
	GameState* game_state_;
	sf::Font font_;

	bool red_trap_;
	int red_timer_;
	int room_size_;

	sf::RenderTexture red_tex;
	sf::View view_;
	ResourceManager<sf::Texture, std::string>* resourceManager_;

	EntityList push_queue_;
	EntityList entities_;
	std::vector<PlayerInfo>* player_infos_;

	std::vector<sf::Text> texts_;
	sf::RenderTexture lit_tex_;
	sf::RenderTexture unlit_tex_;

	sf::Text temp_text_;
	int temp_timer_;
	bool display_temp_;
};