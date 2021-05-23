#include <iostream>
#include <list>
#include <enet/enet.hpp>

using namespace std;

list<ENetPeer*> peers;

void SendPackets(ENetEvent _event)
{
	cout << _event.packet->data << std::endl;

	for (ENetPeer* peer : peers)
	{
		ENetPacket* packet = enet_packet_create(_event.packet->data, _event.packet->dataLength, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet);
	}
}

int main()
{
	if (enet_initialize() != 0)
		cout << "Can't init enet!" << endl;

	ENetAddress address;
	ENetHost* server;
	ENetEvent event;

	address.host = ENET_HOST_ANY;
	address.port = 8080;
	server = enet_host_create(&address, 10, 1, 0, 0);

	cout << "Server is listening..." << endl;

	while (true)
	{
		while (enet_host_service(server, &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				cout << event.peer->address.host << " has connected!" << endl;
				peers.push_back(event.peer);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				cout << event.peer->address.host << " has disconnected!" << endl;
				peers.remove(event.peer);
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				SendPackets(event);
				break;
			}
		}
	}
}