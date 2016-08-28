#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

enum MessageType {
	MESSAGE_INFO,
	MESSAGE_TRAP_RED,
};

struct PlayerInfo {
	int msg_type;
	int player_no;
	int room_no;
	std::string name;
};