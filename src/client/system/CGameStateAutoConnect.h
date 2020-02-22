#pragma once

#include "CGameState.h"

enum class AutoConnectError {
	None,
	ServerError,
	LoginError,
};

/// Game state that handles automatic login, server/channel and avatar select
class CGameStateAutoConnect : public CGameState
{
public:
	CGameStateAutoConnect(int state);

	// -- CGameState
	int Update(bool bLostFocus) override;
	int Enter(int previousState) override;
	int Leave(int nextState) override;
	int ProcMouseInput(UINT uiMsg, WPARAM wParam, LPARAM lParam) override;
	int ProcKeyboardInput(UINT uiMsg, WPARAM wParam, LPARAM lParam) override;
	// --

	// Handlers
	/*
	void OnLoginServerConnected();
	void OnLoginSucceeded(const std::vector<ServerList::ServerInfo>& serverList);
	void OnLoginFailed(int errorCode);

	void OnCharacterServerConnected();
	void OnCharacterServerConnectFailed();
	*/

private:
	std::vector<std::string> messages;
	AutoConnectError last_error;
};