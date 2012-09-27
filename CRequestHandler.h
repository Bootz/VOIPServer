#ifndef CREQUESTHANDLER_H
# define CREQUESTHANDLER_H

# include <map>
# include <string>
# include <list>

class CClient;
class CChannel;
class CRequest;

typedef std::list<CClient *> CLIENT_LIST;

class	CRequestHandler
{
public:
	/* Handles */
	void	handleReg(CClient *, CRequest *);
	void	handleUserInfo(CClient *, CRequest *);
	void	handleSetStatus(CClient *, CRequest *);
	void	handleWatchUser(CClient *, CRequest *);
	void	handleInvite(CClient *, CRequest *);
	void	handleAccept(CClient *, CRequest *);
	void	handleHangup(CClient *, CRequest *);
	void	handleSendText(CClient *, CRequest *);
	void	handleDataPort(CClient *, CRequest *);
	void	handleLogout(CClient *, CRequest *);
	/* Responses */
	void	respError(CClient *, CRequest *);
	void	respData(CClient *, CRequest *, int);
	void	respHeader(CClient *, CRequest *, char, char);
	/* Events */
	void	sendEvent(CRequest *, CLIENT_LIST&);
	void	sendEvent(CClient *, CRequest *, CLIENT_LIST&);
	
	
	void		addClient(const std::string &, CClient *);
	CClient*	getClient(const std::string &);
	CClient*	getClient(const char *);
	void		removeClient(const std::string &);
	
	void							addWatcher(std::string, CClient *);
	std::list<CClient *>&			getWatchedList(std::string);
	void							removeWatched(std::string , CLIENT_LIST&);
	
	int			createChannel(CClient *);
	void		deleteChannel(int);
	CChannel*	getChannel(int);
	CClient*	getChannelOwner(int);

protected:
	CRequestHandler();
	~CRequestHandler();

private:
	std::map<std::string, CClient *>			m_clients;
	std::map<int, CChannel *>					m_channels;
	std::map<std::string, std::list<CClient*> >	m_watched;

};

typedef struct	s_request_header
{
	int			size;
	void		(CRequestHandler::*f)(CClient *, CRequest *);
}				t_request_header;

#endif
