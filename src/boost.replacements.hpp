#ifndef __BOOST_REPLACEMENTS__HPP__
#define __BOOST_REPLACEMENTS__HPP__

#include <string>
#include <sstream>

#include <cctype>

namespace boost
{
	bool starts_with(const ::std::string& string, const char* what)
	{
		return 0 == string.find(what);
	}

	auto is_any_of(const char* set)
	{
		return [=](const char c) -> bool
		{
			auto it = set;
			while (it != nullptr)
			{
				if (c == *it)
				{
					return true;
				}

				++it;
			}

			return false;
		};
	}

	template <typename AppendableT, typename Pred>
	void split(AppendableT& result, const std::string& input, Pred pred)
	{
		auto it = input.begin();
		auto iend = input.end();
		auto start = iend;
		while (it != iend)
		{
			if (!pred(*it))
			{
				start = it;
			}
			else if (iend != start)
			{
				result.push_back(std::string(start, it));
				start = iend;
			}
		}

		if (iend != start)
		{
			result.push_back(std::string(start, iend));
		}
	}

	template <typename Storage>
	class dynamic_bitset
	{
	};

	template <typename Pred>
	void trim_right_if(::std::string& string, Pred pred)
	{
		auto begin = string.begin();
		auto end = string.end();
		while (end != begin)
		{
			if (!pred(*(--end)))
			{
				++end;
				break;
			}
		}

		string.erase(end, string.end());
	}

	template <typename Pred>
	void trim_left_if(::std::string& string, Pred pred)
	{
		auto begin = string.begin();
		auto it = begin;
		auto end = string.end();
		while (it != end && pred(*it));
		string.erase(begin, it);
	}

	template <typename Pred>
	void trim_if(::std::string& string, Pred pred)
	{
		trim_right_if(string, pred);
		trim_left_if(string, pred);
	}

	void trim(::std::string& string)
	{
		trim_if(string, std::isspace);
	}

	template <typename T>
	struct dummy
	{
	};

	template <typename T>
	std::string to_string(const T& what)
	{
		std::stringstream ss;
		ss << what;
		return ss.str();
	}

	template <typename ToT, typename FromT>
	ToT lexical_cast(const FromT&)
	{
		dummy<T>::bad_call();
	}

	template <>	std::string lexical_cast<std::string, int>(const int& from) { return to_string(from); }
	template <>	std::string lexical_cast<std::string, ::std::size_t>(const ::std::size_t& from) { return to_string(from); }
}

#endif // __BOOST_REPLACEMENTS__HPP__

// vim: ts=4 sw=4 tw=0 noet syntax=cpp :
