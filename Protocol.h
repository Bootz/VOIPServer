#ifndef NPROTOCOL_H
# define NPROTOCOL_H

namespace Protocol
{
	enum	reqType
	{
		BABELPROTO_REQ = 1,
		BABELPROTO_RESP = 2,
		BABELPROTO_EVT = 3,
		BABELPROTO_SIZE
	};

	enum	reqCode
	{
		REQ_REGISTER = 1,
		REQ_USERINFO = 2,
		REQ_SETSTATUS = 3,
		REQ_WATCHUSER = 4,
		REQ_INVITE = 5,
		REQ_ACCEPT = 6,
		REQ_HANGUP = 7,
		REQ_SENDTEXTMSG = 8,
		REQ_SETDATAPORT = 9,
		REQ_LOGOUT = 10,
		REQ_SIZE
	};

	enum	respCode
	{
		RES_BADREQUEST = 0,
		RES_REGISTER = 1,
		RES_USERINFO = 2,
		RES_INVITE = 3,
		RES_ACCEPT = 4
	};

	enum	evtCode
	{
		EVT_INVITE = 1,
		EVT_ACCEPT = 2,
		EVT_HANGUP = 3,
		EVT_USERSTATUS = 4,
		EVT_TEXTMSG = 5,
		EVT_DATAPORT = 6
	};

	enum	status
	{
		STATUS_ONLINE = 1,
		STATUS_BUSY = 2,
		STATUS_AWAY = 3,
		STATUS_OFFLINE = 4
	};

	struct	header
	{
		char	type;
		char	code;
	};

	namespace Request
	{
		enum {Type = BABELPROTO_REQ};
		struct		reg{char	login[16]; char	reg_string[32];};
		struct		userinfo{char	login[16];};
		struct		setstatus{char	status;	};
		struct		watchuser{char	login[16];};
		struct		invite{char	login[16]; int	channelid; short port;};
		struct		accept{int	channelid; short port;};
		struct		hangup{int	channelid;};
		struct		sendtext{int	channelid; int	msgLen;};
		struct		dataport{int	channelid; char	login[16]; short port;};
	}
	namespace Response
	{
		enum {Type = BABELPROTO_RESP};
		struct		error{int	err_code;};
		struct		userinfo{char	login[16]; char ip[16]; char	status;};
		struct		invite{int	channelid;};
	}
	namespace Event
	{
		enum {Type = BABELPROTO_EVT};
		struct		invite{int	channelid; int	nbUsers;};
		struct		chanuser{char	login[16]; short	port;};
		struct		accept{int	channelid; char	login[16]; short port;};
		struct		hangup{int	channelid; char	login[16];};
		struct		setstatus{char	login[16]; char status;};
		struct		sendtext{int	channelid; char	login[16]; int msgLen;};
		struct		dataport{int channelid; char login[16]; short port;};
	}

}

#endif
