// Windows includes need to come before the others
// because some of the child includes depend on windows
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "rose/io/stb.h"
#include "util/classstb.h"


#ifndef SAFE_DELETE
    #define SAFE_DELETE(p)  \
        {                   \
            if (p) {        \
                delete (p); \
                (p) = NULL; \
            }               \
        }
#endif

#ifndef SAFE_DELETE_ARRAY
    #define SAFE_DELETE_ARRAY(p) \
        {                        \
            if (p) {             \
                delete[](p);     \
                (p) = NULL;      \
            }                    \
        }
#endif

#ifndef SAFE_RELEASE
    #define SAFE_RELEASE(p)     \
        {                       \
            if (p) {            \
                (p)->Release(); \
                (p) = NULL;     \
            }                   \
        }
#endif

//-------------------------------------------------------------------------------------------------
STBVALUE::STBVALUE() {
    m_bString = false;
    m_pString = NULL;
}
STBVALUE::~STBVALUE() {
    if (m_bString)
        SAFE_DELETE(m_pString);
}
void
STBVALUE::SetVALUE(char* szValue) {
    if (!szValue) {
        m_iValue = 0;
        if (m_pString)
            m_pString->Set(NULL);
        return;
    }

    m_iValue = atoi(szValue);

    int iLen = static_cast<int>(strlen(szValue));
    for (int iC = 0; iC < iLen; iC++) {
        if (!isdigit((unsigned char)szValue[iC])) {
            m_bString = true;
            m_pString = new CStrVAR;
            m_pString->Set(szValue);
            // LogString (LOG_DEBUG_, "S: %s ", szValue);
            return;
        }
    }
    // LogString (LOG_DEBUG_, "D: %d ", m_nValue);
}
void
STBVALUE::SetVALUE(int iValue) {
    m_iValue = iValue;
}

//-------------------------------------------------------------------------------------------------
#define __SEQ_READ

bool
STBDATA::Load2(char* szFileName, bool bHasTYPE, bool bMakeKEY) {
    classSTB cFILE;
    short nX, nY, nFindKEY;
    char* pStr;

    if (cFILE.Open(szFileName)) {
        this->m_bLoad2 = true;

        this->m_nDataCnt = (short)cFILE.GetRowCount();
        this->m_nColCnt = (short)cFILE.GetColumnCount();

        this->m_ppVALUE = new STBVALUE*[this->m_nDataCnt];
        ::ZeroMemory(this->m_ppVALUE, sizeof(STBVALUE*) * this->m_nDataCnt);

        t_HASHKEY HashKEY;

        nY = 0;
        if (bHasTYPE) {
            this->m_ppVALUE[nY] = new STBVALUE[this->m_nColCnt];
#ifdef __SEQ_READ
            cFILE.SetIndexPosition(0, 0);
#endif
            for (nX = 0; nX < this->m_nColCnt; nX++) {
#ifdef __SEQ_READ
                pStr = cFILE.GetString();
#else
                pStr = cFILE.GetString(nX, nY);
#endif
                if (pStr) {
                    this->m_ppVALUE[nY][nX].SetTYPE(0 == _strcmpi(pStr, "string"));
                } else
                    this->m_ppVALUE[nY][nX].SetTYPE(false);
            }
            nY++;
        }

        tagHASH<short>* pHashNode;
        for (; nY < this->m_nDataCnt; nY++) {
            this->m_ppVALUE[nY] = new STBVALUE[this->m_nColCnt];

            if (bMakeKEY) {
                HashKEY = cFILE.GetHASH(nY);
                if (HashKEY) {
                    pHashNode = this->m_KeyTable.Search(HashKEY);
                    nFindKEY = (pHashNode) ? pHashNode->m_DATA : 0;
                    if (nFindKEY) {
                        /*
#ifndef __SERVER
                        g_pCApp->ErrorBOX(CStr::Printf("[ %s / %s ] dupicated key ...",
                                              cFILE.GetNAME(nY),
                                              cFILE.GetNAME(nFindKEY)),
                            szFileName,
                            MB_OK);
#endif
*/
                    }

                    this->m_KeyTable.Insert(HashKEY, nY);
                }
            }

#ifdef __SEQ_READ
            cFILE.SetIndexPosition(0, nY);
#endif
            for (nX = 0; nX < this->m_nColCnt; nX++) {
                if (bHasTYPE) {
                    if (!this->m_ppVALUE[0][nX].IsString()) {
#ifdef __SEQ_READ
                        this->m_ppVALUE[nY][nX].SetVALUE(cFILE.GetInteger());
#else
                        this->m_ppVALUE[nY][nX].SetVALUE(cFILE.GetInteger(nX, nY));
#endif
                        continue;
                    }
                }

                pStr = cFILE.GetString(nX, nY);
                this->m_ppVALUE[nY][nX].SetVALUE(pStr);
            }
        }

        cFILE.Close();
    } else {
        /*
#ifndef __SERVER
        char szStr[MAX_PATH];
        sprintf(szStr, "STB file[ %s ] open error !!!", szFileName);
        g_pCApp->ErrorBOX(szStr, "ERROR", MB_OK);
#endif
*/
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

bool
STBDATA::Load(char* szFileName, bool bHasNameCol, bool bHasDescCol, bool bMakeKEY) {
    classSTB cFILE;
    short nX, nY, nFindKEY;
    char* pStr;

    if (cFILE.Open(szFileName)) {
        this->m_nDataCnt = (short)cFILE.GetRowCount();
        this->m_nColCnt = (short)cFILE.GetColumnCount();

        if (bHasDescCol) {
            this->m_nColCnt--;
            this->m_ppDESC = new char*[this->m_nDataCnt];
        } else
            this->m_ppDESC = NULL;

        this->m_ppDATA = new int*[this->m_nDataCnt];
        this->m_ppNAME = new char*[this->m_nDataCnt];

        tagHASH<short>* pHashNode;

        t_HASHKEY HashKEY;
        for (nY = 0; nY < cFILE.GetRowCount(); nY++) {
            this->m_ppDATA[nY] = new int[cFILE.GetColumnCount()];
            if (bMakeKEY) {
                HashKEY = cFILE.GetHASH(nY);
                if (HashKEY) {
                    pHashNode = this->m_KeyTable.Search(HashKEY);
                    nFindKEY = pHashNode ? pHashNode->m_DATA : 0;
                    if (nFindKEY) {
                        /*
#ifndef __SERVER
                        g_pCApp->ErrorBOX(CStr::Printf("[ %s / %s ] dupicated key ...",
                                              cFILE.GetNAME(nY),
                                              cFILE.GetNAME(nFindKEY)),
                            szFileName,
                            MB_OK);
#endif
*/
                    }

                    this->m_KeyTable.Insert(HashKEY, nY);
                }
            }

            if (bHasNameCol) {
                pStr = cFILE.GetString(0, nY);
                if (NULL != pStr) {
                    this->m_ppNAME[nY] = new char[1 + strlen(pStr)];
                    strcpy_s(this->m_ppNAME[nY], 1 + strlen(pStr), pStr);
                } else
                    this->m_ppNAME[nY] = NULL;

                nX = 1;
            } else {
                this->m_ppNAME[nY] = NULL;
                nX = 0;
            }

#ifdef __SEQ_READ
            cFILE.SetIndexPosition(nX, nY);
#endif
            for (; nX < this->m_nColCnt; nX++) {
#ifdef __SEQ_READ
                this->m_ppDATA[nY][nX] = (int)cFILE.GetInteger();
#else
                this->m_ppDATA[nY][nX] = (int)cFILE.GetInteger(nX, nY);
#endif
            }

            if (bHasDescCol) {
#ifdef __SEQ_READ
                pStr = cFILE.GetString();
#else
                pStr = cFILE.GetString(this->m_nColCnt, nY);
#endif
                if (NULL != pStr) {
                    this->m_ppDESC[nY] = new char[1 + strlen(pStr)];
                    strcpy_s(this->m_ppDESC[nY], 1 + strlen(pStr), pStr);
                } else
                    this->m_ppDESC[nY] = NULL;
            }
        }

        cFILE.Close();
    } else {
/*
#ifndef __SERVER
        char szStr[MAX_PATH];
        sprintf(szStr, "STB file[ %s ] open error !!!", szFileName);
        g_pCApp->ErrorBOX(szStr, "ERROR", MB_OK);
#endif
*/
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
void
STBDATA::Free(void) {
    if (m_bLoad2) {
        if (m_ppVALUE) {
            for (short nY = 0; nY < this->m_nDataCnt; nY++) {
                SAFE_DELETE_ARRAY(this->m_ppVALUE[nY]);
            }

            SAFE_DELETE_ARRAY(this->m_ppVALUE);
        }
        return;
    }

    if (this->m_ppDATA) {
        for (short nY = 0; nY < this->m_nDataCnt; nY++) {
            SAFE_DELETE_ARRAY(this->m_ppDATA[nY]);
            SAFE_DELETE_ARRAY(this->m_ppNAME[nY]);

            if (this->m_ppDESC)
                SAFE_DELETE_ARRAY(this->m_ppDESC[nY]);
        }

        SAFE_DELETE_ARRAY(this->m_ppDATA);
        SAFE_DELETE_ARRAY(this->m_ppNAME);
        SAFE_DELETE_ARRAY(this->m_ppDESC);
    }
}

//-------------------------------------------------------------------------------------------------
bool
STBDATA::LoadWSTB(bool bCheckQuotationMark, char* szFileName, int iKeyColIDX, ...) {
    classSTB cFILE;
    short nY, nFindKEY;

    if (cFILE.Open(szFileName)) {
        this->m_bLoad2 = true;

        this->m_nDataCnt = (short)cFILE.GetRowCount();
        this->m_nColCnt = (short)cFILE.GetColumnCount();

        this->m_ppVALUE = new STBVALUE*[this->m_nDataCnt];
        ::ZeroMemory(this->m_ppVALUE, sizeof(STBVALUE*) * this->m_nDataCnt);

#define MAX_TEMP_STR 2048
        wchar_t* pWStr;
        char* pStr = new char[MAX_TEMP_STR];

        int iRetCnt;
        t_HASHKEY HashKEY;
        tagHASH<short>* pHashNode;
        for (nY = 0; nY < this->m_nDataCnt; nY++) {
            this->m_ppVALUE[nY] = new STBVALUE[this->m_nColCnt];

            if (iKeyColIDX >= 0) {
                pWStr = (wchar_t*)cFILE.GetString(iKeyColIDX, nY);
                iRetCnt =
                    ::WideCharToMultiByte(CP_ACP, 0, pWStr, -1, pStr, MAX_TEMP_STR, NULL, NULL);
                if (*pStr && iRetCnt) {
                    HashKEY = StrToHashKey(pStr);
                    if (HashKEY) {
                        pHashNode = this->m_KeyTable.Search(HashKEY);
                        nFindKEY = (pHashNode) ? pHashNode->m_DATA : 0;
                        if (nFindKEY) {
/*
#ifndef __SERVER
                            g_pCApp->ErrorBOX(CStr::Printf("[ %s / %s ] dupicated key ...",
                                                  cFILE.GetNAME(nY),
                                                  cFILE.GetNAME(nFindKEY)),
                                szFileName,
                                MB_OK);
#else
*/
                            for (int iP = 0; iP < iRetCnt; iP++) {
                                if (!pStr[iP])
                                    break;
                                if (pStr[iP] != ' ') {
                                    // LOG_WARN("Duplicate key[ {}:{} ] in %s", nY, pStr, szFileName);
                                    break;
                                }
                            }
// #endif
                        }

                        this->m_KeyTable.Insert(HashKEY, nY);
                    }
                    // LogString(0xffff,"  [%d] Key:: %s ", nY, pStr );
                }
            }

            va_list vaLst;
            va_start(vaLst, iKeyColIDX);
            int iColIDX;

            char* pTmpStr;
            while ((iColIDX = va_arg(vaLst, int)) >= 0) {
                assert(iColIDX < this->m_nColCnt);

                pWStr = (wchar_t*)cFILE.GetString(iColIDX, nY);
                iRetCnt =
                    ::WideCharToMultiByte(CP_ACP, 0, pWStr, -1, pStr, MAX_TEMP_STR, NULL, NULL);
                if (iRetCnt) {
                    if (bCheckQuotationMark) {
                        pTmpStr = pStr;
                        while (*pTmpStr) {
                            if (*pTmpStr == '\'')
                                *pTmpStr = '`';
                            pTmpStr++;
                        }
                    }
                    this->m_ppVALUE[nY][iColIDX].SetVALUE(pStr);
                }
                // if ( *pStr && iRetCnt )
                //	LogString(0xffff,"    %d:%s", iColIDX, pStr );
            }
            va_end(vaLst);
            // LogString(0xffff,"\n" );
        }

        delete[] pStr;

        cFILE.Close();
    } else {
        /*
#ifndef __SERVER
        char szStr[MAX_PATH];
        sprintf(szStr, "STB file[ %s ] open error !!!", szFileName);
        g_pCApp->ErrorBOX(szStr, "ERROR", MB_OK);
#endif
*/
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
