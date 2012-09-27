#ifndef CSERVER_H
# define CSERVER_H

# include <boost/asio.hpp>
# include "CRequestHandler.h"

class CClient;
class CRequest;

class	CServer : private CRequestHandler
{

public:
	CServer(boost::asio::io_service&, int);

	void	handleClientAccept(const boost::system::error_code&, boost::asio::ip::tcp::socket *);
	void	handleRequestHeader(const boost::system::error_code& ec, CClient *, char *);
	void	handleRequestBody(const boost::system::error_code& ec, CClient *, CRequest *);

	void	asyncAccept();
	void	disconnectClient(CClient *);

private:
	int								m_port;
	boost::asio::io_service&		m_io;
	boost::asio::ip::tcp::acceptor	m_acceptor;

};

#endif
