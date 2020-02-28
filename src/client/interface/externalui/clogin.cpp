#include "stdafx.h"

#include "CLogin.h"

#include "CExternalUIManager.h"
#include "ExternalUILobby.h"

#include "../Command/CTCmdHotExec.h"

#include "../../Game.h"
#include "../../Network/CNetwork.h"
#include "Net_Prototype.h"
#include "../../System/CGame.h"
#include "../../Util/Localizing.h"
#include "../../CClientStorage.h"

#include "tgamectrl/teditbox.h"
#include "tgamectrl/tmsgbox.h"
#include "tgamectrl/tcheckbox.h"
#include "tgamectrl/jcombobox.h"
#include "tgamectrl/jlistbox.h"
#include "tgamectrl/tcontainer.h"
#include "tgamectrl/tlistcellrenderer.h"
#include "tgamectrl/tcontaineritem.h"

#include "../util/clipboardutil.h"

using namespace Rose;

///
/// Construct
///
CLogin::CLogin() {
    memset(m_strID, 0, sizeof(m_strID));
    memset(m_strPassword, 0, sizeof(m_strPassword));

    m_pEUIManager = NULL;

    m_hUserGrade = 0; ///사용자 등급
}

///
/// Destruct
///

CLogin::~CLogin() {}

///
/// set observer class
///
void
CLogin::SetEUIObserver(CExternalUIManager* pObserver) {
    m_pEUIManager = pObserver;
}

///
/// overrided function
///
bool
CLogin::Create(const char* szIDD) {
    CTDialog::Create(szIDD);
    if (CGame::GetInstance().IsActiveRouteComboBox()) {
        CWinCtrl* pComponent = Find(IID_COMBOBOX_ROUTE);
        if (pComponent && pComponent->GetControlType() == CTRL_JCOMBOBOX) {
            CJComboBox* pCombo = (CJComboBox*)pComponent;
            CJListBox* pJListBox = pCombo->GetListBoxPtr();
            if (pJListBox) {
                pJListBox->SetContainer(new CTContainer);
                pJListBox->SetCellRenderer(new CTListCellRenderer);
            }

            CTContainerItem* pItem = NULL;
            pItem = new CTContainerItem;
            pItem->SetIdentify(" FAITH");
            pItem->SetValue("");
            pCombo->AddItem(pItem);

            pItem = new CTContainerItem;
            pItem->SetIdentify(" BB Games");
            pItem->SetValue("@BB");
            pCombo->AddItem(pItem);

            // pItem = new CTContainerItem;
            // pItem->SetIdentify(" Excite");
            // pItem->SetValue("@EX");
            // pCombo->AddItem( pItem );

            // pItem = new CTContainerItem;
            // pItem->SetIdentify(" BIGLOBE");
            // pItem->SetValue("@BG");
            // pCombo->AddItem( pItem );

            int index = g_ClientStorage.GetJapanRoute();
            pCombo->SelectItem(index);
        }
    }

    return true;
}

UINT
CLogin::Process(UINT uiMsg, WPARAM wParam, LPARAM lParam) {
    UINT Ret = 0;
    if (!IsVision())
        return Ret;

    CWinCtrl* pCtrl = NULL;
    CTEditBox* pEditBox = NULL;

    if (uiMsg == WM_KEYDOWN) {
        switch (wParam) {
            case VK_TAB: {
                CTEditBox* pEditBoxID = NULL;
                CTEditBox* pEditBoxPwd = NULL;
                pCtrl = Find(IID_EDIT_ID);
                if (pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX)
                    pEditBoxID = (CTEditBox*)pCtrl;

                pCtrl = Find(IID_EDIT_PWD);
                if (pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX)
                    pEditBoxPwd = (CTEditBox*)pCtrl;

                if (pEditBoxID && pEditBoxPwd) {
                    if (pEditBoxID->IsFocus())
                        pEditBoxPwd->SetFocus(true);
                    else
                        pEditBoxID->SetFocus(true);
                }
                return uiMsg;
                break;
            }
            case VK_RETURN: {
                ConnectLoginServer();
                return uiMsg;
                break;
            }
            case 'V':
            case 'v': {
                if (GetAsyncKeyState(VK_CONTROL) < 0) {
                    CTEditBox* pEditBoxID = NULL;
                    CTEditBox* pEditBoxPwd = NULL;

                    pCtrl = Find(IID_EDIT_ID);
                    if (pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX)
                        pEditBoxID = (CTEditBox*)pCtrl;

                    pCtrl = Find(IID_EDIT_PWD);
                    if (pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX)
                        pEditBoxPwd = (CTEditBox*)pCtrl;

                    if (pEditBoxID && pEditBoxPwd) {
                        if (pEditBoxID->IsFocus()) {
                            std::string ptext = CClipboardUtil::GetStringFromClibboard();
                            if (ptext.c_str() != NULL) {
                                pEditBoxID->Insert(ptext.c_str());
                            }
                        }

                        if (pEditBoxPwd->IsFocus()) {
                            std::string ptext = CClipboardUtil::GetStringFromClibboard();
                            if (ptext.c_str() != NULL) {
                                pEditBoxPwd->Insert(ptext.c_str());
                            }
                        }
                    }
                }
                return true;
            }

            default:
                break;
        }
    }

    unsigned iProcID = 0;
    if (iProcID = CTDialog::Process(uiMsg, wParam, lParam)) {
        switch (uiMsg) {
            case WM_LBUTTONDOWN:
                OnLButtonDown(iProcID);
                break;
            case WM_LBUTTONUP:
                OnLButtonUp(iProcID);
                break;
            default:
                break;
        }
        Ret = uiMsg;
    }
    return Ret;
}

/// set login user ID
/// @param strID user ID
/// @return is valid ID
bool
CLogin::SetID(const char* strID) {
    if (strID == NULL)
        return false;
    strcpy(m_strID, strID);

    g_GameDATA.username = m_strID;

    return true;
}

/// set login user Password
/// @param strID user Password
/// @return is valid Password
bool
CLogin::SetPassword(const char* strPassword) {
    if (strPassword == NULL)
        return false;
    strcpy(m_strPassword, strPassword);

    g_GameDATA.m_Password.Set(m_strPassword);

    return true;
}

/// send login request
void
CLogin::SendLoginReq() {
    CTCommand* pCmd = new CTCmdExit;
    g_EUILobby.ShowMsgBox(STR_WAIT_LOGIN, CTMsgBox::BT_CANCEL, true, GetDialogType(), NULL, pCmd);

    if (CGame::GetInstance().IsActiveRouteComboBox()) {
        ///콤보 박스에서 현재 선택된 아이템의 value를 구해온다.
        CWinCtrl* pComponent = Find(IID_COMBOBOX_ROUTE);
        if (pComponent && pComponent->GetControlType() == CTRL_JCOMBOBOX) {
            CJComboBox* pCombo = (CJComboBox*)pComponent;
            if (const CTObject* pObj = pCombo->GetSelectedItem()) {
                CTContainerItem* pItem = (CTContainerItem*)pObj;

                std::string temp_id = m_strID;
                temp_id.append(pItem->GetValue());
                CGame::GetInstance().SetJapanPartnerString(pItem->GetValue());

                g_pNet->Send_cli_LOGIN_REQ(temp_id, m_strPassword);

                g_ClientStorage.SetJapanRoute(pCombo->GetSelectedItemIndex());
                g_ClientStorage.SaveJapanRoute();
            }
        }
    } else {
        g_pNet->Send_cli_LOGIN_REQ(m_strID, m_strPassword);
    }
}
void
CLogin::Hide() {
    CTDialog::Hide();
    if (m_hUserGrade) {
        unloadTexture(m_hUserGrade);
        m_hUserGrade = NULL;
    }
}
void
CLogin::Show() {
    POINT pt = {g_pCApp->GetWIDTH() - GetWidth() - 50, 100};

    MoveWindow(pt);

    CTDialog::Show();

    CWinCtrl* pCtrl = Find(IID_EDIT_ID);
    CTEditBox* pEditBox = NULL;
    if (pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX) {
        pEditBox = (CTEditBox*)pCtrl;
    }

    pCtrl = Find(IID_EDIT_PWD);
    if (pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX) {
        pEditBox = (CTEditBox*)pCtrl;
    }

    if (pCtrl = Find(IID_CHECKBOX_SAVE_LASTCONECTID)) {
        if (pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX) {
            CTCheckBox* p = (CTCheckBox*)pCtrl;
            if (g_ClientStorage.IsSaveLastConnectID()) {
                p->SetCheck();

                const char* pszID = g_ClientStorage.GetLastConnectedID();
                if (pszID && pszID[0] != '\0') {
                    pCtrl = Find(IID_EDIT_ID);
                    if (pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX) {
                        pEditBox = (CTEditBox*)pCtrl;
                        pEditBox->SetText(g_ClientStorage.GetLastConnectedID());
                    }

                    if (pCtrl = Find(IID_EDIT_PWD))
                        pCtrl->SetFocus(true);
                }
            } else {
                p->SetUncheck();
            }
        }
    }

    pCtrl = Find(IID_EDIT_ID);
    if (pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX)
        ((CTEditBox*)pCtrl)->SetFocus(true);
}

void
CLogin::MsgBoxProc(unsigned int iMsgType, short int iButton) {
    switch (iMsgType) {
        case MSG_TYPE_EXIT:
            g_pCApp->SetExitGame();
            break;
        case MSG_TYPE_NORMAL:
            g_EUILobby.HideMsgBox();
            break;
        default:
            break;
    }
}
void
CLogin::OnLButtonUp(unsigned iProcID) {
    switch (iProcID) {
        case IID_BTN_OK:
            ConnectLoginServer();
            break;
        case IID_BTN_CANCEL:
            g_EUILobby.m_bLoginSuccess = false;
            g_pCApp->SetExitGame();
            break;
        default:
            break;
    }
}
void
CLogin::OnLButtonDown(unsigned iProcID) {
    switch (iProcID) {
        case IID_CHECKBOX_SAVE_LASTCONECTID:
            if (CWinCtrl* pCtrl = Find(IID_CHECKBOX_SAVE_LASTCONECTID)) {
                if (pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX) {
                    CTCheckBox* p = (CTCheckBox*)pCtrl;
                    if (p->IsCheck()) {
                        g_ClientStorage.SetSaveLastConnectID(true);
                    } else {
                        g_ClientStorage.SetSaveLastConnectID(false);
                        g_ClientStorage.SaveLastConnectID(NULL);
                        g_ClientStorage.SaveOptionLastConnectID();
                        pCtrl = Find(IID_EDIT_ID);
                        if (pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX) {
                            CTEditBox* pEditBox = (CTEditBox*)pCtrl;
                            pEditBox->clear_text();
                        }
                    }
                }
            }
            break;
        default:
            break;
    }
}
void
CLogin::Draw() {
    if (!IsVision())
        return;
    CTDialog::Draw();

    if (m_hUserGrade /*CLocalizing::GetSingleton().GetCurrentCodePageNO() == 949*/) {
        D3DXMATRIX mat;
        D3DXMatrixTranslation(&mat, (float)g_pCApp->GetWIDTH() - m_iWidthUserGrade - 50, 50.f, 0);
        ::setTransformSprite(mat);
        ::drawSprite(m_hUserGrade,
            NULL,
            NULL,
            &D3DXVECTOR3(0, 0, 0),
            D3DCOLOR_RGBA(255, 255, 255, 255));
    }

    D3DXMATRIX mat;
    D3DXMatrixTranslation(&mat, 50, 20, 0.0f);
    ::setTransformSprite(mat);

    // TODO: Dynamically set the version
    drawFont(g_GameDATA.m_hFONT[FONT_NORMAL_OUTLINE],
        true,
        0,
        0,
        g_dwWHITE,
        CStr::Printf("%s", GameConfig::NAME));
}

bool
CLogin::ConnectLoginServer() {
    if (!g_GameDATA.m_is_NHN_JAPAN) {
        CWinCtrl* pCtrl = Find(IID_EDIT_ID);
        CTEditBox* pEditBox = NULL;
        char* szTxt = NULL;
        if (pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX) {
            pEditBox = (CTEditBox*)pCtrl;
            szTxt = pEditBox->get_text();
            if (szTxt && strlen(szTxt) >= 1) {
                SetID(szTxt);
            } else {
                pEditBox->SetFocus(true);
                LogString(LOG_DEBUG_, "Empty ID EditBox in CLogin::SendLoginReq()\n");
                return false;
            }
        } else {
            LogString(LOG_DEBUG_, "Not Found ID EditBox in CLogin::SendLoginReq()\n");
            return false;
        }

        pCtrl = Find(IID_EDIT_PWD);
        if (pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX) {
            pEditBox = (CTEditBox*)pCtrl;
            szTxt = pEditBox->get_text();
            if (szTxt && strlen(szTxt) >= 4) {
                SetPassword(szTxt);
                pEditBox->clear_text();
            } else {
                pEditBox->SetFocus(true);
                LogString(LOG_DEBUG_, "Empty Password EditBox in CLogin::SendLoginReq()\n");
                return false;
            }
        } else {
            LogString(LOG_DEBUG_, "Not Found ID EditBox in CLogin::SendLoginReq()\n");
            return false;
        }
    }

    CTCommand* pCmd = new CTCmdExit;
    g_EUILobby.ShowMsgBox(STR_WAIT_CONNECT_LOGIN_SERVER,
        CTMsgBox::BT_CANCEL,
        true,
        EUI_LOGIN,
        NULL,
        pCmd);

    if (!g_pNet->ConnectToServer(g_GameDATA.server_ip, g_GameDATA.server_port, NS_CON_TO_LSV)) {
        g_EUILobby.ShowMsgBox(STR_LOGIN_FAIL, CTMsgBox::BT_OK, true, EUI_LOGIN, pCmd, NULL);
        return false;
    }

    return true;
}
