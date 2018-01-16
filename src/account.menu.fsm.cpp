#include "account.menu.fsm.hpp"

#include "structs.h"

AccountMenuFSM::AccountMenuFSM(DESCRIPTOR_DATA* descriptor): m_state(INITIAL), m_descriptor(descriptor)
{
}

void AccountMenuFSM::handle_input(const char* input)
{
	switch (m_state)
	{
	case INITIAL:
		break;

	default:
		break;
	}
}

void AccountMenuFSM::show_menu()
{
}

// vim: ts=4 sw=4 tw=0 noet syntax=cpp :
