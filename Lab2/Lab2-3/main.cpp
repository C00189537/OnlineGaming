#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <string>

#include "TcpListener.h"
#include "Qotd.h"

using namespace std;


void Listener_MessageReceived(CTcpListener* listener, int client, string msg);

// This is kinda bad because it's global.
CQotd quotes("wisdom.txt");

void main()
{
	CTcpListener server("149.153.106.163", 5050, Listener_MessageReceived);

	if (server.Init())
	{
		server.Run();
	}

}

void Listener_MessageReceived(CTcpListener* listener, int client, string msg)
{
	if (msg == "QUOTE")
	{
		listener->Send(client, quotes.GetRandomQuote());
	}
	else
	{
		std::cout << msg << std::endl;
		listener->Send(client, msg);
	}
}
