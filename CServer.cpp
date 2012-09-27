#include "CServer.h"
#include "CClient.h"
#include "CRequest.h"
#include "Protocol.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>

using namespace boost;

using asio::ip::tcp;
using asio::ip::udp;

static const t_request_header				gl_requestHeader[] =
{
	{0, 0},
	{sizeof(Protocol::Request::reg), &CRequestHandler::handleReg},
	{sizeof(Protocol::Request::userinfo), &CRequestHandler::handleUserInfo},
	{sizeof(Protocol::Request::setstatus), &CRequestHandler::handleSetStatus},
	{sizeof(Protocol::Request::watchuser), &CRequestHandler::handleWatchUser},
	{sizeof(Protocol::Request::invite), &CRequestHandler::handleInvite},
	{sizeof(Protocol::Request::accept), &CRequestHandler::handleAccept},
	{sizeof(Protocol::Request::hangup), &CRequestHandler::handleHangup},
	{sizeof(Protocol::Request::sendtext), &CRequestHandler::handleSendText},
	{sizeof(Protocol::Request::dataport), &CRequestHandler::handleDataPort},
	{0, &CRequestHandler::handleLogout},
	{0, 0}
};

CServer::CServer(asio::io_service& io, int port) :	m_port(port), m_io(io), m_acceptor(m_io, tcp::endpoint(tcp::v4(), m_port))
{
	this->asyncAccept();
}


void	CServer::handleClientAccept(const boost::system::error_code& ec, tcp::socket *newSocket)
{
	if (!ec)
	{
		CClient		*newClient = new CClient(newSocket);
		char		*data = new char[CRequest::header_length];

		asio::async_read(*newSocket, asio::buffer(data, CRequest::header_length),
			bind(&CServer::handleRequestHeader, this, asio::placeholders::error, newClient, data));		
		this->asyncAccept();
	}
}

void	CServer::handleRequestHeader(const boost::system::error_code& ec, CClient *client, char *data)
{
	if (!ec)
	{
		CRequest	*request = new CRequest();
		request->setHeader(data[0], data[1]);
		delete data;
		if (request->decodeHeader())
		{
		  asio::async_read(*(client->getSocket()), asio::buffer(request->getBodyData(), gl_requestHeader[(int)request->getRequestCode()].size),
							bind(&CServer::handleRequestBody, this, asio::placeholders::error, client, request));
		}
		else
		{
			request->setRequestCode(Protocol::RES_BADREQUEST);
			this->respError(client, request);
		}
	}
	else
		this->disconnectClient(client);
}

void	CServer::handleRequestBody(const boost::system::error_code& ec, CClient *client, CRequest *request)
{
	if (!ec)
	{
	  (this->*gl_requestHeader[(int)request->getRequestCode()].f)(client, request);
		delete request;
		char *data = new char[CRequest::header_length];
		asio::async_read(*(client->getSocket()), asio::buffer(data, CRequest::header_length),
			bind(&CServer::handleRequestHeader, this, asio::placeholders::error, client, data));
	}
	else
	{
		this->disconnectClient(client);
		delete request;
	}
}

void	CServer::asyncAccept()
{
	tcp::socket *newSocket = new tcp::socket(m_io);
	m_acceptor.async_accept(*newSocket, bind(&CServer::handleClientAccept, this, asio::placeholders::error, newSocket));
}

void	CServer::disconnectClient(CClient *client)
{
	if (client->isLogged())
	{
		std::cerr << "[" << client->getLogin() << "]: Disconnect - ";
		this->removeClient(client->getLogin());
	}
	std::cerr << "Connection closed." << std::endl;
	client->getSocket()->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	client->getSocket()->close();
}
