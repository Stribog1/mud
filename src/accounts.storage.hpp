#ifndef __ACCOUNTS_STORAGE_HPP__
#define __ACCOUNTS_STORAGE_HPP__

#include "account.hpp"

class AbstractAccountsStorage
{
public:
	using shared_ptr = std::shared_ptr<AbstractAccountsStorage>;

	virtual ~AbstractAccountsStorage() {}

	virtual bool opened() const = 0;
	virtual bool open() = 0;
	virtual bool populate(accounts_list_t& list) = 0;
	virtual bool update(const Account::shared_ptr& account) = 0;
	virtual bool add(const Account::shared_ptr& account) = 0;
	virtual bool close() = 0;

	virtual const char* get_last_error() const { return nullptr; }
};

#endif // __ACCOUNTS_STORAGE_HPP__

// vim: ts=4 sw=4 tw=0 noet syntax=cpp :
