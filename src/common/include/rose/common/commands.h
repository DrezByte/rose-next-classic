#pragma once

#include <unordered_map>

enum class CommandContext {
	Client,
	Server
};

struct CommandInfo {
	const char* name;
	const char* description;
	const char* help;
	CommandContext context;
	// TODO: Command level
};

const std::unordered_map<const char*, CommandInfo> commands = {
	{"maps", CommandInfo{"maps", "List all maps by id", "Usage: maps", CommandContext::Server}},
};