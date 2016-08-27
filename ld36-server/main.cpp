#include <iostream>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

#define PORT 5337 
#define IP 82.23.253.24
#define SEND_DELAY 100

struct PlayerInfo {
	int player_no;
	int room_no;
	std::string name;
};

sf::Packet& operator <<(sf::Packet& packet, const PlayerInfo& m)
{
	return packet << m.player_no << m.room_no << m.name;
}
sf::Packet& operator >> (sf::Packet& packet, PlayerInfo& m)
{
	return packet >> m.player_no >> m.room_no >> m.name;
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
	sf::TcpListener listener;
	listener.listen(PORT);
	listener.setBlocking(false);
	std::cout << "Listening for connections on port " << PORT << ". Press s to start game." << std::endl;
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
				rec >> players[i];
				std::cout << "Received info: Player number: " << players[i].player_no << " Name: " << players[i].name << " Room: " << players[i].room_no << std::endl;
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