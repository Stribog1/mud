#ifndef __ACCOUNT_HPP__
#define __ACCOUNT_HPP__

#include <time.h>

#include <list>
#include <memory>

class Account
{
public:
	using shared_ptr = std::shared_ptr<Account>;
	using player_uid_t = int;
	using created_t = time_t;
	using players_list_t = std::list<player_uid_t>;	// list of players' UIDs

	Account(const std::string& email, const std::string& password, const created_t created = time(0));

	const auto& email() const { return m_email; }
	const auto& password() const { return m_password; }
	auto created() const { return m_created; }
	void set_created(const int created) { m_created = created; }

	void attach_player(const player_uid_t player_uid);

	const auto& players() const { return m_players_list; }

private:
	std::string m_email;
	std::string m_password;

	created_t m_created;

	/// List of players assigned to this account
	players_list_t m_players_list;
};

using accounts_list_t = std::list<Account::shared_ptr>;

#endif // __ACCOUNT_HPP__

// vim: ts=4 sw=4 tw=0 noet syntax=cpp :
