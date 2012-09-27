#ifndef CCONFIG_H
# define CCONFIG_H

# include <string>
# include <map>

class CConfig
{
public:
	static bool		processFile(const std::string &);
	static bool		userExists(const std::string &, const std::string &);
	static bool		userExists(const std::string &);

private:
	static	std::map<std::string, std::string>		m_users;
};

#endif
