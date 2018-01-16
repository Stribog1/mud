#ifndef __ACCOUNT_MENU_FSM_HPP__
#define __ACCOUNT_MENU_FSM_HPP__

class DESCRIPTOR_DATA;	// to avoid inclusion of "structs.h"

class AccountMenuFSM
{
public:
	enum State
	{
		INITIAL		// Prints main menu, lists players and shows currently selected player
	};

	AccountMenuFSM(DESCRIPTOR_DATA* descriptor);

	void handle_input(const char* input);

private:
	void show_menu();

	DESCRIPTOR_DATA* m_descriptor;

	State m_state;
};

#endif // __ACCOUNT_MENU_FSM_HPP__

// vim: ts=4 sw=4 tw=0 noet syntax=cpp :

