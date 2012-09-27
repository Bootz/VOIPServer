#ifndef CREQUEST_H
# define CREQUEST_H

# include <boost/asio.hpp>
# include <strstream>
# include <string>
# include "Protocol.h"

class CRequest
{

public:
	CRequest();
	~CRequest();
	
	enum { header_length = sizeof(Protocol::header) };
	enum { max_body_length = 128 };

	const char*					getHeaderData() const;
	char*						getHeaderData();	
	const char*					getBodyData() const;
	char*						getBodyData();
	std::string					getWriteString();

	char						getRequestType();
	char						getRequestCode();
	
	void						setRequestType(char);
	void						setRequestCode(char);

	bool						decodeHeader();
	void						setHeader(char, char);
	
	void						clearHeader();
	void						clearBody();
	void						clearAll();
	
	void						add(std::string, int);
	void						add(int data);
	void						add(short data);
	void						add(char data);
	
	int							getBodySize() const;
	
private:
	int							m_size;
	std::ostrstream				m_out_data;
	char						m_header_data[header_length];
	char						m_body_data[max_body_length];

};

#endif
