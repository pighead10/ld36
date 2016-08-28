#include <iostream>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include "../ld36/PlayerInfo.h"
#include <vector>
#include <memory>
#include <fstream>

#define SEND_DELAY 100

sf::Packet& operator << (sf::Packet& packet, const PlayerInfo& m)
{
	return packet << m.msg_type << m.player_no << m.room_no << m.name;
}
sf::Packet& operator >> (sf::Packet& packet, PlayerInfo& m)
{
	return packet >> m.msg_type >> m.player_no >> m.room_no >> m.name;
}


void sendToClients(std::vector<std::shared_ptr<sf::TcpSocket>>* clients, sf::Packet packet) {
	for (auto& it : (*clients)) {
		it->send(packet);
	}
}

void setClientBlocking(std::vector<std::shared_ptr<sf::TcpSocket>>* clients, bool blocking) {
	for (auto& it : (*clients)) {
		it->setBlocking(blocking);
	}
}

int main() {
	std::string ip;
	std::string port;
	std::ifstream fin("serverinfo.txt");
	fin >> ip;
	fin >> port;

	int port_num = stoi(port);

	sf::TcpListener listener;
	listener.listen(port_num);
	listener.setBlocking(false);
	std::cout << "Listening for connections on port " << port_num << ". Press s to start game." << std::endl;
	bool started = false;
	std::vector<std::shared_ptr<sf::TcpSocket>> clients;
	std::vector<PlayerInfo> players;

	while (!started) {
		std::shared_ptr<sf::TcpSocket> client(new sf::TcpSocket());
		if (listener.accept(*client) == sf::Socket::Done) {
			std::cout << "New connection received from " << client->getRemoteAddress() << std::endl;
			clients.push_back(client);
			players.push_back(PlayerInfo());
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			started = true;
		}
	}

	std::cout << "Game started. Sending start packets." << std::endl;
	int total_players = clients.size();
	for (int i = 0; i < clients.size(); i++) {
		//Send player number of player to each client followed by number of total players;
		sf::Packet s;
		s << i << total_players;
		clients[i]->send(s);
	}

	setClientBlocking(&clients, false);

	bool running = true;
	sf::Clock clock;
	while (running) {
		for (int i = 0; i < clients.size(); i++) {
			sf::Packet rec;
			if (clients[i]->receive(rec) == sf::Socket::Done) {
				PlayerInfo info;
				rec >> info;
				std::cout << "Received packet. Type num is: " << info.msg_type << std::endl;
				//MessageType type = static_cast<MessageType>(info.msg_type);
				if (info.msg_type == MESSAGE_INFO) {
					players[i] = info;
					std::cout << "Received info: Player number: " << players[i].player_no << " Name: " << players[i].name << " Room: " << players[i].room_no << std::endl;
				}
				else if (info.msg_type == MESSAGE_TRAP_RED || info.msg_type == MESSAGE_TRAP_COTM) {
					std::cout << "Received red trap request in room " << info.room_no << ". Sending trap to clients." << std::endl;
					sendToClients(&clients, rec);
				}
				
			}
		}
		if (clock.getElapsedTime().asMilliseconds() > SEND_DELAY) {
			for (auto& client : clients) {
				for (int i = 0; i < clients.size(); i++) {
					sf::Packet s;
					s << players[i];
					client->send(s);
				}
			}
			clock.restart();
		}
	}
	

	return 0;
}