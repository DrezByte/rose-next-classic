CREATE TABLE account
(
    id integer PRIMARY KEY GENERATED BY DEFAULT AS IDENTITY,
    email varchar(30) NOT NULL UNIQUE,
    password char(64) NOT NULL,
    salt char(16) NOT NULL,
    access_level INTEGER NOT NULL DEFAULT 0,
    remember_token varchar(100) NULL,
    created TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE character
(
    id integer PRIMARY KEY GENERATED BY DEFAULT AS IDENTITY,
    account_email varchar(30) NOT NULL,
    -- General
    name varchar(30) NOT NULL UNIQUE,
    gender_id smallint NOT NULL DEFAULT 0,
    job_id smallint NOT NULL DEFAULT 0,
    face_id integer NOT NULL DEFAULT 1,
    hair_id integer NOT NULL DEFAULT 1,
    -- Stats
    level smallint NOT NULL DEFAULT 1,
    exp int NOT NULL DEFAULT 0,
    hp int NOT NULL DEFAULT 50,
    mp int NOT NULL DEFAULT 40,
    stamina int NOT NULL DEFAULT 5000,
    max_hp int NOT NULL DEFAULT 50,
    max_mp int NOT NULL DEFAULT 40,
    max_stamina int NOT NULL DEFAULT 5000,
    str smallint NOT NULL DEFAULT 15,
    dex smallint NOT NULL DEFAULT 15,
    intt smallint NOT NULL DEFAULT 15,
    con smallint NOT NULL DEFAULT 15,
    cha smallint NOT NULL DEFAULT 10,
    sen smallint NOT NULL DEFAULT 10,
    stat_points smallint NOT NULL DEFAULT 0,
    skill_points smallint NOT NULL DEFAULT 0,
    -- Economy
    money bigint NOT NULL DEFAULT 0,
    storage_money bigint NOT NULL DEFAULT 0,
    -- World
    map_id smallint NOT NULL DEFAULT(22),
    respawn_x real NOT NULL DEFAULT(577987.99),
    respawn_y real NOT NULL DEFAULT(515579.9805),
    town_respawn_id smallint NOT NULL DEFAULT(22),
    town_respawn_x real NOT NULL DEFAULT(577987.99),
    town_respawn_y real NOT NULL DEFAULT(515579.9805),
    -- Other
    union_id smallint NOT NULL DEFAULT 0,
    skills jsonb NOT NULL DEFAULT '[11, 12, 16, 19, 20, 21]'::jsonb,
    quests jsonb NOT NULL DEFAULT '{}'::jsonb,
    hotbar jsonb NOT NULL DEFAULT '[]'::jsonb,
    wishlist jsonb NOT NULL DEFAULT '[]'::jsonb,
    -- Meta
    created TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP,
    delete_by timestamptz,
    CONSTRAINT character_level_positive CHECK (level > 0),
    CONSTRAINT character_money_positive CHECK (money >= 0),
    CONSTRAINT character_storage_money_positive CHECK (storage_money >= 0)
);
CREATE INDEX character_account_email_index ON character (account_email);

CREATE TABLE clan
(
    id integer PRIMARY KEY GENERATED BY DEFAULT AS IDENTITY,
    name varchar(30) NOT NULL UNIQUE,
    description varchar(255) NULL,
    motd varchar(1024) NULL,
    level smallint NOT NULL DEFAULT 1,
    points bigint NOT NULL DEFAULT 0,
    money bigint NOT NULL DEFAULT 0,
    marker_front smallint NOT NULL DEFAULT 0,
    marker_back smallint NOT NULL DEFAULT 0,
    marker_crc smallint NULL,
    created TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE clan_member
(
    id integer PRIMARY KEY GENERATED BY DEFAULT AS IDENTITY,
    character_id integer NOT NULL REFERENCES character (id) ON DELETE CASCADE,
    clan_id integer NOT NULL REFERENCES clan (id) ON DELETE CASCADE,
    rank smallint NOT NULL DEFAULT 1,
    points bigint NOT NULL DEFAULT 0,
    joined TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE item
(
    id integer PRIMARY KEY GENERATED BY DEFAULT AS IDENTITY,
    uuid uuid NOT NULL UNIQUE,
    game_data_id integer NOT NULL DEFAULT 0,
    type_id integer NOT NULL DEFAULT 0,
    stat_id integer NOT NULL DEFAULT 0,
    grade integer NOT NULL DEFAULT 0,
    durability integer NOT NULL DEFAULT 10,
    lifespan integer NOT NULL DEFAULT 1000,
    appraisal boolean NOT NULL DEFAULT false,
    socket boolean NOT NULL DEFAULT false,
    crafted boolean NOT NULL DEFAULT false,
    CONSTRAINT grade_positive CHECK (grade >= 0)
);

CREATE TABLE inventory
(
    id integer PRIMARY KEY GENERATED BY DEFAULT AS IDENTITY,
    owner_id integer NOT NULL REFERENCES character (id) ON DELETE CASCADE,
    item_id integer NOT NULL REFERENCES item (id) ON DELETE CASCADE,
    slot smallint NOT NULL,
    quantity smallint NOT NULL DEFAULT 1,
    UNIQUE (owner_id, slot),
    UNIQUE (owner_id, slot, item_id),
    CONSTRAINT inventory_slot_positive CHECK (slot >= 0),
    CONSTRAINT inventory_quantity_positive CHECK (quantity >= 0)
);

CREATE TABLE storage
(
    id integer PRIMARY KEY GENERATED BY DEFAULT AS IDENTITY,
    owner_id integer NOT NULL REFERENCES character (id) ON DELETE CASCADE,
    item_id integer NOT NULL REFERENCES item (id) ON DELETE CASCADE,
    slot smallint NOT NULL,
    quantity smallint NOT NULL DEFAULT 1,
    UNIQUE (owner_id, slot),
    UNIQUE (owner_id, slot, item_id),
    CONSTRAINT storage_slot_positive CHECK (slot >= 0),
    CONSTRAINT storage_quantity_positive CHECK (quantity >= 0)
);

CREATE TABLE friends
(
    id integer PRIMARY KEY GENERATED BY DEFAULT AS IDENTITY,
    character_id integer NOT NULL REFERENCES character (id) ON DELETE CASCADE,
    friend_id integer NOT NULL REFERENCES character (id) ON DELETE CASCADE,
    UNIQUE (character_id, friend_id)
);

CREATE TABLE mail
(
    id integer PRIMARY KEY GENERATED BY DEFAULT AS IDENTITY,
    sent timestamptz DEFAULT CURRENT_TIMESTAMP,
    sender_id integer NOT NULL REFERENCES character (id) ON DELETE CASCADE,
    recipient_id integer NOT NULL REFERENCES character (id) ON DELETE CASCADE,
    message varchar(255) NOT NULL
);

CREATE TABLE union_points
(
    id integer PRIMARY KEY GENERATED BY DEFAULT AS IDENTITY,
    character_id integer NOT NULL REFERENCES character (id) ON DELETE CASCADE,
    union1 integer NOT NULL DEFAULT 0,
    union2 integer NOT NULL DEFAULT 0,
    union3 integer NOT NULL DEFAULT 0,
    union4 integer NOT NULL DEFAULT 0,
    union5 integer NOT NULL DEFAULT 0,
    union6 integer NOT NULL DEFAULT 0,
    union7 integer NOT NULL DEFAULT 0,
    union8 integer NOT NULL DEFAULT 0,
    union9 integer NOT NULL DEFAULT 0,
    union10 integer NOT NULL DEFAULT 0
);

CREATE TABLE worldvar
(
    id integer PRIMARY KEY GENERATED BY DEFAULT AS IDENTITY,
    name varchar(50) NOT NULL UNIQUE,
    data jsonb NOT NULL,
    modified TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP
);