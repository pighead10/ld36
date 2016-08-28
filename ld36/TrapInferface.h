#pragma once

#include "stdafx.h"

class Trap;
class EntityManager;

class TrapInterface {
public:
	enum TrapInterfaceState {
		TRAP_INTERFACE_NONE,
		TRAP_INTERFACE_SELECTING,
		TRAP_INTERFACE_PLACING
	};
	TrapInterface();
	TrapInterface(EntityManager* entity_manager, int max_rooms);
	~TrapInterface();

	void addTrap(Trap* trap);

	void keyPressed(sf::Keyboard::Key key);
	void render(sf::RenderTarget* target);
private:
	int offset_;
	void positionListItems();

	void placeTrap(int room_num);
	void selectItem(int item);

	void confirmSelection();

	void startSelecting();
	void resetState();
	void createGui();
	void resetListNames();
	
	TrapInterfaceState state_;

	EntityManager* entity_manager_;

	sf::Font* font_;
	std::vector<std::unique_ptr<Trap>> traps_;
	std::vector<sf::Text> trap_texts_;
	int selected_;

	std::string typed_num_;
	sf::Text selection_text_;
	std::vector<sf::Text> texts_;

	int max_rooms_;
};