#include <iostream>
#include "TcpClient.h"

int main()
{
	TcpClient *client = new TcpClient((char*)HOST_ADDRESS, (char*)SERVER_PORT);
	client->start_session();
	return 0;
}  