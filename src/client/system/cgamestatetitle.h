#pragma once

#include "cgamestate.h"

/**
* Ŭ���̾�Ʈ ������ �α��� ���� ����, Ÿ��Ʋ�� �����ָ� ����Ÿ�� �ε��ϴ� State Class
*
* @Author	������
* @Date		2005/9/15
*/
class CGameStateTitle :	public CGameState
{
public:


public:
	CGameStateTitle(int iID);
	~CGameStateTitle(void);
	
	CGameStateTitle() = delete;
	CGameStateTitle(CGameStateTitle&) = delete;
	CGameStateTitle(CGameStateTitle&&) = delete;
	CGameStateTitle& CGameStateTitle::operator= (const CGameStateTitle&) = delete;
	CGameStateTitle& CGameStateTitle::operator= (CGameStateTitle&&) = delete;

	// -- CGameState
	int Update( bool bLostFocus ) override;
	int Enter( int iPrevStateID ) override;
	int Leave( int iNextStateID ) override;
	int ProcMouseInput(UINT uiMsg, WPARAM wParam, LPARAM lParam) override { return 0; }
	int ProcKeyboardInput(UINT uiMsg, WPARAM wParam, LPARAM lParam) override { return 0; }
	// --

	// Handlers
	void ServerDisconnected(){}

protected:
	void Draw();

private:
	std::atomic<bool> data_loaded;
	std::thread data_thread;

	HNODE title_texture;
	int background_zone_id;

private:
	void load_data();
};
