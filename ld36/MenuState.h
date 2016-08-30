#pragma once

#include "BaseState.h"
#include "ResourceManager.h"

class MenuState : public BaseState {
public:
	MenuState();
	~MenuState();

	void sfmlEvent(sf::Event evt);
	void start();
	void pause();
	void resume();
	void exit();

	void update(int frameTime);
	void render(sf::RenderTarget* target);
private:
	sf::Sprite playselect;
	sf::Sprite runselect;
	sf::Sprite instselect;
	sf::Sprite controlselect;

	sf::Sprite run;
	sf::Sprite play;
	sf::Sprite controls;

	int selected;
	bool displaying;

	ResourceManager<sf::Texture, std::string> resource_manager_;
};