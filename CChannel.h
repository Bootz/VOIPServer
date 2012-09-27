#ifndef CCHANNEL_H
# define CCHANNEL_H

# include <list>
# include <string>
# include "CRequestHandler.h"

class CClient;

class CChannel
{

	public:
		CChannel(CClient *);
		~CChannel();
		
		void			addParticipant(CClient *client);
		void			rmvParticipant(CClient *client);
		
		CLIENT_LIST&	getParticipantList();
		CClient*		getOwner() const;
	
	private:
		CLIENT_LIST				m_participants;
		CClient*				m_owner;

};

#endif
