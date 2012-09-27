#include "CConfig.h"
#include "CServer.h"
#include "CClient.h"
#include <iostream>
#include <fstream>
#include <string>

std::map<std::string, std::string> CConfig::m_users;

bool	CConfig::processFile(const std::string &fileName)
{
	std::ifstream		ifs;
	std::string		line;
	std::string		user;
	std::string		password;
	int			x;
	size_t			pos;

	x = 0;
	ifs.open(fileName.c_str());
	if (!ifs)
		return (false);
	while (!ifs.eof())
	{
		getline(ifs, line);
		if ((pos = line.find_first_of("\r")) != std::string::npos)
		  line.erase(pos, line.length());
		x = line.find(":");
		if (x)
		{
			user = line.substr(0, x);
			password = line.substr(x+1, line.length());
			m_users[user] = password;
		}
	}
	return (true);
}

bool	CConfig::userExists(const std::string &name, const std::string &pass)
{
	if (m_users.find(name) == m_users.end())
		return (false);
	if (m_users[name] != pass)
		return (false);
	return (true);
}

bool	CConfig::userExists(const std::string &name)
{
	if (m_users.find(name) == m_users.end())
		return (false);
	return (true);
}
