#ifndef CCLIENT_H
# define CCLIENT_H

# include "Protocol.h"
# include <boost/asio.hpp>
# include <string>
# include <list>

class	CClient
{

	public:
		CClient(boost::asio::ip::tcp::socket *);
		~CClient();
		
		const std::string&				getLogin() const;
		bool							isLogged() const;
		boost::asio::ip::tcp::socket*	getSocket() const;
		bool							auth(const std::string&, const std::string&);
		
		char							getStatus();
		void							setStatus(int status);
		
	private:
		boost::asio::ip::tcp::socket*	m_controlSocket;
		bool							m_isLogged;
		std::string						m_login;
		Protocol::status				m_status;
};

#endif
