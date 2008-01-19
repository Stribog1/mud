// $RCSfile$     $Date$     $Revision$
// Copyright (c) 2008 Krodo
// Part of Bylins http://www.mud.ru

#ifndef GLORY_HPP_INCLUDED
#define GLORY_HPP_INCLUDED

#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "interpreter.h"

namespace Glory {

int get_glory(long uid);
void add_glory(long uid, int amount);
void remove_glory(long uid, int amount);

ACMD(do_spend_glory);
bool parse_spend_glory_menu(CHAR_DATA *ch, char *arg);
void spend_glory_menu(CHAR_DATA *ch);

void load_glory();
void save_glory();
void timers_update();

bool check_stats(CHAR_DATA *ch);
void calculate_total_stats(CHAR_DATA *ch);
void remove_stats(CHAR_DATA *ch, CHAR_DATA *god, int amount);
void transfer_stats(CHAR_DATA *ch, CHAR_DATA *god, std::string name, char *reason);
void show_glory(CHAR_DATA *ch, CHAR_DATA *god);
void show_stats(CHAR_DATA *ch);

void load_glory_log();
void save_glory_log();
void add_glory_log(int type, int num, std::string punish, std::string reason, CHAR_DATA *vict);
void show_glory(CHAR_DATA *ch ,char const * const value);
void print_glory_top(CHAR_DATA *ch);
void hide_char(CHAR_DATA *vict, CHAR_DATA *god, char const * const mode);

void set_freeze(long uid);
void remove_freeze(long uid);

} // namespace Glory

#endif // GLORY_HPP_INCLUDED
