#include "stdafx.h"
#include "MenuState.h"
#include "GameState.h"
#include "StateManager.h"

MenuState::MenuState():selected(0),displaying(false) {
}

MenuState::~MenuState() = default;

void MenuState::sfmlEvent(sf::Event evt) {
	if (evt.type == sf::Event::KeyPressed) {
		if (!displaying) {
			if (evt.key.code == sf::Keyboard::Up) {
				selected--;
				if (selected < 0) selected = 3;
			}
			else if (evt.key.code == sf::Keyboard::Down) {
				selected = (selected + 1) % 4;
			}
			else if (evt.key.code == sf::Keyboard::Return) {
				if (selected != 0) {
					displaying = true;
				}
				else {
					getParent().push(new GameState());
				}
			}
		}
		else {
			displaying = false;
		}
	}
}

void MenuState::update(int frameTime) {}
void MenuState::render(sf::RenderTarget* target) {
	if (!displaying) {
		if (selected == 0) target->draw(playselect);
		else if (selected == 1) target->draw(runselect);
		else if (selected == 2) target->draw(instselect);
		else if (selected == 3) target->draw(controlselect);
	}
	else {
		if (selected == 1) target->draw(run);
		else if (selected == 2) target->draw(play);
		else if (selected == 3) target->draw(controls);
	}
}

void MenuState::pause() {}
void MenuState::resume() {}
void MenuState::exit() {}

void MenuState::start() {
	resource_manager_.setDirectory("media/images/");
	resource_manager_.load("howtoplay", "howtoplay.png");
	resource_manager_.load("howtorun", "howtorun.png");
	resource_manager_.load("controls", "controls.png");
	resource_manager_.load("titlescreen", "titlescreen.png");
	resource_manager_.load("titlescreen_controlsselected", "titlescreen_controlsselect.png");
	resource_manager_.load("titlescreen_instselect", "titlescreen_instselect.png");
	resource_manager_.load("titlescreen_runselect", "titlescreen_runselect.png");

	playselect.setTexture(resource_manager_.get("titlescreen"));
	runselect.setTexture(resource_manager_.get("titlescreen_runselect"));
	instselect.setTexture(resource_manager_.get("titlescreen_instselect"));
	controlselect.setTexture(resource_manager_.get("titlescreen_controlsselected"));
	run.setTexture(resource_manager_.get("howtorun"));
	play.setTexture(resource_manager_.get("howtoplay"));
	controls.setTexture(resource_manager_.get("controls"));
}