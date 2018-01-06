#include "accounts.storage.sqlite.hpp"

#include "account.hpp"

#include <unordered_map>
#include <sstream>

bool SQLiteAccountsStorage::open()
{
	if (m_database)
	{
		return false;
	}

	try
	{
		m_database = std::make_shared<CppSQLite3DB>();
		m_database->open(m_filename.c_str());

		init();
	}
	catch (const CppSQLite3Exception& e)
	{
		set_last_error(e);
		return false;
	}

	return true;
}

bool SQLiteAccountsStorage::populate(accounts_list_t& list)
{
	list.clear();

	if (!opened()
		&& !open())
	{
		return false;
	}

	try
	{
		using accounts_lookup_t = std::unordered_map<sqlite_int64, Account*>;

		auto accounts_query = m_database->execQuery("select * from accounts;");
		accounts_lookup_t accounts_lookup;

		while (!accounts_query.eof())
		{
			const auto id = accounts_query.getInt64Field("id");
			const auto email = accounts_query.getStringField("email");
			const auto password = accounts_query.getStringField("password");
			const auto created = accounts_query.getInt64Field("created");

			const auto account = std::make_shared<Account>(email, password, static_cast<Account::created_t>(created));

			accounts_lookup.emplace(id, account.get());
			list.push_back(account);

			accounts_query.nextRow();
		}

		auto players_query = m_database->execQuery("select * from players_ownership;");
		while (!players_query.eof())
		{
			const auto id = players_query.getInt64Field("id");
			const auto player_uid = players_query.getInt64Field("player_uid");

			const auto& account = accounts_lookup.at(id);
			account->attach_player(static_cast<Account::player_uid_t>(player_uid));

			players_query.nextRow();
		}
	}
	catch (const CppSQLite3Exception& e)
	{
		set_last_error(e);
		return false;
	}

	return true;
}

namespace
{
	class Transaction
	{
	public:
		Transaction(CppSQLite3DB& database) : m_database(database) {}
		void begin() { m_database.execDML("begin transaction;"); }
		void commit() { m_database.execDML("end transaction;"); }
		void rollback() { m_database.execDML("rollback transaction;"); }

	private:
		CppSQLite3DB& m_database;
	};
}

bool SQLiteAccountsStorage::update(const Account::shared_ptr& account)
{
	if (!opened()
		&& !open())
	{
		return false;
	}
	
	Transaction transaction(*m_database);

	try
	{
		transaction.begin();

		auto statement = m_database->compileStatement("update accounts set password=? where email=?;");
		statement.bind(1, account->password().c_str());
		statement.bind(2, account->email().c_str());
		statement.execDML();

		transaction.commit();
	}
	catch (const CppSQLite3Exception& e)
	{
		set_last_error(e);

		try { transaction.rollback(); }
		catch (const CppSQLite3Exception&) {}

		return false;
	}

	return true;
}

bool SQLiteAccountsStorage::add(const Account::shared_ptr& account)
{
	if (!opened()
		&& !open())
	{
		return false;
	}

	Transaction transaction(*m_database);

	try
	{
		transaction.begin();

		auto statement = m_database->compileStatement("insert into accounts (email, password, created) values (?, ?, ?);");
		statement.bind(1, account->password().c_str());
		statement.bind(2, account->email().c_str());
		statement.bind(3, static_cast<sqlite_int64>(account->created()));
		statement.execDML();

		transaction.commit();
	}
	catch (const CppSQLite3Exception& e)
	{
		set_last_error(e);

		try { transaction.rollback(); }
		catch (const CppSQLite3Exception&) {}

		return false;
	}

	return true;
}

bool SQLiteAccountsStorage::close()
{
	try
	{
		m_database.reset();
	}
	catch (const CppSQLite3Exception& e)
	{
		set_last_error(e);
		return false;
	}

	return true;
}

const char* SQLiteAccountsStorage::get_last_error() const
{
	return m_last_error.c_str();
}

void SQLiteAccountsStorage::init()
{
	if (0 == m_database->execScalar("SELECT count(*) FROM sqlite_master WHERE type='table' AND "
		"(name='accounts' OR name='players_ownership');"))
	{
		// TODO: add automatic creation of the database structure
	}
}

void SQLiteAccountsStorage::set_last_error(const CppSQLite3Exception& e)
{
	std::stringstream ss;

	const auto error_code = e.errorCode();
	ss << "SQLiteAccountsStorage, SQLite error #" << e.errorCodeAsString(error_code) << "(" << error_code << "): "
		<< e.errorMessage();

	m_last_error = ss.str();
}

// vim: ts=4 sw=4 tw=0 noet syntax=cpp :
