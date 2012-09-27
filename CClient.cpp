#include "CClient.h"
#include "CConfig.h"
#include "Protocol.h"
#include <boost/asio.hpp>
#include <string>

using namespace boost;

using asio::ip::tcp;
using asio::ip::udp;

CClient::CClient(tcp::socket *clientSocket)
{
	m_controlSocket = clientSocket;
	m_isLogged = false;
	m_status = Protocol::STATUS_OFFLINE;
}

const std::string&	CClient::getLogin() const
{
	return (m_login);
}

bool				CClient::isLogged() const
{
	return (m_isLogged);
}

tcp::socket*		CClient::getSocket() const
{
	return (m_controlSocket);
}

bool	CClient::auth(const std::string &login, const std::string &md5)
{
	if (CConfig::userExists(login, md5))
	{
		m_login = login;
		m_isLogged = true;
		m_status = Protocol::STATUS_ONLINE;
		return (true);
	}
	return (false);
}

char		CClient::getStatus()
{
	return (m_status);
}

void	CClient::setStatus(int status)
{
	m_status = static_cast<Protocol::status>(status);
}
