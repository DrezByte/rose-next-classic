#ifndef _GAME_STATE_MOVE_PLANET_
#define _GAME_STATE_MOVE_PLANET_

#include "cgamestate.h"

enum enumPLANET {
    PLANET_JUNON = 1,
    PLANET_LUNAR = 2,
};

/**
 * �༺�� �̵� ������ ���� State Class
 *
 * @Author	������
 * @Date		2005/9/15
 */
class CGameStateMovePlanet: public CGameState {
public:
    static int m_iTargetPlanet;

public:
    CGameStateMovePlanet(int iStateID);

    CGameStateMovePlanet(void);
    ~CGameStateMovePlanet(void);

    int Update(bool bLostFocus) override;
    int Enter(int iPrevStateID) override;
    int Leave(int iNextStateID) override;

    int ProcMouseInput(UINT uiMsg, WPARAM wParam, LPARAM lParam) override;
    int ProcKeyboardInput(UINT uiMsg, WPARAM wParam, LPARAM lParam) override;
};

#endif //_GAME_STATE_MOVE_PLANET_