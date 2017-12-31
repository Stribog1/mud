#include "account.hpp"

Account::Account(const std::string& email, const std::string& password, const created_t created):
	m_email(email),
	m_password(password),
	m_created(created)
{
}

void Account::attach_player(const player_uid_t player_uid)
{
	m_players_list.push_back(player_uid);
}

// vim: ts=4 sw=4 tw=0 noet syntax=cpp :
