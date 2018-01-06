#include "accounts.hpp"

#include "accounts.storage.sqlite.hpp"
#include "password.hpp"

Accounts::Accounts(const AbstractAccountsStorage::shared_ptr& storage): m_storage(storage)
{
}

Accounts::~Accounts()
{
}

bool Accounts::save() const
{
	return true;
}

bool Accounts::load()
{
	const auto load_result = m_storage->populate(m_accounts);

	if (!load_result)
	{
		return false;
	}

	const auto init_result = build_indexes();
	return init_result;
}

Account::shared_ptr Accounts::get(const std::string& email) const
{
	const auto result = m_email_to_account_index.find(email);
	if (result == m_email_to_account_index.end())
	{
		return nullptr;
	}

	return *result->second;
}

bool Accounts::registered(const std::string& email) const
{
	return nullptr != get(email);
}

bool Accounts::check_password(const std::string& email, const std::string& password) const
{
	const auto account = get(email);
	if (account
		&& Password::compare_password(account->password(), password))
	{
		return true;
	}

	return false;
}

bool Accounts::add(const std::string& email, const std::string& password)
{
	const auto account = get(email);
	if (account)
	{
		return false;
	}

	const auto new_account = std::make_shared<Account>(email, password);
	m_accounts.push_back(new_account);
	m_email_to_account_index[email] = --m_accounts.end();
	return m_storage->add(new_account);
}

bool Accounts::build_indexes()
{
	m_email_to_account_index.clear();
	for (accounts_list_t::iterator i = m_accounts.begin(); i != m_accounts.end(); ++i)
	{
		const auto& account = *i;
		m_email_to_account_index.emplace(account->email(), i);
	}

	return true;
}

const char* ACCOUNTS_DATABASE_NAME = "accounts.sqlite";

Accounts accounts(std::make_shared<SQLiteAccountsStorage>(ACCOUNTS_DATABASE_NAME));

// vim: ts=4 sw=4 tw=0 noet syntax=cpp :
