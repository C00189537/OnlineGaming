#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "TcpListener.h"


CTcpListener::CTcpListener(std::string ipAddress, int port, MessageRecievedHandler handler)
	: m_ipAddress(ipAddress), m_port(port), MessageReceived(handler)
{
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		
		return;
	}

	// Create a socket
	m_listener = socket(AF_INET, SOCK_STREAM, 0);
	if (m_listener == INVALID_SOCKET)
	{
		
		return;
	}

	// Bind the ip address and port to a socket
	m_sockie.sin_addr.s_addr = inet_addr(m_ipAddress.c_str()); //Broadcast locally
	m_sockie.sin_port = htons(m_port); //Port
	m_sockie.sin_family = AF_INET; //IPv4 Socket

	bind(m_listener, (sockaddr*)&m_sockie, sizeof(m_sockie));

	// Tell Winsock the socket is for listening 
	listen(m_listener, SOMAXCONN);


	FD_ZERO(&m_master);

	FD_SET(m_listener, &m_master);

	m_active = true;
}

CTcpListener::~CTcpListener()
{
	Cleanup();
}

// Send a message to the specified client
void CTcpListener::Send(int clientSocket, std::string msg)
{
	send(clientSocket, msg.c_str(), msg.size() + 1, 0);
}

// Initialize winsock
bool CTcpListener::Init()
{
	WSAData data;
	WORD ver = MAKEWORD(2, 2);

	int wsInit = WSAStartup(ver, &data);
	// TODO: Inform caller the error that occured
	
	return wsInit == 0;
}

// The main processing loop
void CTcpListener::Run()
{
	while (m_active)
	{
		fd_set copy = m_master;

		// See who's talking to us
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		// Loop through all the current connections / potential connect
		for (int i = 0; i < socketCount; i++)
		{
			// Makes things easy for us doing this assignment
			SOCKET sock = copy.fd_array[i];

			// Is it an inbound communication?
			if (sock == m_listener)
			{
				// Accept a new connection
				SOCKET client = accept(m_listener, nullptr, nullptr);

				// Add the new connection to the list of connected clients
				FD_SET(client, &m_master);

				// Send a welcome message to the connected client
				std::string welcomeMsg = "Welcome to the Awesome Chat Server!\r\n";
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
			}
			else // It's an inbound message
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				// Receive message
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					// Drop the client
					closesocket(sock);
					FD_CLR(sock, &m_master);
				}
				else
				{
					// Check to see if it's a command. \quit kills the server
					if (buf[0] == '\\')
					{
						// Is the command quit? 
						std::string cmd = std::string(buf, bytesIn);
						if (cmd == "\\quit")
						{
							m_active = false;
							break;
						}

						// Unknown command
						continue;
					}

					// Send message to other clients, and definiately NOT the listening socket

					for (int i = 0; i < m_master.fd_count; i++)
					{
						SOCKET outSock = m_master.fd_array[i];
						if (outSock != m_listener && outSock != sock)
						{
							std::string strOut = "SOCKET #" + std::to_string(sock) + ": " + buf + "\r\n";

							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
				}
			}
		}
	}
	FD_CLR(m_listener, &m_master);
	closesocket(m_listener);

	std::string msg = "Connection Closed";
	while (m_master.fd_count > 0)
	{
		// Get the socket number
		SOCKET sock = m_master.fd_array[0];

		// Send the goodbye message
		send(sock, msg.c_str(), msg.size() + 1, 0);

		// Remove it from the master file list and close the socket
		FD_CLR(sock, &m_master);
		closesocket(sock);
	}
}

void CTcpListener::Cleanup()
{
	WSACleanup();
}

// Create a socket
SOCKET CTcpListener::CreateSocket()
{
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening != INVALID_SOCKET)
	{
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(m_port);
		inet_pton(AF_INET, m_ipAddress.c_str(), &hint.sin_addr);

		int bindOk = bind(listening, (sockaddr*)&hint, sizeof(hint));
		if (bindOk != SOCKET_ERROR)
		{
			int listenOk = listen(listening, SOMAXCONN);
			if (listenOk == SOCKET_ERROR)
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}

	return listening;
}

// Wait for a connection
SOCKET CTcpListener::WaitForConnection(SOCKET listening)
{
	SOCKET client = accept(listening, NULL, NULL);
	return client;
}