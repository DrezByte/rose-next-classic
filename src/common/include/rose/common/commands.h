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
    TELEPORT,
};
}

static const std::vector<CommandInfo> commands = {
    {"help", 1, "Display this help.", "Usage: help", CommandContext::Client},

    {"daytime", 100, "Set time of day.", "Usage: daytime <morning|night>", CommandContext::Server},
    {"levelup",
        100,
        "Level up by the given amount.",
        "Usage: levelup <amount>",
        CommandContext::Server},
    {"maps", 100, "List all maps by id.", "Usage: maps", CommandContext::Server},
    {"rates", 100, "List server rates.", "Usage: rates", CommandContext::Server},
    {"tp",
        100,
        "Teleport to a location.",
        " Usage: tp <map_id> [x_coord] [y_coord]",
        CommandContext::Server},
};

} // namespace Rose::Common