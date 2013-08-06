// $RCSfile$     $Date$     $Revision$
// Copyright (c) 2013 Krodo
// Part of Bylins http://www.mud.ru

#include "conf.h"
#include <map>
#include <sstream>
#include <iomanip>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include "boost/multi_array.hpp"

#include "map.hpp"
#include "utils.h"
#include "screen.h"
#include "room.hpp"
#include "db.h"
#include "char_player.hpp"
#include "handler.h"
#include "shop_ext.hpp"

SPECIAL(shop_keeper);
SPECIAL(ShopExt::shop_ext);
SPECIAL(receptionist);
SPECIAL(postmaster);
SPECIAL(bank);
SPECIAL(exchange);
SPECIAL(horse_keeper);
SPECIAL(guild_mono);
SPECIAL(guild_poly);

namespace MapSystem
{

// ������ ���� ��� ���������
const int MAX_LINES = 25;
const int MAX_LENGHT = 50;
// ������� �������� �� ��������
const int MAX_DEPTH_ROOMS = 5;

// ���� ��� ���������
//int screen[MAX_LINES][MAX_LENGHT];
boost::multi_array<int, 2> screen(boost::extents[MAX_LINES][MAX_LENGHT]);
// ����� ���� ��� �������� ������� ������� ��������� �� ������ �����������
// ������������ ��� ������� ��������� ������ ���� �� �����, � ���� ������
// ������� �������� ����� ������� � ��� ��������� ����� ���
//int depths[MAX_LINES][MAX_LENGHT];
boost::multi_array<int, 2> depths(boost::extents[MAX_LINES][MAX_LENGHT]);

enum
{
	// ��������� ������
	SCREEN_Y_OPEN,
	// �������� �����
	SCREEN_Y_DOOR,
	// ������� ������ (����)
	SCREEN_Y_HIDE,
	// ��� �������
	SCREEN_Y_WALL,
	SCREEN_X_OPEN,
	SCREEN_X_DOOR,
	SCREEN_X_HIDE,
	SCREEN_X_WALL,
	SCREEN_UP_OPEN,
	SCREEN_UP_DOOR,
	SCREEN_UP_HIDE,
	SCREEN_UP_WALL,
	SCREEN_DOWN_OPEN,
	SCREEN_DOWN_DOOR,
	SCREEN_DOWN_HIDE,
	SCREEN_DOWN_WALL,
	SCREEN_Y_UP_OPEN,
	SCREEN_Y_UP_DOOR,
	SCREEN_Y_UP_HIDE,
	SCREEN_Y_UP_WALL,
	SCREEN_Y_DOWN_OPEN,
	SCREEN_Y_DOWN_DOOR,
	SCREEN_Y_DOWN_HIDE,
	SCREEN_Y_DOWN_WALL,
	// ������� ������ ���������
	SCREEN_CHAR,
	// ������� � ������ ����
	SCREEN_NEW_ZONE,
	// ������ �������
	SCREEN_PEACE,
	// �� (���� + 0 ����)
	SCREEN_DEATH_TRAP,
	// ����� ������� �� �������
	SCREEN_EMPTY,
	// � ������� �����, ������� �� �����
	SCREEN_MOB_UNDEF,
	// ���-�� ������� � �������
	SCREEN_MOB_1,
	SCREEN_MOB_2,
	SCREEN_MOB_3,
	SCREEN_MOB_4,
	SCREEN_MOB_5,
	SCREEN_MOB_6,
	SCREEN_MOB_7,
	SCREEN_MOB_8,
	SCREEN_MOB_9,
	// � ������� ������ 9 �������
	SCREEN_MOB_OVERFLOW,
	// ���� � ����������
	SCREEN_OBJ_UNDEF,
	SCREEN_OBJ_1,
	SCREEN_OBJ_2,
	SCREEN_OBJ_3,
	SCREEN_OBJ_4,
	SCREEN_OBJ_5,
	SCREEN_OBJ_6,
	SCREEN_OBJ_7,
	SCREEN_OBJ_8,
	SCREEN_OBJ_9,
	SCREEN_OBJ_OVERFLOW,
	// ���� �� ����������
	SCREEN_MOB_SPEC_SHOP,
	SCREEN_MOB_SPEC_RENT,
	SCREEN_MOB_SPEC_MAIL,
	SCREEN_MOB_SPEC_BANK,
	SCREEN_MOB_SPEC_HORSE,
	SCREEN_MOB_SPEC_TEACH,
	SCREEN_MOB_SPEC_EXCH,
	// ������ � ������� ����� �������
	SCREEN_WATER,
	// ����� ������� ��� ������
	SCREEN_FLYING,
	// ������ � �����
	SCREEN_TOTAL
};

const char *signs[] =
{
	// SCREEN_Y
	"&K - &n",
	"&C-=-&n",
	"&R---&n",
	"&G---&n",
	// SCREEN_X
	"&K:&n",
	"&C/&n",
	"&R|&n",
	"&G|&n",
	// SCREEN_UP
	"&K^&n",
	"&C^&n",
	"&R^&n",
	"",
	// SCREEN_DOWN
	"&Kv&n",
	"&Cv&n",
	"&Rv&n",
	"",
	// SCREEN_Y_UP
	"&K -&n",
	"&C-=&n",
	"&R--&n",
	"&G--&n",
	// SCREEN_Y_DOWN
	"&K- &n",
	"&C=-&n",
	"&R--&n",
	"&G--&n",
	// OTHERS
	"&c@&n",
	"&C>&n",
	"&K~&n",
	"&R�&n",
	"",
	"&K?&n",
	"&r1&n",
	"&r2&n",
	"&r3&n",
	"&r4&n",
	"&r5&n",
	"&r6&n",
	"&r7&n",
	"&r8&n",
	"&r9&n",
	"&R!&n",
	"&K?&n",
	"&y1&n",
	"&y2&n",
	"&y3&n",
	"&y4&n",
	"&y5&n",
	"&y6&n",
	"&y7&n",
	"&y8&n",
	"&y9&n",
	"&Y!&n",
	"&W$&n",
	"&WR&n",
	"&WM&n",
	"&WB&n",
	"&WH&n",
	"&WT&n",
	"&WE&n",
	"&C,&n",
	"&C`&n",
};

std::map<int /* room vnum */, int /* min depth */> check_dupe;

// ��������� ������� �� ���� �� �����������
void put_on_screen(int y, int x, int num, int depth)
{
	if (y >= MAX_LINES || x >= MAX_LENGHT)
	{
		log("SYSERROR: %d;%d (%s %s %d)", y, x, __FILE__, __func__, __LINE__);
		return;
	}
	if (depths[y][x] == -1)
	{
		// ���� ���� ������
		screen[y][x] = num;
		depths[y][x] = depth;
	}
	else if (depths[y][x] > depth)
	{
		// ��� ���-�� ���� ����������
		if (screen[y][x] == num)
		{
			// ���� ��� �� ����� ������,
			// �� ���� �������� ������� �� ������ ����������� ���������
			depths[y][x] = depth;
		}
		else
		{
			// ������ ������ � ������� �������
			// �������� ��� ������� ���� � ����� �������
			// � ������ ������ ������� ������
			const int hide_num = depths[y][x];
			for (int i = 0; i < MAX_LINES; ++i)
			{
				for (int k = 0; k < MAX_LENGHT; ++k)
				{
					if (depths[i][k] >= hide_num)
					{
						screen[i][k] = -1;
						depths[i][k] = -1;
					}
				}
			}
			screen[y][x] = num;
			depths[y][x] = depth;
		}
	}
	else if ((screen[y][x] >= SCREEN_UP_OPEN && screen[y][x] <= SCREEN_UP_WALL)
			|| (screen[y][x] >= SCREEN_DOWN_OPEN && screen[y][x] <= SCREEN_DOWN_WALL))
	{
		// ������ ^ � v ����������, ���� ���� ���
		screen[y][x] = num;
		depths[y][x] = depth;
	}
}

// ������ ����� � ������ ������, ������� ���� �������� ��� ������� �����/����
// �������� ������� ������� ^ � v, �� �� �������� ���� �����, �.�. ���
// ������ ������������, �� � ��������, ������� ���� �� ��������
void check_position_and_put_on_screen(int next_y, int next_x, int sign_num, int depth, int exit_num)
{
	if (exit_num == UP)
	{
		switch(sign_num)
		{
		case SCREEN_DEATH_TRAP:
		case SCREEN_WATER:
		case SCREEN_FLYING:
			put_on_screen(next_y - 1, next_x + 1, sign_num, depth);
			return;
		}
	}
	else if (exit_num == DOWN)
	{
		switch(sign_num)
		{
		case SCREEN_DEATH_TRAP:
		case SCREEN_WATER:
		case SCREEN_FLYING:
			put_on_screen(next_y + 1, next_x - 1, sign_num, depth);
			return;
		}
	}
	else
	{
		put_on_screen(next_y, next_x, sign_num, depth);
	}
}

void draw_mobs(const CHAR_DATA *ch, int room_rnum, int next_y, int next_x)
{
	if (IS_DARK(room_rnum) && !IS_IMMORTAL(ch))
	{
		put_on_screen(next_y, next_x - 1, SCREEN_MOB_UNDEF, 1);
	}
	else
	{
		int cnt = 0;
		for (CHAR_DATA *tch = world[room_rnum]->people; tch; tch = tch->next_in_room)
		{
			if (tch == ch)
			{
				continue;
			}
			if (IS_NPC(tch) && !ch->map_check_option(MAP_MODE_MOBS))
			{
				continue;
			}
			if (!IS_NPC(tch) && !ch->map_check_option(MAP_MODE_PLAYERS))
			{
				continue;
			}
			if (HERE(tch)
				&& (CAN_SEE(ch, tch)
					|| awaking(tch, AW_HIDE | AW_INVIS | AW_CAMOUFLAGE)))
			{
				++cnt;
			}
		}
		if (cnt > 0 && cnt <= 9)
		{
			put_on_screen(next_y, next_x - 1, SCREEN_MOB_UNDEF + cnt, 1);
		}
		else if (cnt > 9)
		{
			put_on_screen(next_y, next_x - 1, SCREEN_MOB_OVERFLOW, 1);
		}
	}
}

void draw_objs(const CHAR_DATA *ch, int room_rnum, int next_y, int next_x)
{
	if (IS_DARK(room_rnum) && !IS_IMMORTAL(ch))
	{
		put_on_screen(next_y, next_x + 1, SCREEN_OBJ_UNDEF, 1);
	}
	else
	{
		int cnt = 0;

		for (OBJ_DATA *obj = world[room_rnum]->contents; obj; obj = obj->next_content)
		{
			if (IS_CORPSE(obj) && GET_OBJ_VAL(obj, 2) >= 0
				&& !ch->map_check_option(MAP_MODE_MOBS_CORPSES))
			{
				continue;
			}
			if (IS_CORPSE(obj) && GET_OBJ_VAL(obj, 2) < 0
				&& !ch->map_check_option(MAP_MODE_PLAYER_CORPSES))
			{
				continue;
			}
			if ((GET_OBJ_TYPE(obj) == ITEM_INGRADIENT || GET_OBJ_TYPE(obj) == ITEM_MING)
				&& !ch->map_check_option(MAP_MODE_INGREDIENTS))
			{
				continue;
			}
			if (!IS_CORPSE(obj)
				&& GET_OBJ_TYPE(obj) != ITEM_INGRADIENT
				&& GET_OBJ_TYPE(obj) != ITEM_MING
				&& !ch->map_check_option(MAP_MODE_OTHER_OBJECTS))
			{
				continue;
			}
			if (CAN_SEE_OBJ(ch, obj))
			{
				++cnt;
			}
		}
		if (cnt > 0 && cnt <= 9)
		{
			put_on_screen(next_y, next_x + 1, SCREEN_OBJ_UNDEF + cnt, 1);
		}
		else if (cnt > 9)
		{
			put_on_screen(next_y, next_x + 1, SCREEN_OBJ_OVERFLOW, 1);
		}
	}
}

void drow_spec_mobs(const CHAR_DATA *ch, int room_rnum, int next_y, int next_x, int cur_depth)
{
	bool all = ch->map_check_option(MAP_MODE_MOB_SPEC_ALL) ? true : false;

	for (CHAR_DATA *tch = world[room_rnum]->people; tch; tch = tch->next_in_room)
	{
		SPECIAL(*func) = GET_MOB_SPEC(tch);
		if (func)
		{
			if ((func == shop_keeper || func == ShopExt::shop_ext)
				&& (all || ch->map_check_option(MAP_MODE_MOB_SPEC_SHOP)))
			{
				put_on_screen(next_y, next_x, SCREEN_MOB_SPEC_SHOP, cur_depth);
			}
			if (func == receptionist
				&& (all || ch->map_check_option(MAP_MODE_MOB_SPEC_RENT)))
			{
				put_on_screen(next_y, next_x, SCREEN_MOB_SPEC_RENT, cur_depth);
			}
			if (func == postmaster
				&& (all || ch->map_check_option(MAP_MODE_MOB_SPEC_MAIL)))
			{
				put_on_screen(next_y, next_x, SCREEN_MOB_SPEC_MAIL, cur_depth);
			}
			if (func == bank
				&& (all || ch->map_check_option(MAP_MODE_MOB_SPEC_BANK)))
			{
				put_on_screen(next_y, next_x, SCREEN_MOB_SPEC_BANK, cur_depth);
			}
			if (func == exchange
				&& (all || ch->map_check_option(MAP_MODE_MOB_SPEC_EXCH)))
			{
				put_on_screen(next_y, next_x, SCREEN_MOB_SPEC_EXCH, cur_depth);
			}
			if (func == horse_keeper
				&& (all || ch->map_check_option(MAP_MODE_MOB_SPEC_HORSE)))
			{
				put_on_screen(next_y, next_x, SCREEN_MOB_SPEC_HORSE, cur_depth);
			}
			if ((func == guild_mono || func == guild_poly)
				&& (all || ch->map_check_option(MAP_MODE_MOB_SPEC_TEACH)))
			{
				put_on_screen(next_y, next_x, SCREEN_MOB_SPEC_TEACH, cur_depth);
			}
		}
	}
}

bool mode_allow(const CHAR_DATA *ch, int cur_depth)
{
	if (ch->map_check_option(MAP_MODE_1_DEPTH)
		&& !ch->map_check_option(MAP_MODE_2_DEPTH)
		&& cur_depth > 1)
	{
		return false;
	}

	if (ch->map_check_option(MAP_MODE_2_DEPTH) && cur_depth > 2)
	{
		return false;
	}

	return true;
}

void draw_room(const CHAR_DATA *ch, const ROOM_DATA *room, int cur_depth, int y, int x)
{
	// ����� �� ������ �� �������� ��������, �� � ��������� �� �������
	std::map<int, int>::iterator i = check_dupe.find(room->number);
	if (i != check_dupe.end())
	{
		if (i->second <= cur_depth)
		{
			return;
		}
		else
		{
			i->second = cur_depth;
		}
	}
	else
	{
		check_dupe.insert(std::make_pair(room->number, cur_depth));
	}

	if (world[ch->in_room] == room)
	{
		put_on_screen(y, x, SCREEN_CHAR, cur_depth);
		if (ch->map_check_option(MAP_MODE_MOBS_CURR_ROOM))
		{
			draw_mobs(ch, ch->in_room, y, x);
		}
		if (ch->map_check_option(MAP_MODE_OBJS_CURR_ROOM))
		{
			draw_objs(ch, ch->in_room, y, x);
		}
	}
	else if (IS_SET(GET_FLAG(room->room_flags, ROOM_PEACEFUL), ROOM_PEACEFUL))
	{
		put_on_screen(y, x, SCREEN_PEACE, cur_depth);
	}

	for (int i = 0; i < NUM_OF_DIRS; ++i)
	{
		int cur_y = y, cur_x = x, cur_sign = -1, next_y = y, next_x = x;
		switch(i)
		{
		case NORTH:
			cur_y -= 1;
			next_y -= 2;
			cur_sign = SCREEN_Y_OPEN;
			break;
		case EAST:
			cur_x += 2;
			next_x += 4;
			cur_sign = SCREEN_X_OPEN;
			break;
		case SOUTH:
			cur_y += 1;
			next_y += 2;
			cur_sign = SCREEN_Y_OPEN;
			break;
		case WEST:
			cur_x -= 2;
			next_x -= 4;
			cur_sign = SCREEN_X_OPEN;
			break;
		case UP:
			cur_y -= 1;
			cur_x += 1;
			cur_sign = SCREEN_UP_OPEN;
			break;
		case DOWN:
			cur_y += 1;
			cur_x -= 1;
			cur_sign = SCREEN_DOWN_OPEN;
			break;
		default:
			log("SYSERROR: i=%d (%s %s %d)", i, __FILE__, __func__, __LINE__);
			return;
		}

		if (room->dir_option[i]
			&& room->dir_option[i]->to_room != NOWHERE
			&& (!EXIT_FLAGGED(room->dir_option[i], EX_HIDDEN) || IS_IMMORTAL(ch)))
		{
			// ����� ����� �����������, ��� ������ ������������ - �� � �����
			const ROOM_DATA *next_room = world[room->dir_option[i]->to_room];
			// �� ����� � ����� � 0 ������
			if (IS_SET(GET_FLAG(next_room->room_flags, ROOM_DEATH), ROOM_DEATH)
				&& (GET_REMORT(ch) <= 0 || IS_IMMORTAL(ch)))
			{
				check_position_and_put_on_screen(next_y, next_x, SCREEN_DEATH_TRAP, cur_depth, i);
			}
			// ����� �������
			if (next_room->sector_type == SECT_WATER_NOSWIM
				|| next_room->sector_type == SECT_UNDERWATER)
			{
				check_position_and_put_on_screen(next_y, next_x, SCREEN_WATER, cur_depth, i);
			}
			// ����-��
			if (next_room->sector_type == SECT_FLYING)
			{
				check_position_and_put_on_screen(next_y, next_x, SCREEN_FLYING, cur_depth, i);
			}
			// ����� � ������ ������, �� ���������� ��� ������� �����/����
			if (i != UP && i != DOWN)
			{
				// ������� � ������ ����
				if (next_room->zone != world[ch->in_room]->zone)
				{
					put_on_screen(next_y, next_x, SCREEN_NEW_ZONE, cur_depth);
				}
				// ��� �� ���������
				drow_spec_mobs(ch, room->dir_option[i]->to_room, next_y, next_x, cur_depth);
			}
			// ��������� ������
			if (EXIT_FLAGGED(room->dir_option[i], EX_CLOSED))
			{
				put_on_screen(cur_y, cur_x, cur_sign + 1, cur_depth);
			}
			else if (EXIT_FLAGGED(room->dir_option[i], EX_HIDDEN))
			{
				put_on_screen(cur_y, cur_x, cur_sign + 2, cur_depth);
			}
			else
			{
				put_on_screen(cur_y, cur_x, cur_sign, cur_depth);
			}
			// ��������
			if (cur_depth == 1
				&& (!EXIT_FLAGGED(room->dir_option[i], EX_CLOSED) || IS_IMMORTAL(ch))
				&& (ch->map_check_option(MAP_MODE_MOBS) || ch->map_check_option(MAP_MODE_PLAYERS)))
			{
				// � ������ �����/���� next_y/x = y/x, �������� ������������
				// ��������� ����, �� ���������, ����� ������ ����� ����� v � ^
				// ������ draw_mobs ���������� ��� ���� �������� �� x-1
				if (cur_sign == SCREEN_UP_OPEN)
				{
					draw_mobs(ch, room->dir_option[i]->to_room, next_y - 1, next_x + 3);
				}
				else if (cur_sign == SCREEN_DOWN_OPEN)
				{
					draw_mobs(ch, room->dir_option[i]->to_room, next_y + 1, next_x - 1);
				}
				else
				{
					// ��������� ������ ������ ������� ��� ������, �� ���������
					// ������������ ������ ��������� �� ������� ������
					draw_mobs(ch, room->dir_option[i]->to_room, next_y, next_x);
				}
			}
			// ��������
			if (cur_depth == 1
				&& (!EXIT_FLAGGED(room->dir_option[i], EX_CLOSED) || IS_IMMORTAL(ch))
				&& (ch->map_check_option(MAP_MODE_MOBS_CORPSES)
					|| ch->map_check_option(MAP_MODE_PLAYER_CORPSES)
					|| ch->map_check_option(MAP_MODE_INGREDIENTS)
					|| ch->map_check_option(MAP_MODE_OTHER_OBJECTS)))
			{
				if (cur_sign == SCREEN_UP_OPEN)
				{
					draw_objs(ch, room->dir_option[i]->to_room, next_y - 1, next_x);
				}
				else if (cur_sign == SCREEN_DOWN_OPEN)
				{
					draw_objs(ch, room->dir_option[i]->to_room, next_y + 1, next_x - 2);
				}
				else
				{
					draw_objs(ch, room->dir_option[i]->to_room, next_y, next_x);
				}
			}

			// ������ �� ��������� � ������� �������
			if (i != UP && i != DOWN
				&& cur_depth < MAX_DEPTH_ROOMS
				&& (!EXIT_FLAGGED(room->dir_option[i], EX_CLOSED) || IS_IMMORTAL(ch))
				&& next_room->zone == world[ch->in_room]->zone
				&& mode_allow(ch, cur_depth))
			{
				draw_room(ch, next_room, cur_depth + 1, next_y, next_x);
			}
		}
		else
		{
			put_on_screen(cur_y, cur_x, cur_sign + 3, cur_depth);
		}
	}
}

// imm �� ������� = 0, ���� ���, �� ������������� ����� ���������� ���
void print_map(CHAR_DATA *ch, CHAR_DATA *imm)
{
	for (int i = 0; i < MAX_LINES; ++i)
	{
		for (int k = 0; k < MAX_LENGHT; ++k)
		{
			screen[i][k] = -1;
			depths[i][k] = -1;
		}
	}
	check_dupe.clear();

	draw_room(ch, world[ch->in_room], 1, MAX_LINES/2, MAX_LENGHT/2);

	int start_line = -1, end_line = MAX_LINES, char_line = -1;
	// ��� ���������� ���� - ������ ������ �� ������
	// ��� ������������ Y �������� � ����������� �� �������
	// ��� ���������� ������� �����/����
	// ������ ������� ������ ������ ������, ����� �� ������
	// ����� ��������� �� ������ ���������� ������
	for (int i = 0; i < MAX_LINES; ++i)
	{
		bool found = false;

		for (int k = 0; k < MAX_LENGHT; ++k)
		{
			if (screen[i][k] > -1 && screen[i][k] < SCREEN_TOTAL)
			{
				found = true;

				if (screen[i][k] == SCREEN_CHAR)
				{
					char_line = i;
				}

				if (screen[i][k] >= SCREEN_Y_OPEN
					&& screen[i][k] <= SCREEN_Y_WALL
					&& k + 1 < MAX_LENGHT && k >= 1)
				{
					if (screen[i][k + 1] > -1
						&& screen[i][k + 1] != SCREEN_UP_WALL)
					{
						screen[i][k - 1] = screen[i][k] + SCREEN_Y_UP_OPEN;
						screen[i][k] = SCREEN_EMPTY;
					}
					else if (screen[i][k - 1] > -1
						&& screen[i][k - 1] != SCREEN_DOWN_WALL)
					{
						screen[i][k] += SCREEN_Y_DOWN_OPEN;
						screen[i][k + 1] = SCREEN_EMPTY;
					}
					else
					{
						screen[i][k - 1] = screen[i][k];
						screen[i][k] = SCREEN_EMPTY;
						screen[i][k + 1] = SCREEN_EMPTY;
					}
				}
				else if (screen[i][k] >= SCREEN_Y_OPEN
					&& screen[i][k] <= SCREEN_Y_WALL)
				{
					screen[i][k] = SCREEN_EMPTY;
					send_to_char("������ ��� ��������� ����� (1), �������� �����!\r\n", ch);
				}
			}
		}

		if (found && start_line < 0)
		{
			start_line = i;
		}
		else if (!found && start_line > 0)
		{
			end_line = i;
			break;
		}
	}

	if (start_line == -1 || char_line == -1)
	{
		log("assert print_map start_line=%d, char_line=%d", start_line, char_line);
		return;
	}

	std::string out;
	out += "\r\n";

	bool fixed_1 = false;
	bool fixed_2 = false;

	if (ch->map_check_option(MAP_MODE_DEPTH_FIXED))
	{
		if (ch->map_check_option(MAP_MODE_1_DEPTH))
		{
			fixed_1 = true;
		}
		if (ch->map_check_option(MAP_MODE_2_DEPTH))
		{
			fixed_1 = false;
			fixed_2 = true;
		}
	}

	if (fixed_1 || fixed_2)
	{
		const int need_top = fixed_2 ? 5 : 3;
		int top_lines = char_line - start_line;
		if (top_lines < need_top)
		{
			for (int i = 1; i <= need_top - top_lines; ++i)
			{
				out += ": \r\n";
			}
		}
	}

	for (int i = start_line; i < end_line; ++i)
	{
		out += ": ";
		for (int k = 0; k < MAX_LENGHT; ++k)
		{
			if (screen[i][k] <= -1)
			{
				out += " ";
			}
			else if (screen[i][k] < SCREEN_TOTAL
				&& screen[i][k] != SCREEN_EMPTY)
			{
				out += signs[screen[i][k]];
			}
		}
		out += "\r\n";
	}

	if (fixed_1 || fixed_2)
	{
		const int need_bot = fixed_2 ? 6 : 4;
		int bot_lines = end_line - char_line;
		if (bot_lines < need_bot)
		{
			for (int i = 1; i <= need_bot - bot_lines; ++i)
			{
				out += ": \r\n";
			}
		}
	}

	out += "\r\n";

	if (imm)
	{
		send_to_char(out, imm);
	}
	else
	{
		send_to_char(out, ch);
	}
}

void Options::olc_menu(CHAR_DATA *ch)
{
	std::stringstream out;
	out << "�������� ��� ����������� �� �����:\r\n";

	int cnt = 0;
	for (int i = 0; i < TOTAL_MAP_OPTIONS; ++i)
	{
		switch (i)
		{
		case MAP_MODE_MOBS:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_MOBS])
				out << "[x] �������� ����� ������� (����)\r\n";
			else
				out << "[ ] �������� ����� ������� (����)\r\n";
			break;
		case MAP_MODE_PLAYERS:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_PLAYERS])
				out << "[x] ������ ������\r\n\r\n";
			else
				out << "[ ] ������ ������\r\n\r\n";
			break;
		case MAP_MODE_MOBS_CORPSES:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_MOBS_CORPSES])
				out << "[x] ����� ������� (����� �������)\r\n";
			else
				out << "[ ] ����� ������� (����� �������)\r\n";
			break;
		case MAP_MODE_PLAYER_CORPSES:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_PLAYER_CORPSES])
				out << "[x] ����� �������\r\n";
			else
				out << "[ ] ����� �������\r\n";
			break;
		case MAP_MODE_INGREDIENTS:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_INGREDIENTS])
				out << "[x] �����������\r\n";
			else
				out << "[ ] �����������\r\n";
			break;
		case MAP_MODE_OTHER_OBJECTS:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_OTHER_OBJECTS])
				out << "[x] ������ ��������\r\n\r\n";
			else
				out << "[ ] ������ ��������\r\n\r\n";
			break;
		case MAP_MODE_1_DEPTH:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_1_DEPTH])
				out << "[x] ������ ����������� �������\r\n";
			else
				out << "[ ] ������ ����������� �������\r\n";
			break;
		case MAP_MODE_2_DEPTH:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_2_DEPTH])
				out << "[x] ����������� ������� + 1\r\n";
			else
				out << "[ ] ����������� ������� + 1\r\n";
			break;
		case MAP_MODE_DEPTH_FIXED:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_DEPTH_FIXED])
				out << "[x] ����������� ������ ����� ����������� ������\r\n\r\n";
			else
				out << "[ ] ����������� ������ ����� ����������� ������\r\n\r\n";
			break;
		case MAP_MODE_MOB_SPEC_SHOP:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_MOB_SPEC_SHOP])
				out << "[x] �������� (��������, $)\r\n";
			else
				out << "[ ] �������� (��������, $)\r\n";
			break;
		case MAP_MODE_MOB_SPEC_RENT:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_MOB_SPEC_RENT])
				out << "[x] ������� (������, R)\r\n";
			else
				out << "[ ] ������� (������, R)\r\n";
			break;
		case MAP_MODE_MOB_SPEC_MAIL:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_MOB_SPEC_MAIL])
				out << "[x] ������ (�����, M)\r\n";
			else
				out << "[ ] ������ (�����, M)\r\n";
			break;
		case MAP_MODE_MOB_SPEC_BANK:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_MOB_SPEC_BANK])
				out << "[x] ������� (�����, B)\r\n";
			else
				out << "[ ] ������� (�����, B)\r\n";
			break;
		case MAP_MODE_MOB_SPEC_EXCH:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_MOB_SPEC_EXCH])
				out << "[x] �������� (�����, E)\r\n";
			else
				out << "[ ] �������� (�����, E)\r\n";
			break;
		case MAP_MODE_MOB_SPEC_HORSE:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_MOB_SPEC_HORSE])
				out << "[x] ������ (�������, H)\r\n";
			else
				out << "[ ] ������ (�������, H)\r\n";
			break;
		case MAP_MODE_MOB_SPEC_TEACH:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_MOB_SPEC_TEACH])
				out << "[x] ������� (�����, T)\r\n";
			else
				out << "[ ] ������� (�����, T)\r\n";
			break;
		case MAP_MODE_MOB_SPEC_ALL:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_MOB_SPEC_ALL])
				out << "[x] ��� ���� �� ����. ���������\r\n\r\n";
			else
				out << "[ ] ��� ���� �� ����. ���������\r\n\r\n";
			break;
		case MAP_MODE_MOBS_CURR_ROOM:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_MOBS_CURR_ROOM])
				out << "[x] �������� (�. 1-2) � ������� � ����������\r\n";
			else
				out << "[ ] �������� (�. 1-2) � ������� � ����������\r\n";
			break;
		case MAP_MODE_OBJS_CURR_ROOM:
			out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM) << ") ";
			if (bit_list_[MAP_MODE_OBJS_CURR_ROOM])
				out << "[x] ������� (�. 3-6) � ������� � ����������\r\n\r\n";
			else
				out << "[ ] ������� (�. 3-6) � ������� � ����������\r\n\r\n";
			break;
		}
	}

	out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM)
		<< ") �������� ���\r\n";
	out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM)
		<< ") ��������� ���\r\n\r\n";

	out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM)
		<< ") ����� ��� ����������\r\n";
	out << CCGRN(ch, C_NRM) << std::setw(2) << ++cnt << CCNRM(ch, C_NRM)
		<< ") ��������� � �����\r\n"
		<< "��� �����:";

	send_to_char(out.str(), ch);
}

void Options::parse_menu(CHAR_DATA *ch, const char *arg)
{
	if (!*arg)
	{
		send_to_char("�������� �����!\r\n", ch);
		olc_menu(ch);
		return;
	}

	int num = atoi(arg);
	--num;

	if (num >= 0 && num < TOTAL_MAP_OPTIONS)
	{
		bit_list_.flip(num);
		olc_menu(ch);
	}
	else if (num == TOTAL_MAP_OPTIONS)
	{
		bit_list_.reset();
		bit_list_.flip();
		bit_list_.reset(MAP_MODE_1_DEPTH);
		bit_list_.reset(MAP_MODE_2_DEPTH);
		bit_list_.reset(MAP_MODE_DEPTH_FIXED);
		olc_menu(ch);
	}
	else if (num == TOTAL_MAP_OPTIONS + 1)
	{
		bit_list_.reset();
		olc_menu(ch);
	}
	else if (num == TOTAL_MAP_OPTIONS + 2)
	{
		ch->desc->map_options.reset();
		STATE(ch->desc) = CON_PLAYING;
		send_to_char("�������������� ��������.\r\n", ch);
		return;
	}
	else if (num == TOTAL_MAP_OPTIONS + 3)
	{
		ch->map_olc_save();
		ch->desc->map_options.reset();
		STATE(ch->desc) = CON_PLAYING;
		send_to_char("��������� ���������.\r\n", ch);
		return;
	}
	else
	{
		send_to_char("�������� �����!\r\n", ch);
		olc_menu(ch);
		return;
	}
}

const char *message =
"��������� �����:\r\n"
"   &W�����&n - ����� ����� � �������� �����������\r\n"
"   &W����� �������������|�����|����&n - ���������� ������������ �� ����� ����������� (������������� ����)\r\n"
"   &W����� ��������|���������&n\r\n"
"      ���, ����, ������, ����� �����, ����� �������, �����������, ������ ��������, ������� 1, ������� 2\r\n"
"      ����������� ������, �������, �����, �����, ����, �����, ������, �������, ��� �����������, ���� � �������,\r\n"
"      �������� � �������\r\n"
"      - ��������� ������� ����� ��� ������ �� ������� ��� ��� �������� ������\r\n"
"   &W����� �������� (��� ���� ����� �� ����������� ������) &n\r\n"
"      - ���������� ���������� ����� � ���������� �������, �� ������� ��������\r\n"
"   &W����� �������|������&n - ����� ������ �������\r\n";

bool parse_text_olc(CHAR_DATA *ch, const std::string &str, std::bitset<TOTAL_MAP_OPTIONS> &bits, bool flag)
{
	std::vector<std::string> str_list;
	boost::split(str_list, str, boost::is_any_of(","), boost::token_compress_on);
	bool error = false;

	for (std::vector<std::string>::const_iterator k = str_list.begin(),
		kend = str_list.end(); k != kend; ++k)
	{
		if (isname(*k, "���"))
		{
			bits.reset();
			if (flag)
			{
				bits.flip();
				bits.reset(MAP_MODE_1_DEPTH);
				bits.reset(MAP_MODE_2_DEPTH);
				bits.reset(MAP_MODE_DEPTH_FIXED);
			}
			return error;
		}
		if (isname(*k, "����"))
		{
			bits[MAP_MODE_MOBS] = flag;
		}
		else if (isname(*k, "������"))
		{
			bits[MAP_MODE_PLAYERS] = flag;
		}
		else if (isname(*k, "����� �����"))
		{
			bits[MAP_MODE_MOBS_CORPSES] = flag;
		}
		else if (isname(*k, "����� �������"))
		{
			bits[MAP_MODE_PLAYER_CORPSES] = flag;
		}
		else if (isname(*k, "�����������"))
		{
			bits[MAP_MODE_INGREDIENTS] = flag;
		}
		else if (isname(*k, "������ ��������"))
		{
			bits[MAP_MODE_OTHER_OBJECTS] = flag;
		}
		else if (isname(*k, "������� 1"))
		{
			bits[MAP_MODE_1_DEPTH] = flag;
		}
		else if (isname(*k, "������� 2"))
		{
			bits[MAP_MODE_2_DEPTH] = flag;
		}
		else if (isname(*k, "����������� ������"))
		{
			bits[MAP_MODE_DEPTH_FIXED] = flag;
		}
		else if (isname(*k, "�������"))
		{
			bits[MAP_MODE_MOB_SPEC_SHOP] = flag;
		}
		else if (isname(*k, "�����"))
		{
			bits[MAP_MODE_MOB_SPEC_RENT] = flag;
		}
		else if (isname(*k, "�����"))
		{
			bits[MAP_MODE_MOB_SPEC_MAIL] = flag;
		}
		else if (isname(*k, "����"))
		{
			bits[MAP_MODE_MOB_SPEC_BANK] = flag;
		}
		else if (isname(*k, "�����"))
		{
			bits[MAP_MODE_MOB_SPEC_EXCH] = flag;
		}
		else if (isname(*k, "������"))
		{
			bits[MAP_MODE_MOB_SPEC_HORSE] = flag;
		}
		else if (isname(*k, "�������"))
		{
			bits[MAP_MODE_MOB_SPEC_TEACH] = flag;
		}
		else if (isname(*k, "��� �����������"))
		{
			bits[MAP_MODE_MOB_SPEC_ALL] = flag;
		}
		else if (isname(*k, "���� � �������"))
		{
			bits[MAP_MODE_MOBS_CURR_ROOM] = flag;
		}
		else if (isname(*k, "�������� � �������"))
		{
			bits[MAP_MODE_OBJS_CURR_ROOM] = flag;
		}
		else
		{
			error = true;
			send_to_char(ch, "�������� ��������: %s\r\n", k->c_str());
		}
	}

	return error;
}

void Options::text_olc(CHAR_DATA *ch, const char *arg)
{
	std::string str(arg), first_arg;
	GetOneParam(str, first_arg);
	boost::trim(str);

	if (isname(first_arg, "�������������") || isname(first_arg, "�����")|| isname(first_arg, "����")
		|| isname(first_arg, "edit") || isname(first_arg, "options") || isname(first_arg, "menu"))
	{
		ch->map_olc();
	}
	else if (isname(first_arg, "�������") || isname(first_arg, "������"))
	{
		send_to_char(message, ch);
	}
	else if (isname(first_arg, "��������") || isname(first_arg, "activate")
		|| isname(first_arg, "���������") || isname(first_arg, "deactivate"))
	{
		if (str.empty())
		{
			send_to_char(message, ch);
			send_to_char("\r\n������� ������ �����.\r\n", ch);
			return;
		}

		const bool flag = (isname(first_arg, "��������") || isname(first_arg, "activate")) ? true : false;
		const bool error = parse_text_olc(ch, str, bit_list_, flag);
		if (!error)
		{
			send_to_char("�������.\r\n", ch);
		}
	}
	else if (isname(first_arg, "��������") || isname(first_arg, "show"))
	{
		if (str.empty())
		{
			send_to_char(message, ch);
			send_to_char("\r\n������� ������ �����.\r\n", ch);
			return;
		}

		std::bitset<TOTAL_MAP_OPTIONS> tmp_bits;
		parse_text_olc(ch, str, tmp_bits, true);

		std::bitset<TOTAL_MAP_OPTIONS> saved_ch_bits = bit_list_;
		bit_list_ = tmp_bits;
		print_map(ch);
		bit_list_ = saved_ch_bits;
	}
	else
	{
		send_to_char(message, ch);
	}
}

} // namespace MapSystem

ACMD(do_map)
{
	if (IS_NPC(ch))
	{
		return;
	}

	skip_spaces(&argument);

	if (!argument || !*argument)
	{
		MapSystem::print_map(ch);
	}
	else
	{
		ch->map_text_olc(argument);
	}
}
