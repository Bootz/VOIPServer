#include "CChannel.h"
#include "CClient.h"
#include "CRequestHandler.h"
#include <string>
#include <list>

CChannel::CChannel(CClient *owner)
{
	m_owner = owner;
	m_participants.push_front(owner);
}

CChannel::~CChannel()
{
}

void		CChannel::addParticipant(CClient *client)
{
	m_participants.push_back(client);
}

void		CChannel::rmvParticipant(CClient *client)
{
	CLIENT_LIST::iterator	it = m_participants.begin();
	CLIENT_LIST::iterator	ite = m_participants.end();

	while (it != ite)
	{
		if ((*it) == client)
			it = m_participants.erase(it);
		if (it != ite)
			++it;
	}
}

CClient*	CChannel::getOwner() const
{
	return (m_owner);
}

CLIENT_LIST&	CChannel::getParticipantList()
{
	return (m_participants);
}
