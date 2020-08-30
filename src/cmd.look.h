#ifndef BYLINS_CMD_LOOK_H
#define BYLINS_CMD_LOOK_H

#include <char.hpp>

void do_auto_exits(CHAR_DATA * ch);
void do_exits(CHAR_DATA *ch, char* /*argument*/, int/* cmd*/, int/* subcmd*/);
void do_blind_exits(CHAR_DATA *ch);
void do_look(CHAR_DATA *ch, char *argument, int/* cmd*/, int subcmd);
void do_looking(CHAR_DATA *ch, char* /*argument*/, int/* cmd*/, int/* subcmd*/);
void do_sides(CHAR_DATA *ch, char* /*argument*/, int/* cmd*/, int/* subcmd*/);
void do_examine(CHAR_DATA *ch, char *argument, int/* cmd*/, int subcmd);
void do_diagnose(CHAR_DATA *ch, char *argument, int/* cmd*/, int/* subcmd*/);
void do_zone(CHAR_DATA *ch, char* /*argument*/, int/* cmd*/, int/* subcmd*/);
void do_inventory(CHAR_DATA *ch, char* /*argument*/, int/* cmd*/, int/* subcmd*/);
void do_equipment(CHAR_DATA *ch, char *argument, int/* cmd*/, int/* subcmd*/);

#endif //BYLINS_CMD_LOOK_H

// vim: ts=4 sw=4 tw=0 noet syntax=cpp :
