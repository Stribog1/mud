#ifndef __ACCOUNTS_STORAGE_SQLITE_HPP__
#define __ACCOUNTS_STORAGE_SQLITE_HPP__

#include "accounts.storage.hpp"

#include <CppSQLite3.h>

class SQLiteAccountsStorage : public AbstractAccountsStorage
{
public:
	SQLiteAccountsStorage(const std::string& filename): m_filename(filename) {}

	virtual bool opened() const override { return nullptr != m_database; }
	virtual bool open() override;
	virtual bool populate(accounts_list_t& list) override;
	virtual bool update(const Account::shared_ptr& account) override;
	virtual bool add(const Account::shared_ptr& account) override;
	virtual bool close() override;

	virtual const char* get_last_error() const override;

private:
	void init();
	void set_last_error(const CppSQLite3Exception& e);

	std::shared_ptr<CppSQLite3DB> m_database;
	std::string m_filename;
	std::string m_last_error;
};

#endif // __ACCOUNTS_STORAGE_HPP__

// vim: ts=4 sw=4 tw=0 noet syntax=cpp :
