#include <iostream>
#include <thread>
#include <enet/enet.hpp>

using namespace std;

string username;

void MsgLoop(ENetHost* _client)
{
	while (true)
	{
		ENetEvent event;
		while (enet_host_service(_client, &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				std::cout << event.packet->data << std::endl;
				enet_packet_destroy(event.packet);
				break;
			}
		}
	}
}

void SendPacket(ENetPeer* _peer, const char* _data)
{
	ENetPacket* packet = enet_packet_create(_data, strlen(_data) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(_peer, 0, packet);
}

int main()
{
	cout << "Enter your username: ";
	cin >> username;

	if (enet_initialize() != 0)
		std::cout << "Can't init ENet!" << std::endl;

	ENetHost* client;
	ENetAddress address;
	ENetEvent event;
	ENetPeer* peer;

	client = enet_host_create(NULL, 1, 1, 0, 0);
	enet_address_set_host(&address, "127.0.0.1");
	address.port = 8080;
	peer = enet_host_connect(client, &address, 1, 0);


	if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
		cout << "Connection succeeded." << endl;
	else
	{
		enet_peer_reset(peer);
		cout << "Connection failed." << endl;
	}

	thread msgLoopThread(MsgLoop, client);

	while (true)
	{
		string input;
		cin >> input;

		string message = username + " > " + input;

		SendPacket(peer, message.c_str());
	}

	msgLoopThread.join();
}