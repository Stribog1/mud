#ifndef __ACCOUNTS_HPP__
#define __ACCOUNTS_HPP__

#include "account.hpp"
#include "accounts.storage.hpp"

#include <unordered_map>

class Accounts
{
public:
	Accounts(const AbstractAccountsStorage::shared_ptr& storage);
	~Accounts();

	bool save() const;
	bool load();

	Account::shared_ptr get(const std::string& email) const;
	bool registered(const std::string& email) const;
	bool check_password(const std::string& email, const std::string& password) const;
	bool add(const std::string& email, const std::string& password);

	const char* get_last_error() const { return m_storage->get_last_error(); }

private:
	using email_to_account_index_t = std::unordered_map<std::string, accounts_list_t::iterator>;

	bool build_indexes();

	/// List of all game accounts. For now we completely keep it in the memory.
	accounts_list_t m_accounts;
	email_to_account_index_t m_email_to_account_index;

	AbstractAccountsStorage::shared_ptr m_storage;
};

extern Accounts accounts;	// users's accounts

#endif // __ACCOUNTS_HPP__

// vim: ts=4 sw=4 tw=0 noet syntax=cpp :