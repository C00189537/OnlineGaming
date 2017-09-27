#include <WinSock2.h>
#include "Net.h"


void playerA(Net* net)
{
	net->setupUDP(28000, "127.0.0.1");
	std::cout << std::endl << "IP: 127.0.0.1" << std::endl << "Port: 28000" << std::endl;
}
void playerB(Net* net)
{
	net->setupUDP(28001, "127.0.0.1");
	std::cout << std::endl << "IP: 127.0.0.1" << std::endl << "Port: 28001" << std::endl;
}
int main()
{
	Net net;
	Net* skynet = &net;
	bool loop = true;
	net.initialise();

	std::string input;
	std::cin >> input;

	if (input == "a")
	{
		playerA(skynet);
		
	}
	else if (input == "b")
	{
		playerB(skynet);
		
	}
	
		if (input == "a")
		{
	
			skynet->sendData("127.0.0.1", 28001, "hello");
			
		}
		else if (input == "b")
		{
			char message[200] = "aaa";
			system("PAUSE");
			net.receiveData(message);
			std::cout << message << std::endl;
		}
	system("PAUSE");
}

