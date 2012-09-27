#include "CServer.h"
#include "CConfig.h"
#include "CRequestHandler.h"
#include <boost/asio.hpp>
#include <iostream>
#include <string>

using namespace boost;

int		main()
{
	int					port = 1164;
	asio::io_service	io_service;
	CServer*			srv;

	srv = new CServer(io_service, port);
	if (CConfig::processFile("users.txt") == false)
	{
		std::cout << "The users.txt file could not be opened." << std::endl;
		return (-1);
	}
	
	io_service.run();
	return (0);
}
