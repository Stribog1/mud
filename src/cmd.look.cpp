//
// Рефакторинг act.informative
// Все функции, отвечающие на обработку команд смотреть/подглядеть
//

#include "cmd.look.h"

#include <pk.h>
#include <structs.h>
#include <skills.h>
#include <screen.h>
#include <act.informative.h>
#include <char_obj_utils.inl>
#include <description.h>
#include <map.hpp>
#include <handler.h>
#include <zone.table.hpp>
#include <house.h>
#include <depot.hpp>
#include <liquid.hpp>
#include <modify.h>

bool put_delim(std::stringstream &out, bool delim)
{
    if (!delim)	{
        out << " (";
    }
    else {
        out << ", ";
    }
    return true;
}

int paste_description(char *string, const char *tag, int need)
{
    if (!*string || !*tag)
    {
        return (FALSE);
    }

    const char *pos = str_str(string, tag);
    if (!pos)
    {
        return FALSE;
    }

    if (!need)
    {
        const size_t offset = pos - string;
        string[offset] = '\0';
        pos = str_str(pos + 1, tag);
        if (pos)
        {
            auto to_pos = string + offset;
            auto from_pos = pos + strlen(tag);
            while ((*(to_pos++) = *(from_pos++)));
        }
        return FALSE;
    }

    for (; *pos && *pos != '>'; pos++);

    if (*pos)
    {
        pos++;
    }

    if (*pos == 'R')
    {
        pos++;
        buf[0] = '\0';
    }

    strcat(buf, pos);
    pos = str_str(buf, tag);
    if (pos)
    {
        const size_t offset = pos - buf;
        buf[offset] = '\0';
    }

    return (TRUE);
}

void show_glow_objs(CHAR_DATA *ch)
{
    unsigned cnt = 0;
    for (OBJ_DATA *obj = world[ch->in_room]->contents;
         obj; obj = obj->get_next_content())
    {
        if (obj->get_extra_flag(EExtraFlag::ITEM_GLOW))
        {
            ++cnt;
            if (cnt > 1)
            {
                break;
            }
        }
    }
    if (!cnt) return;

    const char *str = cnt > 1 ?
                      "Вы видите очертания каких-то блестящих предметов.\r\n" :
                      "Вы видите очертания какого-то блестящего предмета.\r\n";
    send_to_char(str, ch);
}

void skip_hide_on_look(CHAR_DATA * ch)
{

    if (AFF_FLAGGED(ch, EAffectFlag::AFF_HIDE) &&
        ((!ch->get_skill(SKILL_LOOK_HIDE) ||
          ((number(1, 100) -
            calculate_skill(ch, SKILL_LOOK_HIDE, nullptr) - 2 * (ch->get_wis() - 9)) > 0))))
    {
        affect_from_char(ch, SPELL_HIDE);
        if (!AFF_FLAGGED(ch, EAffectFlag::AFF_HIDE))
        {
            send_to_char("Вы прекратили прятаться.\r\n", ch);
            act("$n прекратил$g прятаться.", FALSE, ch, nullptr, nullptr, TO_ROOM);
        }
    }
}

void show_room_affects(CHAR_DATA* ch, const char* name_affects[], const char* name_self_affects[])
{
    bitvector_t bitvector = 0;
    std::ostringstream buffer;

    for (const auto& af : world[ch->in_room]->affected)
    {
        switch (af->bitvector)
        {
            case AFF_ROOM_LIGHT:					// 1 << 0
                if (!IS_SET(bitvector, AFF_ROOM_LIGHT))
                {
                    if (af->caster_id == ch->id && *name_self_affects[0] != '\0')
                    {
                        buffer << name_self_affects[0] << "\r\n";
                    }
                    else if(*name_affects[0] != '\0')
                    {
                        buffer << name_affects[0] << "\r\n";
                    }

                    SET_BIT(bitvector, AFF_ROOM_LIGHT);
                }
                break;
            case AFF_ROOM_FOG:						// 1 << 1
                if (!IS_SET(bitvector, AFF_ROOM_FOG))
                {
                    if (af->caster_id == ch->id && *name_self_affects[1] != '\0')
                    {
                        buffer << name_self_affects[1] << "\r\n";
                    }
                    else if (*name_affects[1] != '\0')
                    {
                        buffer << name_affects[1] << "\r\n";
                    }

                    SET_BIT(bitvector, AFF_ROOM_FOG);
                }
                break;
            case AFF_ROOM_RUNE_LABEL:				// 1 << 2
                if (af->caster_id == ch->id && *name_self_affects[2] != '\0')
                {
                    buffer << name_self_affects[2] << "\r\n";
                }
                else if (*name_affects[2] != '\0')
                {
                    buffer << name_affects[2] << "\r\n";
                }
                break;
            case AFF_ROOM_FORBIDDEN:				// 1 << 3
                if (!IS_SET(bitvector, AFF_ROOM_FORBIDDEN))
                {
                    if (af->caster_id == ch->id && *name_self_affects[3] != '\0')
                    {
                        buffer << name_self_affects[3] << "\r\n";
                    }
                    else if (*name_affects[3] != '\0')
                    {
                        buffer << name_affects[3] << "\r\n";
                    }

                    SET_BIT(bitvector, AFF_ROOM_FORBIDDEN);
                }
                break;
            case AFF_ROOM_HYPNOTIC_PATTERN:			// 1 << 4
                if (!IS_SET(bitvector, AFF_ROOM_HYPNOTIC_PATTERN))
                {
                    if (af->caster_id == ch->id && *name_self_affects[4] != '\0')
                    {
                        buffer << name_self_affects[4] << "\r\n";
                    }
                    else if (*name_affects[4] != '\0')
                    {
                        buffer << name_affects[4] << "\r\n";
                    }

                    SET_BIT(bitvector, AFF_ROOM_HYPNOTIC_PATTERN);
                }
                break;
            case AFF_ROOM_EVARDS_BLACK_TENTACLES:	// 1 << 5
                if (!IS_SET(bitvector, AFF_ROOM_EVARDS_BLACK_TENTACLES))
                {
                    if (af->caster_id == ch->id && *name_self_affects[5] != '\0')
                    {
                        buffer << name_self_affects[5] << "\r\n";
                    }
                    else if (*name_affects[5] != '\0')
                    {
                        buffer << name_affects[5] << "\r\n";
                    }

                    SET_BIT(bitvector, AFF_ROOM_EVARDS_BLACK_TENTACLES);
                }
                break;
            case AFF_ROOM_METEORSTORM:				// 1 << 6
                if (!IS_SET(bitvector, AFF_ROOM_METEORSTORM))
                {
                    if (af->caster_id == ch->id && *name_self_affects[6] != '\0')
                    {
                        buffer << name_self_affects[6] << "\r\n";
                    }
                    else if (*name_affects[6] != '\0')
                    {
                        buffer << name_affects[6] << "\r\n";
                    }

                    SET_BIT(bitvector, AFF_ROOM_METEORSTORM);
                }
                break;
            case AFF_ROOM_THUNDERSTORM:				// 1 << 7
                if (!IS_SET(bitvector, AFF_ROOM_THUNDERSTORM))
                {
                    if (af->caster_id == ch->id && *name_self_affects[7] != '\0')
                    {
                        buffer << name_self_affects[7] << "\r\n";
                    }
                    else if (*name_affects[7] != '\0')
                    {
                        buffer << name_affects[7] << "\r\n";
                    }

                    SET_BIT(bitvector, AFF_ROOM_THUNDERSTORM);
                }
                break;
            default:
                log("SYSERR: Unknown room affect: %d", af->type);
        }
    }

    auto affects = buffer.str();
    if (!affects.empty())
    {
        affects.append("\r\n");
        send_to_char(affects.c_str(), ch);
    }
}

void print_zone_info(CHAR_DATA *ch)
{
    ZoneData *zone = &zone_table[world[ch->in_room]->zone];
    std::stringstream out;
    out << "\r\n" << zone->name;

    bool delim = false;
    if (!zone->is_town)
    {
        delim = put_delim(out, delim);
        out << "средний уровень: " << zone->mob_level;
    }
    if (zone->group > 1)
    {
        delim = put_delim(out, delim);
        out << "групповая на " << zone->group
            << " " << desc_count(zone->group, WHAT_PEOPLE);
    }
    if (delim)
    {
        out << ")";
    }
    out << ".\r\n";

    send_to_char(out.str(), ch);
}

void do_looking(CHAR_DATA *ch, char* /*argument*/, int/* cmd*/, int/* subcmd*/)
{
    int i;

    if (!ch->desc)
        return;

    if (GET_POS(ch) < POS_SLEEPING)
        send_to_char("Белый Ангел возник перед вами, маняще помахивая крыльями.\r\n", ch);
    if (GET_POS(ch) == POS_SLEEPING)
        send_to_char("Виделся часто сон беспокойный...\r\n", ch);
    else if (AFF_FLAGGED(ch, EAffectFlag::AFF_BLIND))
        send_to_char("Вы ослеплены!\r\n", ch);
    else if (ch->get_skill(SKILL_LOOKING))
    {
        if (check_moves(ch, LOOKING_MOVES))
        {
            send_to_char("Вы напрягли зрение и начали присматриваться по сторонам.\r\n", ch);
            for (i = 0; i < NUM_OF_DIRS; i++)
                look_in_direction(ch, i, EXIT_SHOW_LOOKING);
            if (!(IS_IMMORTAL(ch) || GET_GOD_FLAG(ch, GF_GODSLIKE)))
                WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
        }
    }
    else
        send_to_char("Вам явно не хватает этого умения.\r\n", ch);
}

void list_one_char(CHAR_DATA * i, CHAR_DATA * ch, int skill_mode)
{
    int sector = SECT_CITY;
    int n;
    char aura_txt[200];
    const char *positions[] =
            {
                    "лежит здесь, мертвый. ",
                    "лежит здесь, при смерти. ",
                    "лежит здесь, без сознания. ",
                    "лежит здесь, в обмороке. ",
                    "спит здесь. ",
                    "отдыхает здесь. ",
                    "сидит здесь. ",
                    "СРАЖАЕТСЯ! ",
                    "стоит здесь. "
            };

    // Здесь и далее при использовании IS_POLY() - патч для отображения позиций мобов типа "они" -- Ковшегуб
    const char *poly_positions[] =
            {
                    "лежат здесь, мертвые. ",
                    "лежат здесь, при смерти. ",
                    "лежат здесь, без сознания. ",
                    "лежат здесь, в обмороке. ",
                    "спят здесь. ",
                    "отдыхают здесь. ",
                    "сидят здесь. ",
                    "СРАЖАЮТСЯ! ",
                    "стоят здесь. "
            };

    if (IS_HORSE(i) && on_horse(i->get_master()))
    {
        if (ch == i->get_master())
        {
            if (!IS_POLY(i))
            {
                act("$N несет вас на своей спине.", FALSE, ch, nullptr, i, TO_CHAR);
            }
            else
            {
                act("$N несут вас на своей спине.", FALSE, ch, nullptr, i, TO_CHAR);
            }
        }

        return;
    }

    if (skill_mode == SKILL_LOOKING)
    {
        if (HERE(i) && INVIS_OK(ch, i) && GET_REAL_LEVEL(ch) >= (IS_NPC(i) ? 0 : GET_INVIS_LEV(i)))
        {
            if (GET_RACE(i)==NPC_RACE_THING && IS_IMMORTAL(ch)) {
                sprintf(buf, "Вы разглядели %s.(предмет)\r\n", GET_PAD(i, 3));
            } else {
                sprintf(buf, "Вы разглядели %s.\r\n", GET_PAD(i, 3));
            }
            send_to_char(buf, ch);
        }
        return;
    }

    if (!CAN_SEE(ch, i))
    {
        skill_mode =
                check_awake(i, ACHECK_AFFECTS | ACHECK_LIGHT | ACHECK_HUMMING | ACHECK_GLOWING | ACHECK_WEIGHT);
        *buf = 0;
        if (IS_SET(skill_mode, ACHECK_AFFECTS))
        {
            REMOVE_BIT(skill_mode, ACHECK_AFFECTS);
            sprintf(buf + strlen(buf), "магический ореол%s", skill_mode ? ", " : " ");
        }
        if (IS_SET(skill_mode, ACHECK_LIGHT))
        {
            REMOVE_BIT(skill_mode, ACHECK_LIGHT);
            sprintf(buf + strlen(buf), "яркий свет%s", skill_mode ? ", " : " ");
        }
        if (IS_SET(skill_mode, ACHECK_GLOWING)
            && IS_SET(skill_mode, ACHECK_HUMMING)
            && !AFF_FLAGGED(ch, EAffectFlag::AFF_DEAFNESS))
        {
            REMOVE_BIT(skill_mode, ACHECK_GLOWING);
            REMOVE_BIT(skill_mode, ACHECK_HUMMING);
            sprintf(buf + strlen(buf), "шум и блеск экипировки%s", skill_mode ? ", " : " ");
        }
        if (IS_SET(skill_mode, ACHECK_GLOWING))
        {
            REMOVE_BIT(skill_mode, ACHECK_GLOWING);
            sprintf(buf + strlen(buf), "блеск экипировки%s", skill_mode ? ", " : " ");
        }
        if (IS_SET(skill_mode, ACHECK_HUMMING)
            && !AFF_FLAGGED(ch, EAffectFlag::AFF_DEAFNESS))
        {
            REMOVE_BIT(skill_mode, ACHECK_HUMMING);
            sprintf(buf + strlen(buf), "шум экипировки%s", skill_mode ? ", " : " ");
        }
        if (IS_SET(skill_mode, ACHECK_WEIGHT)
            && !AFF_FLAGGED(ch, EAffectFlag::AFF_DEAFNESS))
        {
            REMOVE_BIT(skill_mode, ACHECK_WEIGHT);
            sprintf(buf + strlen(buf), "бряцание металла%s", skill_mode ? ", " : " ");
        }
        strcat(buf, "выдает чье-то присутствие.\r\n");
        send_to_char(CAP(buf), ch);
        return;
    }

    if (IS_NPC(i)
        && !i->player_data.long_descr.empty()
        && GET_POS(i) == GET_DEFAULT_POS(i)
        && ch->in_room == i->in_room
        && !AFF_FLAGGED(i, EAffectFlag::AFF_CHARM)
        && !IS_HORSE(i))
    {
        *buf = '\0';
        if (PRF_FLAGGED(ch, PRF_ROOMFLAGS))
        {
            sprintf(buf, "[%5d] ", GET_MOB_VNUM(i));
        }

        if (AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_MAGIC)
            && !AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_ALIGN))
        {
            if (AFF_FLAGGED(i, EAffectFlag::AFF_EVILESS))
            {
                strcat(buf, "(черная аура) ");
            }
        }
        if (AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_ALIGN))
        {
            if (IS_NPC(i))
            {
                if (NPC_FLAGGED(i, NPC_AIRCREATURE))
                    sprintf(buf + strlen(buf), "%s(аура воздуха)%s ",
                            CCIBLU(ch, C_CMP), CCIRED(ch, C_CMP));
                else if (NPC_FLAGGED(i, NPC_WATERCREATURE))
                    sprintf(buf + strlen(buf), "%s(аура воды)%s ",
                            CCICYN(ch, C_CMP), CCIRED(ch, C_CMP));
                else if (NPC_FLAGGED(i, NPC_FIRECREATURE))
                    sprintf(buf + strlen(buf), "%s(аура огня)%s ",
                            CCIMAG(ch, C_CMP), CCIRED(ch, C_CMP));
                else if (NPC_FLAGGED(i, NPC_EARTHCREATURE))
                    sprintf(buf + strlen(buf), "%s(аура земли)%s ",
                            CCIGRN(ch, C_CMP), CCIRED(ch, C_CMP));
            }
        }
        if (AFF_FLAGGED(i, EAffectFlag::AFF_INVISIBLE))
            sprintf(buf + strlen(buf), "(невидим%s) ", GET_CH_SUF_6(i));
        if (AFF_FLAGGED(i, EAffectFlag::AFF_HIDE))
            sprintf(buf + strlen(buf), "(спрятал%s) ", GET_CH_SUF_2(i));
        if (AFF_FLAGGED(i, EAffectFlag::AFF_CAMOUFLAGE))
            sprintf(buf + strlen(buf), "(замаскировал%s) ", GET_CH_SUF_2(i));
        if (AFF_FLAGGED(i, EAffectFlag::AFF_FLY))
            strcat(buf, IS_POLY(i) ? "(летят) " : "(летит) ");
        if (AFF_FLAGGED(i, EAffectFlag::AFF_HORSE))
            strcat(buf, "(под седлом) ");

        strcat(buf, i->player_data.long_descr.c_str());
        send_to_char(buf, ch);

        *aura_txt = '\0';
        if (AFF_FLAGGED(i, EAffectFlag::AFF_SHIELD))
        {
            strcat(aura_txt, "...окутан");
            strcat(aura_txt, GET_CH_SUF_6(i));
            strcat(aura_txt, " сверкающим коконом ");
        }
        if (AFF_FLAGGED(i, EAffectFlag::AFF_SANCTUARY))
            strcat(aura_txt, IS_POLY(i) ? "...светятся ярким сиянием " : "...светится ярким сиянием ");
        else if (AFF_FLAGGED(i, EAffectFlag::AFF_PRISMATICAURA))
            strcat(aura_txt, IS_POLY(i) ? "...переливаются всеми цветами " : "...переливается всеми цветами ");
        act(aura_txt, FALSE, i, nullptr, ch, TO_VICT);

        *aura_txt = '\0';
        n = 0;
        strcat(aura_txt, "...окружен");
        strcat(aura_txt, GET_CH_SUF_6(i));
        if (AFF_FLAGGED(i, EAffectFlag::AFF_AIRSHIELD))
        {
            strcat(aura_txt, " воздушным");
            n++;
        }
        if (AFF_FLAGGED(i, EAffectFlag::AFF_FIRESHIELD))
        {
            if (n > 0)
                strcat(aura_txt, ", огненным");
            else
                strcat(aura_txt, " огненным");
            n++;
        }
        if (AFF_FLAGGED(i, EAffectFlag::AFF_ICESHIELD))
        {
            if (n > 0)
                strcat(aura_txt, ", ледяным");
            else
                strcat(aura_txt, " ледяным");
            n++;
        }
        if (n == 1)
            strcat(aura_txt, " щитом ");
        else if (n > 1)
            strcat(aura_txt, " щитами ");
        if (n > 0)
            act(aura_txt, FALSE, i, 0, ch, TO_VICT);

        if (AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_MAGIC))
        {
            *aura_txt = '\0';
            n = 0;
            strcat(aura_txt, "...");
            if (AFF_FLAGGED(i, EAffectFlag::AFF_MAGICGLASS)) {
                if (n > 0)
                    strcat(aura_txt, ", серебристая");
                else
                    strcat(aura_txt, "серебристая");
                n++;
            }
            if (AFF_FLAGGED(i, EAffectFlag::AFF_BROKEN_CHAINS)) {
                if (n > 0)
                    strcat(aura_txt, ", ярко-синяя");
                else
                    strcat(aura_txt, "ярко-синяя");
                n++;
            }
            if (AFF_FLAGGED(i, EAffectFlag::AFF_EVILESS)) {
                if (n > 0)
                    strcat(aura_txt, ", черная");
                else
                    strcat(aura_txt, "черная");
                n++;
            }
            if (n == 1)
                strcat(aura_txt, " аура ");
            else if (n > 1)
                strcat(aura_txt, " ауры ");

            if (n > 0)
                act(aura_txt, FALSE, i, 0, ch, TO_VICT);
        }
        *aura_txt = '\0';
        if (AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_MAGIC))
        {
            if (AFF_FLAGGED(i, EAffectFlag::AFF_HOLD))
                strcat(aura_txt, "...парализован$a");
            if (AFF_FLAGGED(i, EAffectFlag::AFF_SILENCE))
                strcat(aura_txt, "...нем$a");
        }
        if (AFF_FLAGGED(i, EAffectFlag::AFF_BLIND))
            strcat(aura_txt, "...слеп$a");
        if (AFF_FLAGGED(i, EAffectFlag::AFF_DEAFNESS))
            strcat(aura_txt, "...глух$a");
        if (AFF_FLAGGED(i, EAffectFlag::AFF_STRANGLED))
            strcat(aura_txt, "...задыхается.");

        if (*aura_txt)
            act(aura_txt, FALSE, i, 0, ch, TO_VICT);

        return;
    }

    if (IS_NPC(i))
    {
        strcpy(buf1, i->get_npc_name().c_str());
        strcat(buf1, " ");
        if (AFF_FLAGGED(i, EAffectFlag::AFF_HORSE))
            strcat(buf1, "(под седлом) ");
        CAP(buf1);
    }
    else
    {
        sprintf(buf1, "%s%s ", i->get_morphed_title().c_str(), PLR_FLAGGED(i, PLR_KILLER) ? " <ДУШЕГУБ>" : "");
    }

    sprintf(buf, "%s%s", AFF_FLAGGED(i, EAffectFlag::AFF_CHARM) ? "*" : "", buf1);
    if (AFF_FLAGGED(i, EAffectFlag::AFF_INVISIBLE))
        sprintf(buf + strlen(buf), "(невидим%s) ", GET_CH_SUF_6(i));
    if (AFF_FLAGGED(i, EAffectFlag::AFF_HIDE))
        sprintf(buf + strlen(buf), "(спрятал%s) ", GET_CH_SUF_2(i));
    if (AFF_FLAGGED(i, EAffectFlag::AFF_CAMOUFLAGE))
        sprintf(buf + strlen(buf), "(замаскировал%s) ", GET_CH_SUF_2(i));
    if (!IS_NPC(i) && !i->desc)
        sprintf(buf + strlen(buf), "(потерял%s связь) ", GET_CH_SUF_1(i));
    if (!IS_NPC(i) && PLR_FLAGGED(i, PLR_WRITING))
        strcat(buf, "(пишет) ");

    if (GET_POS(i) != POS_FIGHTING)
    {
        if (on_horse(i))
        {
            CHAR_DATA *horse = get_horse(i);
            if (horse)
            {
                const char *msg =
                        AFF_FLAGGED(horse, EAffectFlag::AFF_FLY) ? "летает" : "сидит";
                sprintf(buf + strlen(buf), "%s здесь верхом на %s. ",
                        msg, PERS(horse, ch, 5));
            }
        }
        else if (IS_HORSE(i) && AFF_FLAGGED(i, EAffectFlag::AFF_TETHERED))
            sprintf(buf + strlen(buf), "привязан%s здесь. ", GET_CH_SUF_6(i));
        else if ((sector = real_sector(i->in_room)) == SECT_FLYING)
            strcat(buf, IS_POLY(i) ? "летают здесь. " : "летает здесь. ");
        else if (sector == SECT_UNDERWATER)
            strcat(buf, IS_POLY(i) ? "плавают здесь. " : "плавает здесь. ");
        else if (GET_POS(i) > POS_SLEEPING && AFF_FLAGGED(i, EAffectFlag::AFF_FLY))
            strcat(buf, IS_POLY(i) ? "летают здесь. " : "летает здесь. ");
        else if (sector == SECT_WATER_SWIM || sector == SECT_WATER_NOSWIM)
            strcat(buf, IS_POLY(i) ? "плавают здесь. " : "плавает здесь. ");
        else
            strcat(buf, IS_POLY(i) ? poly_positions[static_cast<int>(GET_POS(i))] : positions[static_cast<int>(GET_POS(i))]);
        if (AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_MAGIC) && IS_NPC(i) && affected_by_spell(i, SPELL_CAPABLE))
            sprintf(buf + strlen(buf), "(аура магии) ");
    }
    else
    {
        if (i->get_fighting())
        {
            strcat(buf, IS_POLY(i) ? "сражаются с " : "сражается с ");
            if (i->in_room != i->get_fighting()->in_room)
                strcat(buf, "чьей-то тенью");
            else if (i->get_fighting() == ch)
                strcat(buf, "ВАМИ");
            else
                strcat(buf, GET_PAD(i->get_fighting(), 4));
            if (on_horse(i))
                sprintf(buf + strlen(buf), ", сидя верхом на %s! ", PERS(get_horse(i), ch, 5));
            else
                strcat(buf, "! ");
        }
        else		// NIL fighting pointer
        {
            strcat(buf, IS_POLY(i) ? "колотят по воздуху" : "колотит по воздуху");
            if (on_horse(i))
                sprintf(buf + strlen(buf), ", сидя верхом на %s. ", PERS(get_horse(i), ch, 5));
            else
                strcat(buf, ". ");
        }
    }

    if (AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_MAGIC)
        && !AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_ALIGN))
    {
        if (AFF_FLAGGED(i, EAffectFlag::AFF_EVILESS))
            strcat(buf, "(черная аура) ");
    }
    if (AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_ALIGN))
    {
        if (IS_NPC(i))
        {
            if (IS_EVIL(i))
            {
                if (AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_MAGIC)
                    && AFF_FLAGGED(i, EAffectFlag::AFF_EVILESS))
                    strcat(buf, "(иссиня-черная аура) ");
                else
                    strcat(buf, "(темная аура) ");
            }
            else if (IS_GOOD(i))
            {
                if (AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_MAGIC)
                    && AFF_FLAGGED(i, EAffectFlag::AFF_EVILESS))
                    strcat(buf, "(серая аура) ");
                else
                    strcat(buf, "(светлая аура) ");
            }
            else
            {
                if (AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_MAGIC)
                    && AFF_FLAGGED(i, EAffectFlag::AFF_EVILESS))
                    strcat(buf, "(черная аура) ");
            }
        }
        else
        {
            aura(ch, C_CMP, i, aura_txt);
            strcat(buf, aura_txt);
            strcat(buf, " ");
        }
    }
    if (AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_POISON))
        if (AFF_FLAGGED(i, EAffectFlag::AFF_POISON))
            sprintf(buf + strlen(buf), "(отравлен%s) ", GET_CH_SUF_6(i));

    strcat(buf, "\r\n");
    send_to_char(buf, ch);

    *aura_txt = '\0';
    if (AFF_FLAGGED(i, EAffectFlag::AFF_SHIELD))
    {
        strcat(aura_txt, "...окутан");
        strcat(aura_txt, GET_CH_SUF_6(i));
        strcat(aura_txt, " сверкающим коконом ");
    }
    if (AFF_FLAGGED(i, EAffectFlag::AFF_SANCTUARY))
        strcat(aura_txt, IS_POLY(i) ? "...светятся ярким сиянием " : "...светится ярким сиянием ");
    else if (AFF_FLAGGED(i, EAffectFlag::AFF_PRISMATICAURA))
        strcat(aura_txt, IS_POLY(i) ? "...переливаются всеми цветами " : "...переливается всеми цветами ");
    act(aura_txt, FALSE, i, 0, ch, TO_VICT);

    *aura_txt = '\0';
    n = 0;
    strcat(aura_txt, "...окружен");
    strcat(aura_txt, GET_CH_SUF_6(i));
    if (AFF_FLAGGED(i, EAffectFlag::AFF_AIRSHIELD))
    {
        strcat(aura_txt, " воздушным");
        n++;
    }
    if (AFF_FLAGGED(i, EAffectFlag::AFF_FIRESHIELD))
    {
        if (n > 0)
            strcat(aura_txt, ", огненным");
        else
            strcat(aura_txt, " огненным");
        n++;
    }
    if (AFF_FLAGGED(i, EAffectFlag::AFF_ICESHIELD))
    {
        if (n > 0)
            strcat(aura_txt, ", ледяным");
        else
            strcat(aura_txt, " ледяным");
        n++;
    }
    if (n == 1)
        strcat(aura_txt, " щитом ");
    else if (n > 1)
        strcat(aura_txt, " щитами ");
    if (n > 0)
        act(aura_txt, FALSE, i, 0, ch, TO_VICT);
    if (AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_ALIGN))
    {
        *aura_txt = '\0';
        if (AFF_FLAGGED(i, EAffectFlag::AFF_COMMANDER))
            strcat(aura_txt, "... реет стяг над головой ");
        if (*aura_txt)
            act(aura_txt, FALSE, i, 0, ch, TO_VICT);
    }
    if (AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_MAGIC))
    {
        *aura_txt = '\0';
        n = 0;
        strcat(aura_txt, " ..");
        if (AFF_FLAGGED(i, EAffectFlag::AFF_MAGICGLASS))
        {
            if (n > 0)
                strcat(aura_txt, ", серебристая");
            else
                strcat(aura_txt, "серебристая");
            n++;
        }
        if (AFF_FLAGGED(i, EAffectFlag::AFF_BROKEN_CHAINS))
        {
            if (n > 0)
                strcat(aura_txt, ", ярко-синяя");
            else
                strcat(aura_txt, "ярко-синяя");
            n++;
        }
        if (n == 1)
            strcat(aura_txt, " аура ");
        else if (n > 1)
            strcat(aura_txt, " ауры ");

        if (n > 0)
            act(aura_txt, FALSE, i, 0, ch, TO_VICT);
    }
    *aura_txt = '\0';
    if (AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_MAGIC))
    {
        if (AFF_FLAGGED(i, EAffectFlag::AFF_HOLD))
            strcat(aura_txt, " ...парализован$a");
        if (AFF_FLAGGED(i, EAffectFlag::AFF_SILENCE))
            strcat(aura_txt, " ...нем$a");
    }
    if (AFF_FLAGGED(i, EAffectFlag::AFF_BLIND))
        strcat(aura_txt, " ...слеп$a");
    if (AFF_FLAGGED(i, EAffectFlag::AFF_DEAFNESS))
        strcat(aura_txt, " ...глух$a");
    if (AFF_FLAGGED(i, EAffectFlag::AFF_STRANGLED))
        strcat(aura_txt, " ...задыхается");
    if (*aura_txt)
        act(aura_txt, FALSE, i, 0, ch, TO_VICT);
    if (IS_MANA_CASTER(i))
    {
        *aura_txt = '\0';
        if (i->get_trained_skill(SKILL_DARK_MAGIC) > 0)
            strcat(aura_txt,  "...все сферы магии кружатся над головой");
        else if (i->get_trained_skill(SKILL_AIR_MAGIC) > 0)
            strcat(aura_txt, "...сферы четырех магий кружатся над головой");
        else if (i->get_trained_skill(SKILL_EARTH_MAGIC) > 0)
            strcat(aura_txt, "...сферы трех магий кружатся над головой");
        else if  (i->get_trained_skill(SKILL_WATER_MAGIC) > 0)
            strcat(aura_txt, "...сферы двух магий кружатся над головой");
        else if (i->get_trained_skill(SKILL_FIRE_MAGIC) > 0)
            strcat(aura_txt, "...сфера огня кружит над головой");
        if (*aura_txt)
            act(aura_txt, FALSE, i, 0, ch, TO_VICT);
    }

}

void diag_char_to_char(CHAR_DATA * i, CHAR_DATA * ch)
{
    int percent;

    if (GET_REAL_MAX_HIT(i) > 0)
        percent = (100 * GET_HIT(i)) / GET_REAL_MAX_HIT(i);
    else
        percent = -1;	// How could MAX_HIT be < 1??

    strcpy(buf, PERS(i, ch, 0));
    CAP(buf);

    if (percent >= 100)
    {
        sprintf(buf2, " невредим%s", GET_CH_SUF_6(i));
        strcat(buf, buf2);
    }
    else if (percent >= 90)
    {
        sprintf(buf2, " слегка поцарапан%s", GET_CH_SUF_6(i));
        strcat(buf, buf2);
    }
    else if (percent >= 75)
    {
        sprintf(buf2, " легко ранен%s", GET_CH_SUF_6(i));
        strcat(buf, buf2);
    }
    else if (percent >= 50)
    {
        sprintf(buf2, " ранен%s", GET_CH_SUF_6(i));
        strcat(buf, buf2);
    }
    else if (percent >= 30)
    {
        sprintf(buf2, " тяжело ранен%s", GET_CH_SUF_6(i));
        strcat(buf, buf2);
    }
    else if (percent >= 15)
    {
        sprintf(buf2, " смертельно ранен%s", GET_CH_SUF_6(i));
        strcat(buf, buf2);
    }
    else if (percent >= 0)
        strcat(buf, " в ужасном состоянии");
    else
        strcat(buf, " умирает");

    if (!on_horse(i))
        switch (GET_POS(i))
        {
            case POS_MORTALLYW:
                strcat(buf, ".");
                break;
            case POS_INCAP:
                strcat(buf, IS_POLY(i) ? ", лежат без сознания." : ", лежит без сознания.");
                break;
            case POS_STUNNED:
                strcat(buf, IS_POLY(i) ? ", лежат в обмороке." : ", лежит в обмороке.");
                break;
            case POS_SLEEPING:
                strcat(buf, IS_POLY(i) ? ", спят." : ", спит.");
                break;
            case POS_RESTING:
                strcat(buf, IS_POLY(i) ? ", отдыхают." : ", отдыхает.");
                break;
            case POS_SITTING:
                strcat(buf, IS_POLY(i) ? ", сидят." : ", сидит.");
                break;
            case POS_STANDING:
                strcat(buf, IS_POLY(i) ? ", стоят." : ", стоит.");
                break;
            case POS_FIGHTING:
                if (i->get_fighting())
                    strcat(buf, IS_POLY(i) ? ", сражаются." : ", сражается.");
                else
                    strcat(buf, IS_POLY(i) ? ", махают кулаками." : ", махает кулаками.");
                break;
            default:
                return;
                break;
        }
    else
        strcat(buf, IS_POLY(i) ? ", сидят верхом." : ", сидит верхом.");

    if (AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_POISON))
        if (AFF_FLAGGED(i, EAffectFlag::AFF_POISON))
        {
            sprintf(buf2, " (отравлен%s)", GET_CH_SUF_6(i));
            strcat(buf, buf2);
        }

    strcat(buf, "\r\n");
    send_to_char(buf, ch);

}

void look_at_char(CHAR_DATA * i, CHAR_DATA * ch)
{
    int j, found, push_count = 0;
    OBJ_DATA *tmp_obj, *push = NULL;

    if (!ch->desc)
        return;

    if (i->player_data.description != "")
    {
        if (IS_NPC(i))
            send_to_char(ch, " * %s", i->player_data.description.c_str());
        else
            send_to_char(ch, "*\r\n%s*\r\n", space_before_string(i->player_data.description).c_str());
    }
    else if (!IS_NPC(i))
    {
        strcpy(buf, "\r\nЭто");
        if (i->is_morphed())
            strcat(buf, std::string(" "+i->get_morph_desc()+".\r\n").c_str());
        else
        if (IS_FEMALE(i))
        {
            if (GET_HEIGHT(i) <= 151)
            {
                if (GET_WEIGHT(i) >= 140)
                    strcat(buf, " маленькая плотная дамочка.\r\n");
                else if (GET_WEIGHT(i) >= 125)
                    strcat(buf, " маленькая женщина.\r\n");
                else
                    strcat(buf, " миниатюрная дамочка.\r\n");
            }
            else if (GET_HEIGHT(i) <= 159)
            {
                if (GET_WEIGHT(i) >= 145)
                    strcat(buf, " невысокая плотная мадам.\r\n");
                else if (GET_WEIGHT(i) >= 130)
                    strcat(buf, " невысокая женщина.\r\n");
                else
                    strcat(buf, " изящная леди.\r\n");
            }
            else if (GET_HEIGHT(i) <= 165)
            {
                if (GET_WEIGHT(i) >= 145)
                    strcat(buf, " среднего роста женщина.\r\n");
                else
                    strcat(buf, " среднего роста изящная красавица.\r\n");
            }
            else if (GET_HEIGHT(i) <= 175)
            {
                if (GET_WEIGHT(i) >= 150)
                    strcat(buf, " высокая дородная баба.\r\n");
                else if (GET_WEIGHT(i) >= 135)
                    strcat(buf, " высокая стройная женщина.\r\n");
                else
                    strcat(buf, " высокая изящная женщина.\r\n");
            }
            else
            {
                if (GET_WEIGHT(i) >= 155)
                    strcat(buf, " очень высокая крупная дама.\r\n");
                else if (GET_WEIGHT(i) >= 140)
                    strcat(buf, " очень высокая стройная женщина.\r\n");
                else
                    strcat(buf, " очень высокая худощавая женщина.\r\n");
            }
        }
        else
        {
            if (GET_HEIGHT(i) <= 165)
            {
                if (GET_WEIGHT(i) >= 170)
                    strcat(buf, " маленький, похожий на колобок, мужчина.\r\n");
                else if (GET_WEIGHT(i) >= 150)
                    strcat(buf, " маленький плотный мужчина.\r\n");
                else
                    strcat(buf, " маленький плюгавенький мужичонка.\r\n");
            }
            else if (GET_HEIGHT(i) <= 175)
            {
                if (GET_WEIGHT(i) >= 175)
                    strcat(buf, " невысокий коренастый крепыш.\r\n");
                else if (GET_WEIGHT(i) >= 160)
                    strcat(buf, " невысокий крепкий мужчина.\r\n");
                else
                    strcat(buf, " невысокий худощавый мужчина.\r\n");
            }
            else if (GET_HEIGHT(i) <= 185)
            {
                if (GET_WEIGHT(i) >= 180)
                    strcat(buf, " среднего роста коренастый мужчина.\r\n");
                else if (GET_WEIGHT(i) >= 165)
                    strcat(buf, " среднего роста крепкий мужчина.\r\n");
                else
                    strcat(buf, " среднего роста худощавый мужчина.\r\n");
            }
            else if (GET_HEIGHT(i) <= 195)
            {
                if (GET_WEIGHT(i) >= 185)
                    strcat(buf, " высокий крупный мужчина.\r\n");
                else if (GET_WEIGHT(i) >= 170)
                    strcat(buf, " высокий стройный мужчина.\r\n");
                else
                    strcat(buf, " длинный, худощавый мужчина.\r\n");
            }
            else
            {
                if (GET_WEIGHT(i) >= 190)
                    strcat(buf, " огромный мужик.\r\n");
                else if (GET_WEIGHT(i) >= 180)
                    strcat(buf, " очень высокий, крупный амбал.\r\n");
                else
                    strcat(buf, " длиннющий, похожий на жердь мужчина.\r\n");
            }
        }
        send_to_char(buf, ch);
    }
    else
        act("\r\nНичего необычного в $n5 вы не заметили.", FALSE, i, 0, ch, TO_VICT);

    if (AFF_FLAGGED(i, EAffectFlag::AFF_CHARM)
        && i->get_master() == ch)
    {
        if (i->low_charm())
        {
            act("$n скоро перестанет следовать за вами.", FALSE, i, 0, ch, TO_VICT);
        }
        else
        {
            for (const auto& aff : i->affected)
            {
                if (aff->type == SPELL_CHARM)
                {
                    sprintf(buf, IS_POLY(i) ? "$n будут слушаться вас еще %d %s." : "$n будет слушаться вас еще %d %s.", aff->duration / 2, desc_count(aff->duration / 2, 1));
                    act(buf, FALSE, i, 0, ch, TO_VICT);
                    break;
                }
            }
        }
    }

    if (IS_HORSE(i)
        && i->get_master() == ch)
    {
        strcpy(buf, "\r\nЭто ваш скакун. Он ");
        if (GET_HORSESTATE(i) <= 0)
            strcat(buf, "загнан.\r\n");
        else if (GET_HORSESTATE(i) <= 20)
            strcat(buf, "весь в мыле.\r\n");
        else if (GET_HORSESTATE(i) <= 80)
            strcat(buf, "в хорошем состоянии.\r\n");
        else
            strcat(buf, "выглядит совсем свежим.\r\n");
        send_to_char(buf, ch);
    };

    diag_char_to_char(i, ch);

    if (i->is_morphed())
    {
        send_to_char("\r\n", ch);
        std::string coverDesc = "$n покрыт$a " + i->get_cover_desc()+".";
        act(coverDesc.c_str(), FALSE, i, 0, ch, TO_VICT);
        send_to_char("\r\n", ch);
    }
    else
    {
        found = FALSE;
        for (j = 0; !found && j < NUM_WEARS; j++)
            if (GET_EQ(i, j) && CAN_SEE_OBJ(ch, GET_EQ(i, j)))
                found = TRUE;

        if (found)
        {
            send_to_char("\r\n", ch);
            act("$n одет$a :", FALSE, i, 0, ch, TO_VICT);
            for (j = 0; j < NUM_WEARS; j++)
            {
                if (GET_EQ(i, j) && CAN_SEE_OBJ(ch, GET_EQ(i, j)))
                {
                    send_to_char(where[j], ch);
                    if (i->has_master()
                        && IS_NPC(i))
                    {
                        show_obj_to_char(GET_EQ(i, j), ch, 1, ch == i->get_master(), 1);
                    }
                    else
                    {
                        show_obj_to_char(GET_EQ(i, j), ch, 1, ch == i, 1);
                    }
                }
            }
        }
    }

    if (ch != i && (ch->get_skill(SKILL_LOOK_HIDE) || IS_IMMORTAL(ch)))
    {
        found = FALSE;
        act("\r\nВы попытались заглянуть в $s ношу:", FALSE, i, 0, ch, TO_VICT);
        for (tmp_obj = i->carrying; tmp_obj; tmp_obj = tmp_obj->get_next_content())
        {
            if (CAN_SEE_OBJ(ch, tmp_obj) && (number(0, 30) < GET_LEVEL(ch)))
            {
                if (!push)
                {
                    push = tmp_obj;
                    push_count = 1;
                }
                else if (GET_OBJ_VNUM(push) != GET_OBJ_VNUM(tmp_obj)
                         || GET_OBJ_VNUM(push) == -1)
                {
                    show_obj_to_char(push, ch, 1, ch == i, push_count);
                    push = tmp_obj;
                    push_count = 1;
                }
                else
                    push_count++;
                found = TRUE;
            }
        }
        if (push && push_count)
            show_obj_to_char(push, ch, 1, ch == i, push_count);
        if (!found)
            send_to_char("...и ничего не обнаружили.\r\n", ch);
    }
}

void list_char_to_char(const ROOM_DATA::people_t& list, CHAR_DATA* ch)
{
    for (const auto i : list)
    {
        if (ch != i)
        {
            if (HERE(i) && (GET_RACE(i) != NPC_RACE_THING)
                && (CAN_SEE(ch, i)
                    || awaking(i, AW_HIDE | AW_INVIS | AW_CAMOUFLAGE)))
            {
                list_one_char(i, ch, 0);
            }
            else if (IS_DARK(i->in_room)
                     && i->in_room == ch->in_room
                     && !CAN_SEE_IN_DARK(ch)
                     && AFF_FLAGGED(i, EAffectFlag::AFF_INFRAVISION))
            {
                send_to_char("Пара светящихся глаз смотрит на вас.\r\n", ch);
            }
        }
    }
}

void list_obj_to_char(OBJ_DATA * list, CHAR_DATA * ch, int mode, int show)
{
    OBJ_DATA *i, *push = NULL;
    bool found = FALSE;
    int push_count = 0;
    std::ostringstream buffer;
    long count = 0, cost = 0;

    bool clan_chest = false;
    if (mode == 1 && (show == 3 || show == 4))
    {
        clan_chest = true;
    }

    for (i = list; i; i = i->get_next_content())
    {
        if (CAN_SEE_OBJ(ch, i))
        {
            if (!push)
            {
                push = i;
                push_count = 1;
            }
            else if ((!equal_obj(i, push))
                     || (i->quest_item()))
            {
                if (clan_chest)
                {
                    buffer << show_obj_to_char(push, ch, mode, show, push_count);
                    count += push_count;
                    cost += GET_OBJ_RENTEQ(push) * push_count;
                }
                else
                    show_obj_to_char(push, ch, mode, show, push_count);
                push = i;
                push_count = 1;
            }
            else
                push_count++;
            found = TRUE;
        }
    }
    if (push && push_count)
    {
        if (clan_chest)
        {
            buffer << show_obj_to_char(push, ch, mode, show, push_count);
            count += push_count;
            cost += GET_OBJ_RENTEQ(push) * push_count;
        }
        else
            show_obj_to_char(push, ch, mode, show, push_count);
    }
    if (!found && show)
    {
        if (show == 1)
            send_to_char(" Внутри ничего нет.\r\n", ch);
        else if (show == 2)
            send_to_char(" Вы ничего не несете.\r\n", ch);
        else if (show == 3)
        {
            send_to_char(" Пусто...\r\n", ch);
            return;
        }
    }
    if (clan_chest)
        page_string(ch->desc, buffer.str());
}

void look_in_obj(CHAR_DATA * ch, char *arg)
{
    OBJ_DATA *obj = NULL;
    CHAR_DATA *dummy = NULL;
    char whatp[MAX_INPUT_LENGTH], where[MAX_INPUT_LENGTH];
    int amt, bits;
    int where_bits = FIND_OBJ_INV | FIND_OBJ_ROOM | FIND_OBJ_EQUIP;

    if (!*arg)
        send_to_char("Смотреть во что?\r\n", ch);
    else
        half_chop(arg, whatp, where);

    if (isname(where, "земля комната room ground"))
        where_bits = FIND_OBJ_ROOM;
    else if (isname(where, "инвентарь inventory"))
        where_bits = FIND_OBJ_INV;
    else if (isname(where, "экипировка equipment"))
        where_bits = FIND_OBJ_EQUIP;

    bits = generic_find(arg, where_bits, ch, &dummy, &obj);

    if ((obj == NULL) || !bits)
    {
        sprintf(buf, "Вы не видите здесь '%s'.\r\n", arg);
        send_to_char(buf, ch);
    }
    else if (GET_OBJ_TYPE(obj) != OBJ_DATA::ITEM_DRINKCON
             && GET_OBJ_TYPE(obj) != OBJ_DATA::ITEM_FOUNTAIN
             && GET_OBJ_TYPE(obj) != OBJ_DATA::ITEM_CONTAINER)
    {
        send_to_char("Ничего в нем нет!\r\n", ch);
    }
    else
    {
        if (Clan::ChestShow(obj, ch))
        {
            return;
        }
        if (ClanSystem::show_ingr_chest(obj, ch))
        {
            return;
        }
        if (Depot::is_depot(obj))
        {
            Depot::show_depot(ch);
            return;
        }

        if (GET_OBJ_TYPE(obj) == OBJ_DATA::ITEM_CONTAINER)
        {
            if (OBJVAL_FLAGGED(obj, CONT_CLOSED))
            {
                act("Закрыт$A.", FALSE, ch, obj, 0, TO_CHAR);
                const int skill_pick = ch->get_skill(SKILL_PICK_LOCK) ;
                int count = sprintf(buf, "Заперт%s.", GET_OBJ_SUF_6(obj));
                if (OBJVAL_FLAGGED(obj, CONT_LOCKED) && skill_pick)
                {
                    if (OBJVAL_FLAGGED(obj, CONT_PICKPROOF))
                        count += sprintf(buf+count, "%s Вы никогда не сможете ЭТО взломать!%s\r\n", CCICYN(ch, C_NRM), CCNRM(ch, C_NRM));
                    else if (OBJVAL_FLAGGED(obj, CONT_BROKEN))
                        count += sprintf(buf+count, "%s Замок сломан... %s\r\n", CCRED(ch, C_NRM), CCNRM(ch, C_NRM));
                    else
                    {
                        const int chance = get_pick_chance(skill_pick, GET_OBJ_VAL(obj, 3));
                        const int index = chance ? chance/5 + 1 : 0;
                        std::string color = Locks[index][1];
                        if (abs(skill_pick - GET_OBJ_VAL(obj, 3))>10)
                            color = KIDRK;

                        if (COLOR_LEV(ch)>C_NRM)
                            count += sprintf(buf + count, Locks[index][0], color.c_str(), KNRM);
                        else
                            count += sprintf(buf + count, Locks[index][0], KNUL, KNUL);
                    }
                    send_to_char(buf, ch);
                }
            }
            else
            {
                send_to_char(OBJN(obj, ch, 0), ch);
                switch (bits)
                {
                    case FIND_OBJ_INV:
                        send_to_char("(в руках)\r\n", ch);
                        break;
                    case FIND_OBJ_ROOM:
                        send_to_char("(на земле)\r\n", ch);
                        break;
                    case FIND_OBJ_EQUIP:
                        send_to_char("(в амуниции)\r\n", ch);
                        break;
                }
                if (!obj->get_contains())
                    send_to_char(" Внутри ничего нет.\r\n", ch);
                else
                {
                    if (GET_OBJ_VAL(obj, 0) > 0 && bits != FIND_OBJ_ROOM) {
                        /* amt - индекс массива из 6 элементов (0..5) с описанием наполненности
                           с помощью нехитрых мат. преобразований мы получаем соотношение веса и максимального объема контейнера,
                           выраженные числами от 0 до 5. (причем 5 будет лишь при полностью полном контейнере)
                        */
                        amt = MAX(0, MIN(5, (GET_OBJ_WEIGHT(obj) * 100) / (GET_OBJ_VAL(obj, 0) *  20)));
                        //sprintf(buf, "DEBUG 1: %d 2: %d 3: %d.\r\n", GET_OBJ_WEIGHT(obj), GET_OBJ_VAL(obj, 0), amt);
                        //send_to_char(buf, ch);
                        sprintf(buf, "Заполнен%s содержимым %s:\r\n", GET_OBJ_SUF_6(obj), fullness[amt]);
                        send_to_char(buf, ch);
                    }
                    list_obj_to_char(obj->get_contains(), ch, 1, bits != FIND_OBJ_ROOM);
                }
            }
        }
        else  	// item must be a fountain or drink container
        {
            if (GET_OBJ_VAL(obj, 1) <= 0)
                send_to_char("Пусто.\r\n", ch);
            else
            {
                if (GET_OBJ_VAL(obj, 0) <= 0 || GET_OBJ_VAL(obj, 1) > GET_OBJ_VAL(obj, 0))
                {
                    sprintf(buf, "Заполнен%s вакуумом?!\r\n", GET_OBJ_SUF_6(obj));	// BUG
                }
                else
                {
                    const char* msg = AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_POISON)
                                      && obj->get_val(3) == 1 ? "(отравленной)" : "";
                    amt = (GET_OBJ_VAL(obj, 1) * 5) / GET_OBJ_VAL(obj, 0);
                    sprinttype(GET_OBJ_VAL(obj, 2), color_liquid, buf2);
                    sprintf(buf, "Наполнен%s %s%s%s жидкостью.\r\n", GET_OBJ_SUF_6(obj), fullness[amt], buf2, msg);
                }
                send_to_char(buf, ch);
            }
        }
    }
}

void look_in_direction(CHAR_DATA * ch, int dir, int info_is)
{
    int count = 0, probe, percent;
    ROOM_DATA::exit_data_ptr rdata;

    if (CAN_GO(ch, dir)
        || (EXIT(ch, dir)
            && EXIT(ch, dir)->to_room() != NOWHERE))
    {
        rdata = EXIT(ch, dir);
        count += sprintf(buf, "%s%s:%s ", CCYEL(ch, C_NRM), Dirs[dir], CCNRM(ch, C_NRM));
        if (EXIT_FLAGGED(rdata, EX_CLOSED))
        {
            if (rdata->keyword)
            {
                count += sprintf(buf + count, " закрыто (%s).\r\n", rdata->keyword);
            }
            else
            {
                count += sprintf(buf + count, " закрыто (вероятно дверь).\r\n");
            }

            const int skill_pick = ch->get_skill(SKILL_PICK_LOCK) ;
            if (EXIT_FLAGGED(rdata, EX_LOCKED) && skill_pick)
            {
                if (EXIT_FLAGGED(rdata, EX_PICKPROOF))
                {
                    count += sprintf(buf+count-2, "%s вы никогда не сможете ЭТО взломать!%s\r\n", CCICYN(ch, C_NRM), CCNRM(ch, C_NRM));
                }
                else if (EXIT_FLAGGED(rdata, EX_BROKEN))
                {
                    count += sprintf(buf+count-2, "%s Замок сломан... %s\r\n", CCRED(ch, C_NRM), CCNRM(ch, C_NRM));
                }
                else
                {
                    const int chance = get_pick_chance(skill_pick, rdata->lock_complexity);
                    const int index = chance ? chance/5 + 1 : 0;

                    std::string color = Locks[index][1];
                    if (abs(skill_pick - rdata->lock_complexity)>10)
                    {
                        color = KIDRK;
                    }
                    if (COLOR_LEV(ch)>C_NRM)
                    {
                        count += sprintf(buf + count - 2, Locks[index][0], color.c_str(), KNRM);
                    }
                    else
                    {
                        count += sprintf(buf + count - 2, Locks[index][0], KNUL, KNUL);
                    }
                }
            }

            send_to_char(buf, ch);
            return;
        }

        if (IS_TIMEDARK(rdata->to_room()))
        {
            count += sprintf(buf + count, " слишком темно.\r\n");
            send_to_char(buf, ch);
            if (info_is & EXIT_SHOW_LOOKING)
            {
                send_to_char("&R&q", ch);
                count = 0;
                for (const auto tch : world[rdata->to_room()]->people)
                {
                    percent = number(1, skill_info[SKILL_LOOKING].max_percent);
                    probe =
                            train_skill(ch, SKILL_LOOKING, skill_info[SKILL_LOOKING].max_percent, tch);
                    if (HERE(tch) && INVIS_OK(ch, tch) && probe >= percent
                        && (percent < 100 || IS_IMMORTAL(ch)))
                    {
                        // Если моб не вещь и смотрящий не им
                        if ( GET_RACE(tch) != NPC_RACE_THING || IS_IMMORTAL(ch) ) {
                            list_one_char(tch, ch, SKILL_LOOKING);
                            count++;
                        }
                    }
                }

                if (!count)
                {
                    send_to_char("Вы ничего не смогли разглядеть!\r\n", ch);
                }
                send_to_char("&Q&n", ch);
            }
        }
        else
        {
            if (!rdata->general_description.empty())
            {
                count += sprintf(buf + count, "%s\r\n", rdata->general_description.c_str());
            }
            else
            {
                count += sprintf(buf + count, "%s\r\n", world[rdata->to_room()]->name);
            }
            send_to_char(buf, ch);
            send_to_char("&R&q", ch);
            list_char_to_char(world[rdata->to_room()]->people, ch);
            send_to_char("&Q&n", ch);
        }
    }
    else if (info_is & EXIT_SHOW_WALL)
        send_to_char("И что вы там мечтаете увидеть?\r\n", ch);
}

void list_char_to_char_thing(const ROOM_DATA::people_t& list, CHAR_DATA* ch)
{
    for (const auto i : list)
    {
        if (ch != i)
        {
            if (GET_RACE(i) == NPC_RACE_THING)
            {
                list_one_char(i, ch, 0);
            }
        }
    }
}

void show_extend_room(const char * const description, CHAR_DATA * ch)
{
    int found = FALSE;
    char string[MAX_STRING_LENGTH], *pos;

    if (!description || !*description)
        return;

    strcpy(string, description);
    if ((pos = strchr(string, '<')))
        * pos = '\0';
    strcpy(buf, string);
    if (pos)
        *pos = '<';

    found = found || paste_description(string, TAG_WINTERNIGHT,
                                       (weather_info.season == SEASON_WINTER
                                        && (weather_info.sunlight == SUN_SET || weather_info.sunlight == SUN_DARK)));
    found = found || paste_description(string, TAG_WINTERDAY,
                                       (weather_info.season == SEASON_WINTER
                                        && (weather_info.sunlight == SUN_RISE || weather_info.sunlight == SUN_LIGHT)));
    found = found || paste_description(string, TAG_SPRINGNIGHT,
                                       (weather_info.season == SEASON_SPRING
                                        && (weather_info.sunlight == SUN_SET || weather_info.sunlight == SUN_DARK)));
    found = found || paste_description(string, TAG_SPRINGDAY,
                                       (weather_info.season == SEASON_SPRING
                                        && (weather_info.sunlight == SUN_RISE || weather_info.sunlight == SUN_LIGHT)));
    found = found || paste_description(string, TAG_SUMMERNIGHT,
                                       (weather_info.season == SEASON_SUMMER
                                        && (weather_info.sunlight == SUN_SET || weather_info.sunlight == SUN_DARK)));
    found = found || paste_description(string, TAG_SUMMERDAY,
                                       (weather_info.season == SEASON_SUMMER
                                        && (weather_info.sunlight == SUN_RISE || weather_info.sunlight == SUN_LIGHT)));
    found = found || paste_description(string, TAG_AUTUMNNIGHT,
                                       (weather_info.season == SEASON_AUTUMN
                                        && (weather_info.sunlight == SUN_SET || weather_info.sunlight == SUN_DARK)));
    found = found || paste_description(string, TAG_AUTUMNDAY,
                                       (weather_info.season == SEASON_AUTUMN
                                        && (weather_info.sunlight == SUN_RISE || weather_info.sunlight == SUN_LIGHT)));
    found = found || paste_description(string, TAG_NIGHT,
                                       (weather_info.sunlight == SUN_SET || weather_info.sunlight == SUN_DARK));
    found = found || paste_description(string, TAG_DAY,
                                       (weather_info.sunlight == SUN_RISE || weather_info.sunlight == SUN_LIGHT));

    // Trim any LF/CRLF at the end of description
    pos = buf + strlen(buf);
    while (pos > buf && *--pos == '\n')
    {
        *pos = '\0';
        if (pos > buf && *(pos - 1) == '\r')
            *--pos = '\0';
    }

    send_to_char(buf, ch);
    send_to_char("\r\n", ch);
}

void look_at_room(CHAR_DATA * ch, int ignore_brief)
{
    if (!ch->desc)
        return;

    if (IS_DARK(ch->in_room) && !CAN_SEE_IN_DARK(ch) && !can_use_feat(ch, DARK_READING_FEAT))
    {
        send_to_char("Слишком темно...\r\n", ch);
        show_glow_objs(ch);
        return;
    }
    else if (AFF_FLAGGED(ch, EAffectFlag::AFF_BLIND))
    {
        send_to_char("Вы все еще слепы...\r\n", ch);
        return;
    }
    else if (GET_POS(ch) < POS_SLEEPING)
    {
        return;
    }

    if (PRF_FLAGGED(ch, PRF_DRAW_MAP) && !PRF_FLAGGED(ch, PRF_BLIND))
    {
        MapSystem::print_map(ch);
    }
    else if (ch->desc->snoop_by
             && ch->desc->snoop_by->snoop_with_map
             && ch->desc->snoop_by->character)
    {
        ch->map_print_to_snooper(ch->desc->snoop_by->character.get());
    }

    send_to_char(CCICYN(ch, C_NRM), ch);

    if (!IS_NPC(ch) && PRF_FLAGGED(ch, PRF_ROOMFLAGS))
    {
        // иммам рандомная * во флагах ломает мапер грят
        const bool has_flag = ROOM_FLAGGED(ch->in_room, ROOM_BFS_MARK) ? true : false;
        GET_ROOM(ch->in_room)->unset_flag(ROOM_BFS_MARK);

        GET_ROOM(ch->in_room)->flags_sprint(buf, ";");
        sprintf(buf2, "[%5d] %s [%s]", GET_ROOM_VNUM(ch->in_room), world[ch->in_room]->name, buf);
        send_to_char(buf2, ch);

        if (has_flag)
        {
            GET_ROOM(ch->in_room)->set_flag(ROOM_BFS_MARK);
        }
    }
    else
    {
        if (PRF_FLAGGED(ch, PRF_MAPPER) && !PLR_FLAGGED(ch, PLR_SCRIPTWRITER) && !ROOM_FLAGGED(ch->in_room, ROOM_NOMAPPER))
        {
            sprintf(buf2, "%s [%d]", world[ch->in_room]->name, GET_ROOM_VNUM(ch->in_room));
            send_to_char(buf2, ch);
        }
        else
            send_to_char(world[ch->in_room]->name, ch);
    }

    send_to_char(CCNRM(ch, C_NRM), ch);
    send_to_char("\r\n", ch);

    if (IS_DARK(ch->in_room) && !PRF_FLAGGED(ch, PRF_HOLYLIGHT))
    {
        send_to_char("Слишком темно...\r\n", ch);
    }
    else if ((!IS_NPC(ch) && !PRF_FLAGGED(ch, PRF_BRIEF)) || ignore_brief || ROOM_FLAGGED(ch->in_room, ROOM_DEATH))
    {
        show_extend_room(RoomDescription::show_desc(world[ch->in_room]->description_num).c_str(), ch);
    }

    // autoexits
    if (!IS_NPC(ch) && PRF_FLAGGED(ch, PRF_AUTOEXIT) && !PLR_FLAGGED(ch, PLR_SCRIPTWRITER))
    {
        do_auto_exits(ch);
    }

    // Отображаем аффекты комнаты. После автовыходов чтобы не ломать популярный маппер.
    if (AFF_FLAGGED(ch, EAffectFlag::AFF_DETECT_MAGIC) || IS_IMMORTAL(ch))
    {
        show_room_affects(ch, room_aff_invis_bits, room_self_aff_invis_bits);
    }
    else
    {
        show_room_affects(ch, room_aff_visib_bits, room_aff_visib_bits);
    }

    // now list characters & objects
    if (world[ch->in_room]->fires)
    {
        sprintf(buf, "%sВ центре %s.%s\r\n",
                CCRED(ch, C_NRM), Fires[MIN(world[ch->in_room]->fires, MAX_FIRES - 1)], CCNRM(ch, C_NRM));
        send_to_char(buf, ch);
    }

    if (world[ch->in_room]->portal_time)
    {
        if (world[ch->in_room]->pkPenterUnique)
        {
            sprintf(buf, "%sЛазурная пентаграмма %sс кровавым отблеском%s ярко сверкает здесь.%s\r\n",
                    CCIBLU(ch, C_NRM), CCIRED(ch, C_NRM), CCIBLU(ch, C_NRM), CCNRM(ch, C_NRM));
        }
        else
        {
            sprintf(buf, "%sЛазурная пентаграмма ярко сверкает здесь.%s\r\n",
                    CCIBLU(ch, C_NRM), CCNRM(ch, C_NRM));
        }

        send_to_char(buf, ch);
    }

    if (world[ch->in_room]->holes)
    {
        const int ar = roundup(world[ch->in_room]->holes / HOLES_TIME);
        sprintf(buf, "%sЗдесь выкопана ямка глубиной примерно в %i аршин%s.%s\r\n",
                CCYEL(ch, C_NRM), ar, (ar == 1 ? "" : (ar < 5 ? "а" : "ов")), (CCNRM(ch, C_NRM)));
        send_to_char(buf, ch);
    }

    if (ch->in_room != NOWHERE && !ROOM_FLAGGED(ch->in_room, ROOM_NOWEATHER))
    {
        *buf = '\0';
        switch (real_sector(ch->in_room))
        {
            case SECT_FIELD_SNOW:
            case SECT_FOREST_SNOW:
            case SECT_HILLS_SNOW:
            case SECT_MOUNTAIN_SNOW:
                sprintf(buf, "%sСнежный ковер лежит у вас под ногами.%s\r\n",
                        CCWHT(ch, C_NRM), CCNRM(ch, C_NRM));
                break;
            case SECT_FIELD_RAIN:
            case SECT_FOREST_RAIN:
            case SECT_HILLS_RAIN:
                sprintf(buf, "%sВы просто увязаете в грязи.%s\r\n", CCIWHT(ch, C_NRM), CCNRM(ch, C_NRM));
                break;
            case SECT_THICK_ICE:
                sprintf(buf, "%sУ вас под ногами толстый лед.%s\r\n", CCIBLU(ch, C_NRM), CCNRM(ch, C_NRM));
                break;
            case SECT_NORMAL_ICE:
                sprintf(buf, "%sУ вас под ногами достаточно толстый лед.%s\r\n",
                        CCIBLU(ch, C_NRM), CCNRM(ch, C_NRM));
                break;
            case SECT_THIN_ICE:
                sprintf(buf, "%sТоненький ледок вот-вот проломится под вами.%s\r\n",
                        CCICYN(ch, C_NRM), CCNRM(ch, C_NRM));
                break;
        };
        if (*buf)
        {
            send_to_char(buf, ch);
        }
    }
    send_to_char("&Y&q", ch);
//  if (IS_SET(GET_SPELL_TYPE(ch, SPELL_TOWNPORTAL),SPELL_KNOW))
    if (ch->get_skill(SKILL_TOWNPORTAL)) {
        if (find_portal_by_vnum(GET_ROOM_VNUM(ch->in_room))) {
            send_to_char("Рунный камень с изображением пентаграммы немного выступает из земли.\r\n", ch);
        }
    }
    list_obj_to_char(world[ch->in_room]->contents, ch, 0, FALSE);
    list_char_to_char_thing(world[ch->in_room]->people, ch);  //добавим отдельный вызов если моб типа предмет выводим желтым
    send_to_char("&R&q", ch);
    list_char_to_char(world[ch->in_room]->people, ch);
    send_to_char("&Q&n", ch);

    // вход в новую зону
    if (!IS_NPC(ch)) {
        zone_rnum inroom = world[ch->in_room]->zone;
        if (zone_table[world[ch->get_from_room()]->zone].number != zone_table[inroom].number) {
            if (PRF_FLAGGED(ch, PRF_ENTER_ZONE))
                print_zone_info(ch);
            if ((ch->get_level() < LVL_IMMORT) && !ch->get_master())
                ++zone_table[inroom].traffic;
        }
    }
}

bool look_at_target(CHAR_DATA * ch, char *arg, int subcmd)
{
    int bits, found = FALSE, fnum, i = 0, cn = 0;
    struct portals_list_type *port;
    CHAR_DATA *found_char = NULL;
    OBJ_DATA *found_obj = NULL;
    struct char_portal_type *tmp;
    char *desc, *what, whatp[MAX_INPUT_LENGTH], where[MAX_INPUT_LENGTH];
    int where_bits = FIND_OBJ_INV | FIND_OBJ_ROOM | FIND_OBJ_EQUIP | FIND_CHAR_ROOM | FIND_OBJ_EXDESC;

    if (!ch->desc)
    {
        return false;
    }

    if (!*arg)
    {
        send_to_char("На что вы так мечтаете посмотреть?\r\n", ch);
        return false;
    }

    half_chop(arg, whatp, where);
    what = whatp;

    if (isname(where, "земля комната room ground"))
        where_bits = FIND_OBJ_ROOM | FIND_CHAR_ROOM;
    else if (isname(where, "инвентарь inventory"))
        where_bits = FIND_OBJ_INV;
    else if (isname(where, "экипировка equipment"))
        where_bits = FIND_OBJ_EQUIP;

    // для townportal
    if (isname(whatp, "камень") &&
        //       IS_SET(GET_SPELL_TYPE(ch, SPELL_TOWNPORTAL), SPELL_KNOW) &&
        ch->get_skill(SKILL_TOWNPORTAL) &&
        (port = get_portal(GET_ROOM_VNUM(ch->in_room), NULL)) != NULL && IS_SET(where_bits, FIND_OBJ_ROOM))
    {

        if (has_char_portal(ch, GET_ROOM_VNUM(ch->in_room)))
        {
            send_to_char("На камне огненными буквами написано слово '&R", ch);
            send_to_char(port->wrd, ch);
            send_to_char("&n'.\r\n", ch);
            return 0;
        } else if (GET_LEVEL(ch) < MAX(1, port->level - GET_REMORT(ch) / 2))
        {
            send_to_char("На камне что-то написано огненными буквами.\r\n", ch);
            send_to_char("Но вы еще недостаточно искусны, чтобы разобрать слово.\r\n", ch);
            return false;
        }
        else
        {
            for (tmp = GET_PORTALS(ch); tmp; tmp = tmp->next)
            {
                cn++;
            }
            if (cn >= MAX_PORTALS(ch))
            {
                send_to_char
                        ("Все доступные вам камни уже запомнены, удалите и попробуйте еще.\r\n", ch);
                return false;
            }
            send_to_char("На камне огненными буквами написано слово '&R", ch);
            send_to_char(port->wrd, ch);
            send_to_char("&n'.\r\n", ch);
            // теперь добавляем в память чара
            add_portal_to_char(ch, GET_ROOM_VNUM(ch->in_room));
            check_portals(ch);
            return false;
        }
    }

    // заглянуть в пентаграмму
    if (isname(whatp, "пентаграмма") && world[ch->in_room]->portal_time && IS_SET(where_bits, FIND_OBJ_ROOM))
    {
        const auto r = ch->in_room;
        const auto to_room = world[r]->portal_room;
        send_to_char("Приблизившись к пентаграмме, вы осторожно заглянули в нее.\r\n\r\n", ch);
        act("$n0 осторожно заглянул$g в пентаграмму.\r\n", TRUE, ch, 0, 0, TO_ROOM);
        if (world[to_room]->portal_time && (r == world[to_room]->portal_room))
        {
            send_to_char
                    ("Яркий свет, идущий с противоположного конца прохода, застилает вам глаза.\r\n\r\n", ch);
            return false;
        }
        ch->in_room = world[ch->in_room]->portal_room;
        look_at_room(ch, 1);
        ch->in_room = r;
        return false;
    }

    bits = generic_find(what, where_bits, ch, &found_char, &found_obj);
    // Is the target a character?
    if (found_char != NULL)
    {
        if (subcmd == SCMD_LOOK_HIDE && !check_moves(ch, LOOKHIDE_MOVES))
            return false;
        look_at_char(found_char, ch);
        if (ch != found_char)
        {
            if (subcmd == SCMD_LOOK_HIDE && ch->get_skill(SKILL_LOOK_HIDE) > 0)
            {
                fnum = number(1, skill_info[SKILL_LOOK_HIDE].max_percent);
                found =
                        train_skill(ch, SKILL_LOOK_HIDE,
                                    skill_info[SKILL_LOOK_HIDE].max_percent, found_char);
                if (!WAITLESS(ch))
                    WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
                if (found >= fnum && (fnum < 100 || IS_IMMORTAL(ch)) && !IS_IMMORTAL(found_char))
                    return false;
            }
            if (CAN_SEE(found_char, ch))
                act("$n оглядел$g вас с головы до пят.", TRUE, ch, 0, found_char, TO_VICT);
            act("$n посмотрел$g на $N3.", TRUE, ch, 0, found_char, TO_NOTVICT);
        }
        return false;
    }

    // Strip off "number." from 2.foo and friends.
    if (!(fnum = get_number(&what)))
    {
        send_to_char("Что осматриваем?\r\n", ch);
        return false;
    }

    // Does the argument match an extra desc in the room?
    if ((desc = find_exdesc(what, world[ch->in_room]->ex_description)) != NULL && ++i == fnum)
    {
        page_string(ch->desc, desc, FALSE);
        return false;
    }

    // If an object was found back in generic_find
    if (bits && (found_obj != NULL))
    {

        if (Clan::ChestShow(found_obj, ch))
        {
            return true;
        }
        if (ClanSystem::show_ingr_chest(found_obj, ch))
        {
            return true;
        }
        if (Depot::is_depot(found_obj))
        {
            Depot::show_depot(ch);
            return true;
        }

        // Собственно изменение. Вместо проверки "if (!found)" юзается проверка
        // наличия описания у объекта, найденного функцией "generic_find"
        if (!(desc = find_exdesc(what, found_obj->get_ex_description())))
        {
            show_obj_to_char(found_obj, ch, 5, TRUE, 1);	// Show no-description
        }
        else
        {
            send_to_char(desc, ch);
            show_obj_to_char(found_obj, ch, 6, TRUE, 1);	// Find hum, glow etc
        }

        *buf = '\0';
        obj_info(ch, found_obj, buf);
        send_to_char(buf, ch);
    }
    else
        send_to_char("Похоже, этого здесь нет!\r\n", ch);

    return false;
}

void do_auto_exits(CHAR_DATA * ch)
{
    int door, slen = 0;

    *buf = '\0';

    for (door = 0; door < NUM_OF_DIRS; door++)
    {
        // Наконец-то добавлена отрисовка в автовыходах закрытых дверей
        if (EXIT(ch, door) && EXIT(ch, door)->to_room() != NOWHERE)
        {
            if (EXIT_FLAGGED(EXIT(ch, door), EX_CLOSED))
            {
                slen += sprintf(buf + slen, "(%c) ", LOWER(*dirs[door]));
            }
            else if (!EXIT_FLAGGED(EXIT(ch, door), EX_HIDDEN))
            {
                if (world[EXIT(ch, door)->to_room()]->zone == world[ch->in_room]->zone)
                {
                    slen += sprintf(buf + slen, "%c ", LOWER(*dirs[door]));
                }
                else
                {
                    slen += sprintf(buf + slen, "%c ", UPPER(*dirs[door]));
                }
            }
        }
    }
    sprintf(buf2, "%s[ Exits: %s]%s\r\n", CCCYN(ch, C_NRM), *buf ? buf : "None! ", CCNRM(ch, C_NRM));

    send_to_char(buf2, ch);
}

void do_exits(CHAR_DATA *ch, char* /*argument*/, int/* cmd*/, int/* subcmd*/)
{
    int door;

    *buf = '\0';

    if (PRF_FLAGGED(ch, PRF_BLIND))
    {
        do_blind_exits(ch);
        return;
    }
    if (AFF_FLAGGED(ch, EAffectFlag::AFF_BLIND))
    {
        send_to_char("Вы слепы, как котенок!\r\n", ch);
        return;
    }
    for (door = 0; door < NUM_OF_DIRS; door++)
        if (EXIT(ch, door) && EXIT(ch, door)->to_room() != NOWHERE && !EXIT_FLAGGED(EXIT(ch, door), EX_CLOSED))
        {
            if (IS_GOD(ch))
                sprintf(buf2, "%-5s - [%5d] %s\r\n", Dirs[door],
                        GET_ROOM_VNUM(EXIT(ch, door)->to_room()), world[EXIT(ch, door)->to_room()]->name);
            else
            {
                sprintf(buf2, "%-5s - ", Dirs[door]);
                if (IS_DARK(EXIT(ch, door)->to_room()) && !CAN_SEE_IN_DARK(ch))
                    strcat(buf2, "слишком темно\r\n");
                else
                {
                    strcat(buf2, world[EXIT(ch, door)->to_room()]->name);
                    strcat(buf2, "\r\n");
                }
            }
            strcat(buf, CAP(buf2));
        }
    send_to_char("Видимые выходы:\r\n", ch);
    if (*buf)
        send_to_char(buf, ch);
    else
        send_to_char(" Замуровали, ДЕМОНЫ!\r\n", ch);
}

void do_blind_exits(CHAR_DATA *ch)
{
    int door;

    *buf = '\0';

    if (AFF_FLAGGED(ch, EAffectFlag::AFF_BLIND))
    {
        send_to_char("Вы слепы, как котенок!\r\n", ch);
        return;
    }
    for (door = 0; door < NUM_OF_DIRS; door++)
        if (EXIT(ch, door) && EXIT(ch, door)->to_room() != NOWHERE && !EXIT_FLAGGED(EXIT(ch, door), EX_CLOSED))
        {
            if (IS_GOD(ch))
                sprintf(buf2, "&W%-5s - [%5d] %s ", Dirs[door],
                        GET_ROOM_VNUM(EXIT(ch, door)->to_room()), world[EXIT(ch, door)->to_room()]->name);
            else
            {
                sprintf(buf2, "&W%-5s - ", Dirs[door]);
                if (IS_DARK(EXIT(ch, door)->to_room()) && !CAN_SEE_IN_DARK(ch))
                    strcat(buf2, "слишком темно");
                else
                {
                    strcat(buf2, world[EXIT(ch, door)->to_room()]->name);
                    strcat(buf2, "");
                }
            }
            strcat(buf, CAP(buf2));
        }
    send_to_char("Видимые выходы:\r\n", ch);
    if (*buf)
        send_to_char(ch, "%s&n\r\n", buf);
    else
        send_to_char("&W Замуровали, ДЕМОНЫ!&n\r\n", ch);
}

void do_look(CHAR_DATA *ch, char *argument, int/* cmd*/, int subcmd)
{
    char arg2[MAX_INPUT_LENGTH];
    int look_type;

    if (!ch->desc)
        return;

    if (GET_POS(ch) < POS_SLEEPING)
    {
        send_to_char("Виделся часто сон беспокойный...\r\n", ch);
    }
    else if (AFF_FLAGGED(ch, EAffectFlag::AFF_BLIND))
    {
        send_to_char("Вы ослеплены!\r\n", ch);
    }
    else if (is_dark(ch->in_room) && !CAN_SEE_IN_DARK(ch))
    {
        if (GET_LEVEL(ch) > 30)
        {
            sprintf(buf,
                    "%sКомната=%s%d %sСвет=%s%d %sОсвещ=%s%d %sКостер=%s%d %sЛед=%s%d "
                    "%sТьма=%s%d %sСолнце=%s%d %sНебо=%s%d %sЛуна=%s%d%s.\r\n",
                    CCNRM(ch, C_NRM), CCINRM(ch, C_NRM), ch->in_room,
                    CCRED(ch, C_NRM), CCIRED(ch, C_NRM), world[ch->in_room]->light,
                    CCGRN(ch, C_NRM), CCIGRN(ch, C_NRM), world[ch->in_room]->glight,
                    CCYEL(ch, C_NRM), CCIYEL(ch, C_NRM), world[ch->in_room]->fires,
                    CCYEL(ch, C_NRM), CCIYEL(ch, C_NRM), world[ch->in_room]->ices,
                    CCBLU(ch, C_NRM), CCIBLU(ch, C_NRM), world[ch->in_room]->gdark,
                    CCMAG(ch, C_NRM), CCICYN(ch, C_NRM), weather_info.sky,
                    CCWHT(ch, C_NRM), CCIWHT(ch, C_NRM), weather_info.sunlight,
                    CCYEL(ch, C_NRM), CCIYEL(ch, C_NRM), weather_info.moon_day, CCNRM(ch, C_NRM));
            send_to_char(buf, ch);
        }
        skip_hide_on_look(ch);

        send_to_char("Слишком темно...\r\n", ch);
        list_char_to_char(world[ch->in_room]->people, ch);	// glowing red eyes
        show_glow_objs(ch);
    }
    else
    {
        half_chop(argument, arg, arg2);

        skip_hide_on_look(ch);

        if (subcmd == SCMD_READ)
        {
            if (!*arg)
                send_to_char("Что вы хотите прочитать?\r\n", ch);
            else
                look_at_target(ch, arg, subcmd);
            return;
        }
        if (!*arg)	// "look" alone, without an argument at all
        {
            if (ch->desc)
            {
                ch->desc->msdp_report("ROOM");
            }
            look_at_room(ch, 1);
        }
        else if (is_abbrev(arg, "in") || is_abbrev(arg, "внутрь"))
            look_in_obj(ch, arg2);
            // did the char type 'look <direction>?'
        else if (((look_type = search_block(arg, dirs, FALSE)) >= 0) ||
                 ((look_type = search_block(arg, Dirs, FALSE)) >= 0))
            look_in_direction(ch, look_type, EXIT_SHOW_WALL);
        else if (is_abbrev(arg, "at") || is_abbrev(arg, "на"))
            look_at_target(ch, arg2, subcmd);
        else
            look_at_target(ch, argument, subcmd);
    }
}

void do_sides(CHAR_DATA *ch, char* /*argument*/, int/* cmd*/, int/* subcmd*/)
{
    int i;

    if (!ch->desc)
        return;

    if (GET_POS(ch) <= POS_SLEEPING)
        send_to_char("Виделся часто сон беспокойный...\r\n", ch);
    else if (AFF_FLAGGED(ch, EAffectFlag::AFF_BLIND))
        send_to_char("Вы ослеплены!\r\n", ch);
    else
    {
        skip_hide_on_look(ch);
        send_to_char("Вы посмотрели по сторонам.\r\n", ch);
        for (i = 0; i < NUM_OF_DIRS; i++)
        {
            look_in_direction(ch, i, 0);
        }
    }
}

void do_examine(CHAR_DATA *ch, char *argument, int/* cmd*/, int subcmd)
{
    CHAR_DATA *tmp_char;
    OBJ_DATA *tmp_object;
    char where[MAX_INPUT_LENGTH];
    int where_bits = FIND_OBJ_INV | FIND_OBJ_ROOM | FIND_OBJ_EQUIP | FIND_CHAR_ROOM | FIND_OBJ_EXDESC;


    if (GET_POS(ch) < POS_SLEEPING)
    {
        send_to_char("Виделся часто сон беспокойный...\r\n", ch);
        return;
    }
    else if (AFF_FLAGGED(ch, EAffectFlag::AFF_BLIND))
    {
        send_to_char("Вы ослеплены!\r\n", ch);
        return;
    }

    two_arguments(argument, arg, where);

    if (!*arg)
    {
        send_to_char("Что вы желаете осмотреть?\r\n", ch);
        return;
    }

    if (isname(where, "земля комната room ground"))
        where_bits = FIND_OBJ_ROOM | FIND_CHAR_ROOM;
    else if (isname(where, "инвентарь inventory"))
        where_bits = FIND_OBJ_INV;
    else if (isname(where, "экипировка equipment"))
        where_bits = FIND_OBJ_EQUIP;

    skip_hide_on_look(ch);

    if (look_at_target(ch, argument, subcmd))
        return;

    if (isname(arg, "пентаграмма") && world[ch->in_room]->portal_time && IS_SET(where_bits, FIND_OBJ_ROOM))
        return;

    if (isname(arg, "камень") &&
        ch->get_skill(SKILL_TOWNPORTAL) &&
        (get_portal(GET_ROOM_VNUM(ch->in_room), NULL)) != NULL && IS_SET(where_bits, FIND_OBJ_ROOM))
        return;

    generic_find(arg, where_bits, ch, &tmp_char, &tmp_object);
    if (tmp_object)
    {
        if (GET_OBJ_TYPE(tmp_object) == OBJ_DATA::ITEM_DRINKCON
            || GET_OBJ_TYPE(tmp_object) == OBJ_DATA::ITEM_FOUNTAIN
            || GET_OBJ_TYPE(tmp_object) == OBJ_DATA::ITEM_CONTAINER)
        {
            look_in_obj(ch, argument);
        }
    }
}

void do_diagnose(CHAR_DATA *ch, char *argument, int/* cmd*/, int/* subcmd*/)
{
    CHAR_DATA *vict;

    one_argument(argument, buf);

    if (*buf)
    {
        if (!(vict = get_char_vis(ch, buf, FIND_CHAR_ROOM)))
            send_to_char(NOPERSON, ch);
        else
            diag_char_to_char(vict, ch);
    }
    else
    {
        if (ch->get_fighting())
            diag_char_to_char(ch->get_fighting(), ch);
        else
            send_to_char("На кого вы хотите взглянуть?\r\n", ch);
    }
}

void do_zone(CHAR_DATA *ch, char* /*argument*/, int/* cmd*/, int/* subcmd*/)
{
    if (ch->desc
        && !(IS_DARK(ch->in_room) && !CAN_SEE_IN_DARK(ch) && !can_use_feat(ch, DARK_READING_FEAT))
        && !AFF_FLAGGED(ch, EAffectFlag::AFF_BLIND))
    {
        MapSystem::print_map(ch);
    }

    print_zone_info(ch);

    if ((IS_IMMORTAL(ch) || PRF_FLAGGED(ch, PRF_CODERINFO))
        && zone_table[world[ch->in_room]->zone].comment)
    {
        send_to_char(ch, "Комментарий: %s.\r\n",
                     zone_table[world[ch->in_room]->zone].comment);
    }
}

void do_inventory(CHAR_DATA *ch, char* /*argument*/, int/* cmd*/, int/* subcmd*/)
{
    send_to_char("Вы несете:\r\n", ch);
    list_obj_to_char(ch->carrying, ch, 1, 2);
}

void do_equipment(CHAR_DATA *ch, char *argument, int/* cmd*/, int/* subcmd*/)
{
    int i, found = 0;
    skip_spaces(&argument);

    send_to_char("На вас надето:\r\n", ch);
    for (i = 0; i < NUM_WEARS; i++)
    {
        if (GET_EQ(ch, i)){
            if (CAN_SEE_OBJ(ch, GET_EQ(ch, i)))	{
                send_to_char(where[i], ch);
                show_obj_to_char(GET_EQ(ch, i), ch, 1, TRUE, 1);
                found = TRUE;
            }
            else {
                send_to_char(where[i], ch);
                send_to_char("что-то.\r\n", ch);
                found = TRUE;
            }
        }
        else {
            if (is_abbrev(argument, "все") || is_abbrev(argument, "all")) {
                if (GET_EQ(ch, WEAR_BOTHS))
                    if ((i==WEAR_WIELD) || (i==WEAR_HOLD))
                        continue;
                if ((i==WEAR_QUIVER)&&(GET_EQ(ch, WEAR_BOTHS)))
                {
                    if (!(((GET_OBJ_TYPE(GET_EQ(ch, WEAR_BOTHS))) == OBJ_DATA::ITEM_WEAPON) && (GET_OBJ_SKILL(GET_EQ(ch, WEAR_BOTHS)) == SKILL_BOWS )))
                        continue;
                }
                else if (i==19)
                    continue;
                if (GET_EQ(ch, WEAR_WIELD) || GET_EQ(ch, WEAR_HOLD))
                    if (i==18)
                        continue;
                if (GET_EQ(ch, WEAR_SHIELD))
                {
                    if ((i==WEAR_HOLD) || (i==WEAR_BOTHS))
                        continue;
                }
                send_to_char(where[i], ch);
                sprintf(buf, "%s[ Ничего ]%s\r\n", CCINRM(ch, C_NRM), CCNRM(ch, C_NRM));
                send_to_char(buf, ch);
                found = TRUE;
            }
        }
    }
    if (!found)
    {
        if (IS_FEMALE(ch))
            send_to_char("Костюм Евы вам очень идет :)\r\n", ch);
        else
            send_to_char(" Вы голы, аки сокол.\r\n", ch);
    }
}

/*
 * \return флаг если смотрим в клан-сундук, чтобы после осмотра не смотреть второй раз по look_in_obj
 */

// vim: ts=4 sw=4 tw=0 noet syntax=cpp :
