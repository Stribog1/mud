#ifndef BYLINS_ACT_INFORMATIVE_H
#define BYLINS_ACT_INFORMATIVE_H

#include <obj.hpp>
#include <screen.h>

#define EXIT_SHOW_LOOKING (1 << 1)
#define EXIT_SHOW_WALL    (1 << 0)

#define TAG_NIGHT       "<night>"
#define TAG_DAY         "<day>"
#define TAG_WINTERNIGHT "<winternight>"
#define TAG_WINTERDAY   "<winterday>"
#define TAG_SPRINGNIGHT "<springnight>"
#define TAG_SPRINGDAY   "<springday>"
#define TAG_SUMMERNIGHT "<summernight>"
#define TAG_SUMMERDAY   "<summerday>"
#define TAG_AUTUMNNIGHT "<autumnnight>"
#define TAG_AUTUMNDAY   "<autumnday>"

#define MAX_FIRES 6
extern const char *Fires[MAX_FIRES];
extern const char *class_name[];
extern const char *ac_text[];
extern const char *weapon_class[];
extern const char *ObjState[8][2];
extern const char *Dirs[NUM_OF_DIRS + 1];
extern const char *Locks[4][2];

std::string space_before_string(char const *text);
std::string space_before_string(std::string text);

const char *diag_obj_timer(const OBJ_DATA * obj);
const char *show_obj_to_char(OBJ_DATA * object, CHAR_DATA * ch, int mode, int show_state, int how);
void obj_info(CHAR_DATA * ch, OBJ_DATA *obj, char buf[MAX_STRING_LENGTH]);

void do_quest(CHAR_DATA *ch, char *argument, int cmd, int subcmd);

void do_affects(CHAR_DATA *ch, char *argument, int cmd, int subcmd);
void do_gold(CHAR_DATA *ch, char *argument, int cmd, int subcmd);
void do_score(CHAR_DATA *ch, char *argument, int cmd, int subcmd);
void do_time(CHAR_DATA *ch, char *argument, int cmd, int subcmd);
void do_weather(CHAR_DATA *ch, char *argument, int cmd, int subcmd);
void do_who(CHAR_DATA *ch, char *argument, int cmd, int subcmd);
void do_users(CHAR_DATA *ch, char *argument, int cmd, int subcmd);
void do_gen_ps(CHAR_DATA *ch, char *argument, int cmd, int subcmd);
void perform_mortal_where(CHAR_DATA * ch, char *arg);
void perform_immort_where(CHAR_DATA * ch, char *arg);
void do_where(CHAR_DATA *ch, char *argument, int cmd, int subcmd);
void do_levels(CHAR_DATA *ch, char *argument, int cmd, int subcmd);
void do_consider(CHAR_DATA *ch, char *argument, int cmd, int subcmd);
void do_diagnose(CHAR_DATA *ch, char *argument, int cmd, int subcmd);
void do_toggle(CHAR_DATA *ch, char *argument, int cmd, int subcmd);
void do_commands(CHAR_DATA *ch, char *argument, int cmd, int subcmd);
void do_hearing(CHAR_DATA *ch, char *argument, int cmd, int subcmd);
void do_check(CHAR_DATA *ch, char *argument, int cmd, int subcmd);
void do_cities(CHAR_DATA *ch, char*, int, int);
void look_at_char(CHAR_DATA * i, CHAR_DATA * ch);
void look_in_direction(CHAR_DATA * ch, int dir, int info_is);
char *find_exdesc(char *word, const EXTRA_DESCR_DATA::shared_ptr& list);
void gods_day_now(CHAR_DATA * ch);
void do_blind_exits(CHAR_DATA *ch);






#endif //BYLINS_ACT_INFORMATIVE_H

// vim: ts=4 sw=4 tw=0 noet syntax=cpp :
