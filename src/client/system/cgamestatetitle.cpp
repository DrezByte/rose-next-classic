#include "stdafx.h"

#include "cgamestatetitle.h"

#include "CGame.h"
#include "CCamera.h"
#include "System_FUNC.h"
#include "SystemProcScript.h"
#include "interface/IO_ImageRes.h"

CGameStateTitle::CGameStateTitle(int iID):
    data_loaded(false), title_texture(NULL), background_zone_id(0) {
    m_iStateID = iID;
}

CGameStateTitle::~CGameStateTitle(void) {
    // NOTE: Crashes the application (std::terminate) if it is force closed while data_thread
    // is still running. Root cause is calling destructor of joinable std::thread.
    // The real issue is that we cannot safely join/detach the thread here because
    // it will corrupt memory anyway. By the time this destructor is called several
    // objects have been deallocated which load_data depends on. The fix to this
    // problem is to improve the memory safety of those objects (e.g. smart pointers, etc.)
    // and then join/detach the thread here.
}

int
CGameStateTitle::Update(bool bLostFocus) {
    if (this->data_loaded) {
        this->data_thread.join();

        if (g_GameDATA.auto_connect()) {
            CGame::GetInstance().ChangeState(CGame::GS_AUTOCONNECT);
        } else {
            CGame::GetInstance().ChangeState(CGame::GS_LOGIN);
        }
    }

    return 0;
}

int
CGameStateTitle::Enter(int iPrevStateID) {
    CGame::GetInstance().Load_DataNotUseThread();

    this->background_zone_id = SC_GetBGZoneNO();
    this->title_texture = loadTexture("logo.png", "logo.png", 1, 0);

    Draw();

    this->data_thread = std::thread(&CGameStateTitle::load_data, this);

    return 0;
}

int
CGameStateTitle::Leave(int iNextStateID) {
    if (this->title_texture) {
        unloadTexture(this->title_texture);
        this->title_texture = NULL;
    }
    return 0;
}

void
CGameStateTitle::load_data() {
    setDelayedLoad(0);
    CImageResManager::GetSingletonPtr()->LoadImageResources();

    g_itMGR.Init();

    CGame::GetInstance().Load_BasicDATA();

    //	g_pTerrain->SetLoadingMode( true );
    g_pTerrain->LoadZONE(this->background_zone_id, false);

    D3DVECTOR position;
    position.x = 520000.0f;
    position.y = 520000.0f;
    position.z = 0.0f;

    g_pCamera->Set_Position(position);
    ::setDelayedLoad(2);
    ::setDelayedLoad(0);

    this->data_loaded = true;
    return;
}

void
CGameStateTitle::Draw() {
    ::setClearColor(1, 1, 1);

    if (g_pCApp->IsActive()) {
        if (!::beginScene()) {
            return;
        }

        ::clearScreen();

        ::beginSprite(D3DXSPRITE_ALPHABLEND);

        int width, height;

        ::getTextureSize(this->title_texture, width, height);

        const D3DXVECTOR3 center(width / 2.0f, height / 2.0f, 0);
        const float scale_width = g_pCApp->GetWIDTH() / 1024.0f;
        const float scale_height = g_pCApp->GetHEIGHT() / 768.0f;

        D3DXMATRIX mat, mat_scale, mat_trans;
        D3DXMatrixScaling(&mat_scale, scale_width, scale_height, 0.0f);
        D3DXMatrixTranslation(&mat_trans,
            g_pCApp->GetWIDTH() / 2.0f,
            g_pCApp->GetHEIGHT() / 2.0f,
            0);
        D3DXMatrixMultiply(&mat, &mat_scale, &mat_trans);

        ::setTransformSprite(mat);
        ::drawSprite(this->title_texture, nullptr, &center, nullptr, zz_color_white);

        ::endSprite();
        ::endScene();

        ::swapBuffers();
    }
}