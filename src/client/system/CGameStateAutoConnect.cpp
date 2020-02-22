#include "stdafx.h"

#include "cgame.h"
#include "CGameStateAutoConnect.h"

#include "Network/cnetwork.h"

CGameStateAutoConnect::CGameStateAutoConnect(int state) :
	last_error(AutoConnectError::None)
{
	this->m_iStateID = state;
}

int
CGameStateAutoConnect::Update(bool bLostFocus)
{
	updateScene();
	if (!beginScene()) {
		return 0;
	}

	clearScreen();

	int messageY = 15;
	for (const std::string& message : this->messages) {
		drawFont(g_GameDATA.m_hFONT[FONT_LARGE_BOLD], false, 15, messageY, zz_color_black, message.c_str());
		messageY += 15;
	}

	if (this->last_error != AutoConnectError::None) {
		drawFont(g_GameDATA.m_hFONT[FONT_LARGE_BOLD], false, 15, messageY, zz_color_black, "Press any key to continue...");
	}

	endScene();
	swapBuffers();

	return 0;
}

int
CGameStateAutoConnect::Enter(int previousState)
{
	this->messages.clear();
	this->messages.push_back("Started auto connect.");

	if (!g_pNet->ConnectToServer(g_GameDATA.server_ip, g_GameDATA.server_port, NS_CON_TO_LSV)) {
		this->last_error = AutoConnectError::ServerError;
	}

	return 0;
}

int
CGameStateAutoConnect::Leave(int nextState)
{
	return 0;
}

int
CGameStateAutoConnect::ProcMouseInput(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

int
CGameStateAutoConnect::ProcKeyboardInput(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch (this->last_error) {
		case AutoConnectError::LoginError:
			CGame::GetInstance().ChangeState(CGame::GS_LOGIN);
			break;
		case AutoConnectError::ServerError:
			g_pCApp->SetExitGame();
			break;
		};

	return 0;
}

/*
void
CGameStateAutoConnect::OnLoginServerConnected()
{
	_messages.push_back("Connected to login server.");
	g_pNet->Send_cli_LOGIN_REQ(g_GameDATA.userAccount.c_str(), g_GameDATA.userPassword.c_str(), true);
}

void
CGameStateAutoConnect::OnLoginSucceeded(const std::vector<ServerList::ServerInfo>& serverList)
{
	int serverId = 1;
	int channelId = 1;

	if (g_GameDATA.autoConnectServerId > 1) {
		serverId = g_GameDATA.autoConnectServerId;
	}

	if (g_GameDATA.autoConnectChannelId > 1) {
		channelId = g_GameDATA.autoConnectChannelId;
	}

	std::stringstream message("");
	message << "Login succeeded. Connecting: Server #" << serverId << "; Channel #" << channelId << ".";
	_messages.push_back(message.str());

	g_pNet->Send_cli_SELECT_SERVER(serverId, channelId);

	return;
}

void
CGameStateAutoConnect::OnLoginFailed(int errorCode)
{
	if (errorCode == RESULT_LOGIN_REPLY_ALREADY_LOGGEDIN) {
		_messages.push_back("Account already logged in. Attempting to reconnect.");

		if (!g_pNet->ConnectToServer(g_GameDATA.serverIp.c_str(), g_GameDATA.serverPort, NS_CON_TO_LSV)) {
			_lastError = AutoConnectError::ServerError;
			return;
		}

		g_pNet->Send_cli_LOGIN_REQ(g_GameDATA.userAccount.c_str(), g_GameDATA.userPassword.c_str(), true);
		return;
	}

}

void
CGameStateAutoConnect::OnCharacterServerConnected()
{
	int characterIndex = 0;

	if (g_GameDATA.autoConnectCharacterIndex > 0) {
		characterIndex = g_GameDATA.autoConnectCharacterIndex;
	}

	std::stringstream message("");
	message << "Server/Channel select succeeded. Selecting character #" << characterIndex + 1 << ".";
	_messages.push_back(message.str());

	CGame::GetInstance().Load_BasicDATA2();
	g_pNet->Send_cli_SELECT_CHAR(characterIndex);
}

void
CGameStateAutoConnect::OnCharacterServerConnectFailed()
{
	_messages.push_back("Failed to connect to server/channel.");
	CGame::GetInstance().ChangeState(CGame::GS_LOGIN);
}
*/