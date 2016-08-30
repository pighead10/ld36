#include <iostream>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include "../ld36/PlayerInfo.h"
#include <vector>
#include <memory>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <algorithm>

#define SEND_DELAY 100
#define ROW_SIZE 12

struct ListCondition {
	ListCondition(int p, int r) :room_no(r), player_no(p) {}
	int room_no;
	int player_no;
};

typedef std::vector<ListCondition> ConditionList;
typedef std::vector<ConditionList> SolutionList;


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

sf::Vector2i roomNumToCoord(int n) {
	int x = floor(n / (float)ROW_SIZE);
	int y = n % ROW_SIZE;
	return sf::Vector2i(x, y);
}

int coordToRoomNum(sf::Vector2i coord) {
	return coord.x * ROW_SIZE + coord.y;
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

	/*for (int i = 0; i < 3; i++) {
		players.push_back(PlayerInfo());
	}*/

	int total_players = players.size();

	std::vector<std::vector<int>> spawn_locations; //spawn_locations[no_of_players] = list of room nums for spawns

	//TODO: spawning is slightly wrong, fix
	std::vector<int> s2;
	s2.push_back(0);
	s2.push_back(ROW_SIZE*ROW_SIZE - 1);
	std::vector<int> s3;
	s3.push_back(0);
	s3.push_back(ROW_SIZE - 1 + ROW_SIZE*ROW_SIZE/2);
	s3.push_back(ROW_SIZE*ROW_SIZE - ROW_SIZE);
	std::vector<int> s4;
	s4.push_back(0);
	s4.push_back(ROW_SIZE - 1);
	s4.push_back(ROW_SIZE*ROW_SIZE - ROW_SIZE);
	s4.push_back(ROW_SIZE*ROW_SIZE - 1);
	std::vector<int> s5;
	s5.push_back(0);
	s5.push_back(ROW_SIZE - 1);
	s5.push_back(ROW_SIZE*ROW_SIZE - ROW_SIZE);
	s5.push_back(ROW_SIZE*ROW_SIZE - 1);
	s5.push_back(ROW_SIZE - 1 + ROW_SIZE*ROW_SIZE / 2);
	std::vector<int> s6;
	s6.push_back(0);
	s6.push_back(ROW_SIZE - 1);
	s6.push_back(ROW_SIZE*ROW_SIZE - ROW_SIZE);
	s6.push_back(ROW_SIZE*ROW_SIZE - 1);
	s6.push_back(ROW_SIZE - 1 + ROW_SIZE*ROW_SIZE / 2);
	s6.push_back(ROW_SIZE + ROW_SIZE*ROW_SIZE / 2);
	spawn_locations.push_back(s2);
	spawn_locations.push_back(s3);
	spawn_locations.push_back(s4);
	spawn_locations.push_back(s5);
	spawn_locations.push_back(s6);

	srand(time(NULL));

	std::vector<int> spawns(total_players, 0);
	int loc_i = total_players - 2;
	if (loc_i >= spawn_locations.size()) {
		loc_i = spawn_locations.size() - 1;
	}
	std::vector<int> locs = spawn_locations[total_players - 2];
	for (auto& it : spawns) {
		int n = rand() % locs.size();
		it = locs[n];
		//locs.erase(locs.begin() + n);
	}

	int completion_steps = rand() % 8 + 4;

	std::vector<std::vector<bool>> visited = std::vector<std::vector<bool>>(total_players, std::vector<bool>(ROW_SIZE*ROW_SIZE, false));
	SolutionList solution_list;

	//treasure opened by last instruction relasting to player. Spawns near that instruction room.
	//Necessary: Message that displays when room opens
	
	std::cout << "Game started. Sending start packets." << std::endl;

	for (int i = 0; i < clients.size(); i++) {
		//Send player number of player to each client followed by number of total players;
		sf::Packet s;
		s << i << total_players;
		clients[i]->send(s);
	}

	std::cout << "starting generation" << std::endl;
	for (int step = 1; step <= completion_steps+1; step++) {
		std::cout << "step: " << step << std::endl;
		ConditionList step_list;
		for (int player_no = 0; player_no < spawns.size(); player_no++) {
			std::cout << "player: " << player_no << std::endl;
			int chance = rand() % 10;
			if (chance <= 5) { //60% chance that a player will need to do something on each step
				std::cout << "player generating step" << std::endl;
				int final_x;
				int final_y;
				do {
					int cap = step;
					if (cap > 5) cap = 5;
					int spawn_dist = rand() % cap + 1;// step;
					int compx = rand() % spawn_dist;
					int compy = spawn_dist - compx;
					int negx = rand() % 2;
					int negy = rand() % 2;
					if (negx) compx = -compx;
					if (negy) compy = -compy;
					final_x = roomNumToCoord(spawns[player_no]).x + compx;
					final_y = roomNumToCoord(spawns[player_no]).y + compy;
					std::cout << "attempted room door: x " << final_x << " y " << final_y << std::endl;
				} while (final_x < 0 || final_x >= ROW_SIZE || final_y < 0 || final_y >= ROW_SIZE ||
					visited[player_no][coordToRoomNum(sf::Vector2i(final_x, final_y))]);
				
				int room_num = coordToRoomNum(sf::Vector2i(final_x, final_y));
				std::cout << "generated on room " << room_num << std::endl;
				ListCondition condition(player_no, room_num);
				visited[player_no][room_num] = true;
				step_list.push_back(condition);
			}
		}
		solution_list.push_back(step_list);
	}

	for (int i = 0; i < clients.size(); i++) {
		sf::Packet s;
		s << spawns[i];
		std::cout << "Sending spawn info. Player " << i << " spawns at " << spawns[i] << std::endl;
		clients[i]->send(s);
	}

	for (auto& condition_list : solution_list) {
		sf::Packet np;
		np << "next";
		sendToClients(&clients, np);
		std::cout << "sending next" << std::endl;
		for (auto& condition : condition_list) {
			sf::Packet c;
			c << "cond" << condition.player_no << condition.room_no;
			std::cout << "condition: player " << condition.player_no << " room: " << condition.room_no << std::endl;
			sendToClients(&clients, c);
		}
		sf::Packet p;
		p << "add";
		sendToClients(&clients, p);
	}

	sf::Packet ep;
	ep << "end";
	sendToClients(&clients, ep);
	std::cout << "sending end" << std::endl;
	
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
				else if (info.msg_type == MESSAGE_TRAP_RED || info.msg_type == MESSAGE_TRAP_COTM || info.msg_type == MESSAGE_TRAP_COTP || info.msg_type == MESSAGE_TRAP_LOCUST
					|| info.msg_type == MESSAGE_PACK_HEALTH) {
					std::cout << "Received red trap request in room " << info.room_no << ". Sending trap to clients." << std::endl;
					sendToClients(&clients, rec);
				}
				else if (info.msg_type == MESSAGE_WIN) {
					std::cout << "Player won the game!" << std::endl;
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