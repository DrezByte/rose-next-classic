#pragma once

#include <unordered_map>

namespace Rose::Common {

enum class CommandContext { Client, Server };

struct CommandInfo {
    const char* name;
    int level;
    const char* description;
    const char* usage;
    CommandContext context;
};

namespace Command {
enum CommandId {
    HELP,

    DAYTIME,
    LEVELUP,
    MAPS,
    RATES,
    STATS,
    TELEPORT,

    RELOAD_CONFIG,
};
}

const char* HELP_HELP = "Display this help.";
const char* HELP_USAGE = "Usage: help";

const char* DAYTIME_HELP = "Set time of day.";
const char* DAYTIME_USAGE = "Usage: daytime <morning|night>";

const char* LEVELUP_HELP = "Level up by the given amount.";
const char* LEVELUP_USAGE = "Usage: levelup <amount>";

const char* MAPS_HELP = "List all maps by id.";
const char* MAPS_USAGE = "Usage: maps";

const char* RATES_HELP = "List server rates.";
const char* RATES_USAGE = "Usage: rates";

const char* STATS_HELP = "List server side character stats.";
const char* STATS_USAGE = "Usage: stats [target_name]";

const char* TP_HELP = "Teleport to a location.";
const char* TP_USAGE = "Usage: tp <map_id> [x_coord] [y_coord]";

const char* RELOADCONFIG_HELP = "Reload server game configs.";
const char* RELOADCONFIG_USAGE = "Usage: reloadconfig";


static const std::vector<CommandInfo> commands = {
    {"help", 1, HELP_HELP, HELP_USAGE, CommandContext::Client},

    {"daytime", 100, DAYTIME_HELP, DAYTIME_USAGE, CommandContext::Server},
    {"levelup", 100, LEVELUP_HELP, LEVELUP_USAGE, CommandContext::Server},
    {"maps", 100, MAPS_HELP, MAPS_USAGE, CommandContext::Server},
    {"rates", 100, RATES_HELP, RATES_USAGE, CommandContext::Server},
    {"stats", 100, STATS_HELP, STATS_USAGE, CommandContext::Server},
    {"tp", 100, TP_HELP, TP_USAGE, CommandContext::Server},

    {"reloadconfig", 500, RELOADCONFIG_HELP, RELOADCONFIG_USAGE, CommandContext::Server},
};

} // namespace Rose::Common