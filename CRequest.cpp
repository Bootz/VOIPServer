#include "CRequest.h"
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <iomanip>
#include <string>
#include <iostream>
#include <sstream>

CRequest::CRequest() : m_out_data(m_body_data, max_body_length)
{
	m_header_data[0] = 0;
	m_header_data[1] = 0;
	memset(m_header_data, 0, header_length);
	memset(m_body_data, 0, max_body_length);
	m_size = 0;
}

CRequest::~CRequest()
{
}

void		CRequest::clearHeader()
{
	memset(m_header_data, 0, header_length);
}

void		CRequest::clearBody()
{
	memset(m_body_data, 0, max_body_length);
	m_size = 0;
	m_out_data.clear();
	m_out_data.seekp(m_out_data.beg);
}

void		CRequest::clearAll()
{
	this->clearHeader();
	this->clearBody();
}

const char*	CRequest::getHeaderData() const
{
	return (m_header_data);
}

const char*	CRequest::getBodyData() const
{
	return (m_body_data);
}

char*	CRequest::getHeaderData()
{
	return (m_header_data);
}

char*	CRequest::getBodyData()
{
	return (m_body_data);
}

void	CRequest::setHeader(char type, char code)
{
	memset(m_header_data, 0, header_length);
	m_header_data[0] = type;
	m_header_data[1] = code;
}

bool	CRequest::decodeHeader()
{
	char			type;
	char			code;

	type = m_header_data[0];
	code = m_header_data[1];
	if (type < Protocol::BABELPROTO_REQ ||
		type >= Protocol::BABELPROTO_SIZE)
		return (false);
	if (code < Protocol::REQ_REGISTER ||
		code >= Protocol::REQ_SIZE)
		return (false);
	return (true);
}

char	CRequest::getRequestType()
{
	return (m_header_data[0]);
}

char	CRequest::getRequestCode()
{
	return (m_header_data[1]);
}

void	CRequest::setRequestType(char type)
{
	m_header_data[0] = type;
}

void	CRequest::setRequestCode(char code)
{
	m_header_data[1] = code;
}

void	CRequest::add(std::string data, int len)
{
	m_out_data.fill(0);
	m_out_data.setf(m_out_data.left);
	m_out_data << std::setw(len) << data;
	m_size += len;
}

void	CRequest::add(int data)
{
	m_out_data.write((char*)(&data), sizeof(int));
	m_size += sizeof(int);
}

void	CRequest::add(short data)
{
	m_out_data.write((char*)(&data), sizeof(short));
	m_size += sizeof(short);
}

void	CRequest::add(char data)
{
	m_out_data.fill(0);
	m_out_data.setf(m_out_data.left);
	m_out_data << std::setw(1) << data;
	m_size += 1;
}

int		CRequest::getBodySize() const
{
	return (m_size);
}
