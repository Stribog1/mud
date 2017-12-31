#ifndef __ACCOUNTS_HPP__
#define __ACCOUNTS_HPP__

#include <list>
#include <memory>

class Account
{
public:
	using shared_ptr = std::shared_ptr<Account*>;
	using player_uid_t = int;

	Account() {}

private:
	using players_list_t = std::list<player_uid_t>;	// list of players' UIDs

	std::string m_email;
	std::string m_password;

	/// The flag whether user confirmed ownership of his ID. Usually in order to do it some secret code
	/// is being sent to user's email. Then he must enter it to confirm email ownership.
	bool m_confirmed;

	/// List of players assigned to this account
	players_list_t m_players_list;
};

class Accounts
{
public:
	Accounts() {}

	bool save() const;
	bool load();

private:
	using accounts_list_t = std::list<Account::shared_ptr>;

	/// List of all game accounts. For now we completely keep it in the memory.
	accounts_list_t m_accounts;
};

#endif // __ACCOUNTS_HPP__

// vim: ts=4 sw=4 tw=0 noet syntax=cpp :