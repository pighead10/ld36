#include "stdafx.h"
#include "TrapInferface.h"
#include "EntityManager.h"
#include "Trap.h"
#include "Eye.h"

TrapInterface::TrapInterface() = default;

TrapInterface::TrapInterface(EntityManager* entity_manager, int max_rooms) : entity_manager_(entity_manager), state_(TRAP_INTERFACE_NONE),selected_(0), max_rooms_(max_rooms){
	font_ = entity_manager->getFont();
	createGui();
}
TrapInterface::~TrapInterface() = default;

void TrapInterface::createGui() {
	sf::Text text;
	text.setFont(*font_);
	text.setCharacterSize(30);
	text.setString("Items available: (press T to select)");
	text.setPosition(20, 20);
	texts_.push_back(text);
	selection_text_.setFont(*font_);
	offset_ = 25;
}

void TrapInterface::placeTrap(int room_num) {
	traps_[selected_]->doTrap(room_num);
	traps_.erase(traps_.begin() + selected_);
	trap_texts_.erase(trap_texts_.begin() + selected_);
	positionListItems();
	resetState();
}

void TrapInterface::positionListItems() {
	for (int i = 0; i < trap_texts_.size(); i++) {
		trap_texts_[i].setPosition(20, i * offset_ + 70);
	}
}

void TrapInterface::addTrap(Trap* trap) {
	traps_.push_back(std::unique_ptr<Trap>(trap));
	sf::Text text;
	text.setFont(*font_);
	text.setCharacterSize(20);
	text.setFillColor(sf::Color::White);
	text.setString(trap->getName());
	trap_texts_.push_back(text);
	positionListItems();
}

void TrapInterface::resetState() {
	state_ = TRAP_INTERFACE_NONE;
	resetListNames();
}

void TrapInterface::addSight() {
	sf::Text text;
	text.setFont(*font_);
	text.setCharacterSize(20);
	text.setFillColor(sf::Color::White);
	text.setString("Ra's Eye");
	trap_texts_.push_back(text);

	traps_.push_back(std::unique_ptr<Trap>(new Eye(entity_manager_)));
	positionListItems();
}

void TrapInterface::selectItem(int item) {
	selected_ = item;
	resetListNames();
	trap_texts_[item].setString(trap_texts_[item].getString() + " X");
}

void TrapInterface::resetListNames() {
	for (int i = 0; i < traps_.size(); i++) {
		trap_texts_[i].setString(traps_[i]->getName());
	}
}

void TrapInterface::confirmSelection() {
	typed_num_ = "";
	if (traps_[selected_]->getName() != "Ra's Eye") {
		state_ = TRAP_INTERFACE_PLACING;
		selection_text_.setString("Item selected. Enter room: ");
		selection_text_.setCharacterSize(20);
		sf::FloatRect bounds = trap_texts_[selected_].getGlobalBounds();
		selection_text_.setPosition(bounds.left + bounds.width, bounds.top);
	}
	else {
		placeTrap(0);
	}
}

void TrapInterface::startSelecting() {
	state_ = TRAP_INTERFACE_SELECTING;
	selectItem(0);
}

void TrapInterface::keyPressed(sf::Keyboard::Key key) {
	if (key == sf::Keyboard::T) {
		if (traps_.size() > 0) {
			startSelecting();
		}
	}
	else if (key == sf::Keyboard::Escape) {
		resetState();
	}
	if (state_ == TRAP_INTERFACE_SELECTING) {
		if (key == sf::Keyboard::Up) {
			int n = selected_ - 1;
			if (n < 0) {
				n = traps_.size() - 1;
			}
			selectItem(n);
		}
		else if (key == sf::Keyboard::Down) {
			selectItem((selected_ + 1) % traps_.size());
		}
		else if (key == sf::Keyboard::Return) {
			confirmSelection();
		}
	}
	else if (state_ == TRAP_INTERFACE_PLACING) {
		if (key >= 26 && key <= 35) {
			char code = key + 22;
			typed_num_ += code;
			selection_text_.setString("Item selected. Enter room: " + typed_num_);
		}
		else if (key == sf::Keyboard::BackSpace) {
			if (typed_num_.size() > 0) {
				typed_num_.pop_back();
				selection_text_.setString("Trap selected. Enter room: " + typed_num_);
			}
		}
		else if (key == sf::Keyboard::Return) {
			if (typed_num_.size() > 0) {
				int room_no = stoi(typed_num_);
				if (room_no >= 0 && room_no <= max_rooms_) {
					placeTrap(stoi(typed_num_));
				}
			}
		}
	}
}

void TrapInterface::render(sf::RenderTarget* target) {
	for (auto& it : texts_) {
		target->draw(it);
	}
	for (auto& it : trap_texts_) {
		target->draw(it);
	}
	if (state_ == TRAP_INTERFACE_PLACING) {
		target->draw(selection_text_);
	}
}