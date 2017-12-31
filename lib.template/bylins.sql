-- Данный SQL скрипт описывает структуру базы данных для хранения информации об аккаунтах. 

create table "accounts"
(
       "id" integer primary key autoincrement,
       "email" text not null,
       "password" text not null,
       "failed_attempts" integer not null,                -- теукщее количество неудачных попыток входа с момента последнего успешного входа
       "confirmed" bool default false
);

create unique index "accounts_email" on "accounts"("email");

create table "players_ownership"
(
       "id" integer primary key autoincrement,
       "account_id" integer,
       "player_uid" integer not null,
       foreign key ("account_id") references "accounts"("id") on delete cascade on update cascade
);

/*
create table "players"
(
       "id" integer primary key autoincrement,
       "name" text not null,
       "level" integer not null,
       "class" integer not null,
       "last_login" timestamp,
       "last_host" text,
       "player_id" integer not null,
       "experience" integer not null,
       "flags" integer not null,
       "case_i" text not null,                            -- именительный падеж
       "case_r" text not null,                            -- родительный падеж
       "case_d" text not null,                            -- дательный падеж
       "case_v" text not null,                            -- винительный падеж
       "case_t" text not null,                            -- творительный падеж
       "case_p" text not null,                            -- предложный падеж
       "description" text,
       "gender" integer not null,
       "kin" integer,                                     -- ??? я хз что это такое ???
       "birth" timestamp,
       "played_time" integer,                             -- в секундах ???
       "height" integer not null,
       "weight" integer not null,
       "size" integer not null,
       "align" integer not null,
       "affects" integer not null,                        -- битовая маска аффектов игрока
       
       "strenght" integer not null,
       "intelligence" integer not null,
       "wisdom" integer not null,
       "dexterity" integer not null,
       "constitution" integer not null,
       "charisma" integer not null,
       
       "hit_roll" integer not null,                       -- ???
       "damage_roll" integer not null,                    -- ???
       "ac" integer not null,
       "current_health" integer not null,
       "maximal_health" integer not null,
       "current_mana" integer not null,
       "maximal_mana" integer not null,
       "current_move" integer not null,
       "maximal_move" integer not null,
       "gold" integer not null,
       "bank" integer not null,
       "wimp_level" integer not null,                     -- ???
       "freeze_level" integer not null,                   -- ???
       "invisible_level" integer not null,                -- уровень невидимости игрока
       "load_room" integer not null,
       
       "start_strenght" integer not null,
       "start_intelligence" integer not null,
       "start_wisdom" integer not null,
       "start_dexterity" integer not null,
       "start_constitution" integer not null,
       "start_charisma" integer not null,
       
       "" integer not null,
       "" integer not null,
       "" integer not null,
);

create table "features"
-- отображение с player_id на фиты (я хз что это значит) с таймером
(
       "id" integer primary key autoincrement,
       "player_id" integer not null,                      -- need index ???
       "feature_id" integer not null,
       foreign key ("player_id") references "players"("id") on delete cascade on update cascade
);

create table "timed_features"
-- отображение с player_id на фиты (я хз что это значит) с таймером
(
       "id" integer primary key autoincrement,
       "player_id" integer not null,                      -- need index ???
       "feature_id" integer not null,
       "timer" integer not null,
       foreign key ("player_id") references "players"("id") on delete cascade on update cascade
);

create table "skills"
-- отображение с player_id на умения.
-- Эта же таблица хранит уровень прокачки умения
(
       "id" integer primary key autoincrement,
       "player_id" integer not null,                      -- need index ???
       "skill_id" integer not null,                       -- см. в коде перечисление ESkill
       "percents" integer not null,                       -- уровень прокачки умения
       foreign key ("player_id") references "players"("id") on delete cascade on update cascade
);

create table "timed_skills"
-- отображение с player_id на умения с таймером
(
       "id" integer primary key autoincrement,
       "player_id" integer not null,                      -- need index ???
       "skill_id" integer not null,
       "timer" integer not null,
       foreign key ("player_id") references "players"("id") on delete cascade on update cascade
);

create table "spells"
-- отображение с player_id на список известных игроку заклинаний
(
       "id" integer primary key autoincrement,
       "player_id" integer not null,                      -- need index ???
       "spell_id" integer not null,
       foreign key ("player_id") references "players"("id") on delete cascade on update cascade
);

create table "memorized_spells"
-- отображение с player_id на список запомненных игроком заклинаний:
-- пара (слот, заклинание)
(
       "id" integer primary key autoincrement,
       "player_id" integer not null,                      -- need index ???
       "slot", integer not null,
       "spell_id" integer not null,
       foreign key ("player_id") references "players"("id") on delete cascade on update cascade
);

create table "recipes"
-- отображение с player_id на список известных игроку рецептов.
-- Эта же таблица хранит уровень прокачки того или иного рецепта.
(
       "id" integer primary key autoincrement,
       "player_id" integer not null,                      -- need index ???
       "recipe_id", integer not null, 
       "percents" integer not null,
       foreign key ("player_id") references "players"("id") on delete cascade on update cascade
);

create table "boards_time"
-- отображение с player_id на время последнего чтения каждой из досок: на пару (доска, время)
(
       "id" integer primary key autoincrement,
       "player_id" integer not null,                      -- need index ???
       "board_id", integer not null, 
       "timestamp" timestamp not null,
       foreign key ("player_id") references "players"("id") on delete cascade on update cascade
)
*/