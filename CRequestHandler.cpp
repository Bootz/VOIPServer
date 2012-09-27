#include "CRequestHandler.h"
#include "Protocol.h"
#include "CClient.h"
#include "CRequest.h"
#include "CConfig.h"
#include "CChannel.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <strstream>

using namespace boost;

CRequestHandler::CRequestHandler()
{
}

CRequestHandler::~CRequestHandler()
{
}

void		CRequestHandler::addClient(const std::string &name, CClient *client)
{
	if (this->getClient(name) == NULL)
	{
		m_clients[name] = client;
		m_watched[name];
	}
}

void		CRequestHandler::removeClient(const std::string &name)
{
	std::map<std::string, std::list<CClient *> >::iterator it;
	std::map<std::string, std::list<CClient *> >::iterator ite;

	it = m_watched.begin();
	ite = m_watched.end();
	while (it != ite)
	{
		this->removeWatched(name, it->second);
		++it;
	}
	if (m_clients.find(name) != m_clients.end())
		m_clients.erase(name);
	if (m_watched.find(name) != m_watched.end())
		m_watched.erase(name);
}

CClient*	CRequestHandler::getClient(const std::string &name)
{
	if (m_clients.find(name) == m_clients.end())
		return (NULL);
	return (m_clients[name]);
}

CClient*	CRequestHandler::getClient(const char* name)
{
	std::string	login(name, 16);

	if (m_clients.find(login) == m_clients.end())
		return (NULL);
	return (m_clients[login]);
}

/*
** REQUESTS TO SERVER
*/
using namespace Protocol;

void	CRequestHandler::handleReg(CClient *client, CRequest *request)
{
	Request::reg	*reg = reinterpret_cast<Request::reg *>(request->getBodyData());
	std::string		login(reg->login, 16);
	std::string		pass(reg->reg_string, 32);

	login.resize(strlen(login.c_str()));
	if (client->auth(login, pass) == true)
	{
		std::cout << "New user connected :[" << login << "]\n";
		this->addClient(login, client);
		this->respHeader(client, request, BABELPROTO_RESP, RES_REGISTER);
		request->setHeader(BABELPROTO_EVT, EVT_USERSTATUS);
		request->clearBody();
		request->add(client->getLogin(), 16);
		request->add(client->getStatus());
		this->sendEvent(request, this->getWatchedList(client->getLogin()));
	}
	else
		this->respError(client, request);
}

void	CRequestHandler::handleUserInfo(CClient *client, CRequest *request)
{
	Request::userinfo	*userinfo = reinterpret_cast<Request::userinfo *>(request->getBodyData());
	std::string			login(userinfo->login);
	CClient				*infoClient;

	login.resize(strlen(login.c_str()));
	request->clearBody();
	std::cout << "Server is asked information for :[" << login << "]\n";
	infoClient = getClient(login);
	if (infoClient)
	{
		this->respHeader(client, request, BABELPROTO_RESP, RES_USERINFO);
		request->clearBody();
		request->add(login, 16);
		request->add(infoClient->getSocket()->remote_endpoint().address().to_string(), 16);
		request->add(infoClient->getStatus());
		this->respData(client, request, sizeof(Protocol::Response::userinfo));
	}
	else if (CConfig::userExists(login))
	{
		this->respHeader(client, request, BABELPROTO_RESP, RES_USERINFO);
		request->clearBody();
		request->add(login, 16);
		request->add("", 16);
		request->add(Protocol::STATUS_OFFLINE);
		this->respData(client, request, sizeof(Protocol::Response::userinfo));
	}
	else
		this->respError(client, request);
}

void	CRequestHandler::handleSetStatus(CClient *client, CRequest *request)
{
	Request::setstatus	*setstatus = reinterpret_cast<Request::setstatus *>(request->getBodyData());

	std::cout << "Handling setstatus of [" << client->getLogin() << "]" << std::endl;
	client->setStatus(setstatus->status);

	request->clearBody();
	request->setHeader(BABELPROTO_EVT, EVT_USERSTATUS);
	request->add(client->getLogin(), 16);
	request->add(client->getStatus());
	this->sendEvent(request, this->getWatchedList(client->getLogin()));	
}

void	CRequestHandler::handleWatchUser(CClient *client, CRequest *request)
{
	Request::watchuser	*watchuser = reinterpret_cast<Request::watchuser *>(request->getBodyData());
	std::string			login(watchuser->login);

	login.resize(strlen(login.c_str()));
	if (CConfig::userExists(login))
		this->addWatcher(login, client);
}

void	CRequestHandler::handleInvite(CClient *client, CRequest *request)
{
	Request::invite		*invite = reinterpret_cast<Request::invite *>(request->getBodyData());
	std::string			login(invite->login);
	CClient				*toInvite;
	short				port = invite->port;

	login.resize(strlen(login.c_str()));
	if (invite->channelid == 0 && (toInvite = this->getClient(login)) &&
		login.compare(client->getLogin()) != 0)
	{
		int id = this->createChannel(client);
		this->respHeader(client, request, BABELPROTO_RESP, RES_INVITE);
		request->clearBody();
		request->add(id);
		this->respData(client, request, request->getBodySize());
		std::cout << "User:[" << login << "] has been invited." << std::endl;
		request->add(1); //nbUsers - todo: change if we want to have conferences
		request->add(client->getLogin(), 16);
		request->add(port);
		this->respHeader(toInvite, request, BABELPROTO_EVT, EVT_INVITE);
		this->respData(toInvite, request, request->getBodySize());
	}
	else
		this->respError(client, request);
}

void	CRequestHandler::handleAccept(CClient *client, CRequest *request)
{
	Request::accept		*accept = reinterpret_cast<Request::accept *>(request->getBodyData());
	CChannel			*chan = this->getChannel(accept->channelid);
	int					id = accept->channelid;
	short				port = accept->port;

	if (chan)
	{
		std::cout << "[" << client->getLogin() << "] Accepted invitation." << std::endl;
		request->setHeader(BABELPROTO_EVT, EVT_ACCEPT);
		request->clearBody();
		request->add(id);
		request->add(client->getLogin(), 16);
		request->add(port);
		this->sendEvent(request, chan->getParticipantList());
		chan->addParticipant(client);
	}
	else
		this->respError(client, request);
}

void	CRequestHandler::handleHangup(CClient *client, CRequest *request)
{
	Request::hangup		*hangup = reinterpret_cast<Request::hangup *>(request->getBodyData());
	CChannel			*chan = this->getChannel(hangup->channelid);
	int					id = hangup->channelid;

	if (chan)
	{
		std::cout << "[" << client->getLogin() << "] Hungup." << std::endl;
		chan->rmvParticipant(client);
		request->setHeader(BABELPROTO_EVT, EVT_HANGUP);
		request->clearBody();
		request->add(id);
		request->add(client->getLogin(), 16);
		this->sendEvent(request, chan->getParticipantList());
		if (chan->getParticipantList().size() == 0)
		{
			this->deleteChannel(hangup->channelid);
			delete chan;
		}
	}
}

void	CRequestHandler::handleSendText(CClient *client, CRequest *request)
{
	Request::sendtext	*sendtext = reinterpret_cast<Request::sendtext *>(request->getBodyData());	
	CChannel			*chan = this->getChannel(sendtext->channelid);
	int					id = sendtext->channelid;
	int					len = sendtext->msgLen;

	if (chan)
	{
		char *data = new char[len];
		asio::read(*(client->getSocket()), asio::buffer(data, len));
		request->setHeader(BABELPROTO_EVT, EVT_TEXTMSG);
		request->clearBody();
		request->add(id);
		request->add(client->getLogin(), 16);
		request->add(len);
		request->add(data, len);
		this->sendEvent(client, request, chan->getParticipantList());
		delete data;
	}
}

void	CRequestHandler::handleDataPort(CClient *client, CRequest *request)
{
	Request::dataport	*dataport = reinterpret_cast<Request::dataport *>(request->getBodyData());
	CChannel			*chan = this->getChannel(dataport->channelid);
	int					id = dataport->channelid;
	std::string			login(dataport->login);
	short				port = dataport->port;

	client = client;
	if (chan)
	{
		request->setHeader(BABELPROTO_EVT, EVT_DATAPORT);
		request->clearBody();
		request->add(id);
		request->add(login, 16);
		request->add(port);
		this->sendEvent(request, chan->getParticipantList());
	}	
}

void	CRequestHandler::handleLogout(CClient *client, CRequest *request)
{
	client->setStatus(STATUS_OFFLINE);
	request->clearBody();
	request->setHeader(BABELPROTO_EVT, EVT_USERSTATUS);
	request->add(client->getLogin(), 16);
	request->add(client->getStatus());
	this->sendEvent(client, request, this->getWatchedList(client->getLogin()));	
	this->removeClient(client->getLogin());
}

/*
** RESPONSES FROM SERVER
*/

void	CRequestHandler::sendEvent(CRequest *request, CLIENT_LIST& list)
{
	CLIENT_LIST::iterator it;
	CLIENT_LIST::iterator ite;

	it = list.begin();
	ite = list.end();
	std::cout << "Sending new event to:" << std::endl;
	while (it != ite)
	{
		if (this->getClient((*it)->getLogin()))
		{
			std::cout << "-- [" << (*it)->getLogin() << "]" << std::endl;
			this->respHeader((*it), request, request->getRequestType(), request->getRequestCode());
			this->respData((*it), request, request->getBodySize());
		}
		else
			it = list.erase(it);
		if (it != ite)
			++it;
	}
}

void	CRequestHandler::sendEvent(CClient *owner, CRequest *request, CLIENT_LIST& list)
{
	CLIENT_LIST::iterator it;
	CLIENT_LIST::iterator ite;

	it = list.begin();
	ite = list.end();
	std::cout << "Sending new event to:" << std::endl;
	while (it != ite)
	{
		if ((*it)->getLogin() != owner->getLogin())
		{
			if (this->getClient((*it)->getLogin()))
			{
				std::cout << "-- [" << (*it)->getLogin() << "]" << std::endl;
				this->respHeader((*it), request, request->getRequestType(), request->getRequestCode());
				this->respData((*it), request, request->getBodySize());
			}
			else
				it = list.erase(it);
		}
		if (it != ite)
			++it;
	}
}

void	CRequestHandler::respError(CClient *client, CRequest *request)
{
	char					newReqCode;

	newReqCode = request->getRequestCode();
	this->respHeader(client, request, BABELPROTO_RESP, (-newReqCode));
	request->clearBody();
	request->add(42);
	this->respData(client, request, sizeof(Protocol::Response::error));
}

void	CRequestHandler::respHeader(CClient *client, CRequest *req, char requestType, char requestCode)
{
	req->setHeader(requestType, requestCode);
	asio::write(*(client->getSocket()), asio::buffer(req->getHeaderData(), sizeof(Protocol::header)));
}

void	CRequestHandler::respData(CClient *client, CRequest *req, int size)
{
	asio::write(*(client->getSocket()), asio::buffer(req->getBodyData(), size));
}


/*
** CHANNEL HANDLING
*/
int				CRequestHandler::createChannel(CClient *owner)
{
	int			nextId;
	CChannel	*channel;

	nextId = m_channels.size() + 1;
	channel = new CChannel(owner);
	m_channels[nextId] = channel;
	return (nextId);
}

void			CRequestHandler::deleteChannel(int id)
{
	if (this->getChannel(id))
		m_channels.erase(id);
}

CChannel*		CRequestHandler::getChannel(int id)
{
	if (m_channels.find(id) == m_channels.end())
		return (NULL);
	return (m_channels[id]);
}

CClient*		CRequestHandler::getChannelOwner(int id)
{
	if (m_channels.find(id) == m_channels.end())
		return (NULL);
	return (m_channels[id]->getOwner());
}

void			CRequestHandler::addWatcher(std::string name, CClient *toWatch)
{
	(m_watched[name]).push_front(toWatch);
}

void			CRequestHandler::removeWatched(std::string name, CLIENT_LIST& list)
{
	CLIENT_LIST::iterator it = list.begin();
	CLIENT_LIST::iterator ite = list.end();

	while (it != ite)
	{
		if ((*it)->getLogin() == name)
			it = list.erase(it);
		if (it != ite)
			++it;
	}
}

CLIENT_LIST&	CRequestHandler::getWatchedList(std::string name)
{
	return (m_watched[name]);
}
