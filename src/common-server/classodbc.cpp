#include <crtdbg.h>
#include <stdio.h>
#include <string>
#include <tchar.h>
#include <vector>
#include <windows.h>

#include "classODBC.h"
#include "rose/common/log.h"

#pragma comment(lib, "odbc32.lib")
// #pragma	comment( lib, "odbccp32.lib" )

classODBC::classODBC(short nBindParamCNT, WORD wQueryBufferLEN) {
    m_ErrMSG.Alloc(512);

    m_hENV = SQL_NULL_HENV;
    m_hDBC1 = SQL_NULL_HDBC;
    m_hSTMT1 = SQL_NULL_HSTMT;

    m_nAllocColCnt = 0;
    m_pColumn = NULL;
    AllocColumn(MAX_ODBC_COLUMN_CNT);

    m_nBindParamCNT = 0;
    m_pBindPARAM = NULL;
    AllocParam(MAX_ODBC_BIND_PARAMETER);

    m_dwMaxQueryBuffSize = wQueryBufferLEN;
    m_pQueryBuff = new char[wQueryBufferLEN];

    param_data.resize(MAX_BIND_PARAMS);
}
classODBC::~classODBC() {
    delete[] m_pColumn;
}

//-------------------------------------------------------------------------------------------------
// information about DSN
bool
classODBC::FirstDSN(char* pszDSN, int lMax) {
    RETCODE rc;
    short cbData, cbData1;
    char szDesc[256];

    rc = ::SQLDataSources(m_hENV,
        SQL_FETCH_FIRST,
        (unsigned char*)pszDSN,
        lMax,
        &cbData,
        (unsigned char*)szDesc,
        256,
        &cbData1);
    if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
        return true;

    ::OutputDebugString(this->GetERROR());
    return false;
}

bool
classODBC::NextDSN(char* pszDSN, int lMax) {
    RETCODE rc;
    short cbData, cbData1;
    char szDesc[256];

    rc = ::SQLDataSources(m_hENV,
        SQL_FETCH_NEXT,
        (unsigned char*)pszDSN,
        lMax,
        &cbData,
        (unsigned char*)szDesc,
        256,
        &cbData1);
    if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
        return true;

    ::OutputDebugString(this->GetERROR());
    return false;
}

bool
classODBC::RegisterDSN(char* szDSN, char* szDBName, char* szServerIP, char* szUser) {
    DWORD dwRet;
    HKEY hKey = NULL;
    BYTE buffer[128];
    ULONG size = 128;
    if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
            _T("Software\\ODBC\\ODBCINST.INI\\SQL Server"),
            0,
            KEY_READ,
            &hKey)
        != ERROR_SUCCESS) {
        return false;
    }

    // C:\WIN98\SYSTEM\SQLSRV32.DLL
    buffer[0] = 0;
    DWORD type = REG_SZ;
    dwRet = ::RegQueryValueEx(hKey, _T("Driver"), NULL, &type, buffer, &size);
    ::RegCloseKey(hKey);

    CStrVAR stMsg;
    stMsg.Printf("Software\\ODBC\\ODBC.INI\\%s", szDSN);
    // CString msg;
    // msg.Format("Software\\ODBC\\ODBC.INI\\%s", m_dns);

    if (::RegCreateKey(HKEY_CURRENT_USER, stMsg.Get(), &hKey) != ERROR_SUCCESS) {
        return false;
    }

    dwRet = ::RegSetValueEx(hKey,
        _T("Database"),
        NULL,
        REG_SZ,
        (const BYTE*)(LPCSTR)szDBName,
        strlen(szDBName));
    dwRet = ::RegSetValueEx(hKey,
        _T("Server"),
        NULL,
        REG_SZ,
        (const BYTE*)(LPCSTR)szServerIP,
        strlen(szServerIP));
    dwRet = ::RegSetValueEx(hKey,
        _T("Lastuser"),
        NULL,
        REG_SZ,
        (const BYTE*)(LPCSTR)szUser,
        strlen(szUser));
    dwRet = ::RegSetValueEx(hKey, _T("Driver"), NULL, REG_SZ, buffer, strlen((char*)buffer));

    ::RegCloseKey(hKey);
    if (::RegCreateKey(HKEY_CURRENT_USER, _T("SOFTWARE\\ODBC\\ODBC.INI\\ODBC Data Sources"), &hKey)
        != ERROR_SUCCESS) {
        return false;
    }

    stMsg.Set("SQL Server");
    dwRet = ::RegSetValueEx(hKey,
        szDSN,
        NULL,
        REG_SZ,
        (const BYTE*)(LPCSTR)stMsg.Get(),
        stMsg.BuffLength());

    return true;
}

//-------------------------------------------------------------------------------------------------
void
classODBC::AllocParam(short nCntToAlloc) {
    tagODBCDATA* pNEW;

    pNEW = new tagODBCDATA[nCntToAlloc];

    for (short nI = 0; nI < m_nBindParamCNT; nI++)
        pNEW[nI] = m_pBindPARAM[nI];
    if (m_pBindPARAM)
        delete[] m_pBindPARAM;

    m_nBindParamCNT = nCntToAlloc;
    m_pBindPARAM = pNEW;
}

//-------------------------------------------------------------------------------------------------
void
classODBC::AllocColumn(short nCntToAlloc) {
    if (m_pColumn)
        delete[] m_pColumn;

    m_nAllocColCnt = nCntToAlloc;
    m_pColumn = new tagODBCCOL[m_nAllocColCnt];
}

//-------------------------------------------------------------------------------------------------
const char*
classODBC::GetERROR(void) {
    if (SQL_NULL_HSTMT == m_hSTMT1)
        return "ODBC ERROR:: Can't get result";

    SQLINTEGER NativeError;
    SQLCHAR SqlState[6], Msg[255];
    SQLSMALLINT MsgLen;

    int iCnt = 1;
    while (SQL_NO_DATA
        != ::SQLGetDiagRec(SQL_HANDLE_STMT,
            m_hSTMT1,
            iCnt,
            SqlState,
            &NativeError,
            Msg,
            sizeof(Msg),
            &MsgLen)) {
        g_LOG.CS_ODS(0xffff, "SQLSTATE:%s, Diagnostic information:%s \n", SqlState, Msg);
        if (1 == iCnt) {
            this->m_ErrMSG.Printf("SQLSTATE:%s, %s \n", SqlState, Msg);
        }
        iCnt++;
    }

    if (iCnt > 1)
        return this->m_ErrMSG.Get();

    return "ODBC ERROR:: Unknown";
}

//-------------------------------------------------------------------------------------------------
bool
classODBC::ReigsterDsnIfNone(char* szDSN, char* szDBName, char* szServerIP, char* szUser) {
    // Allocate the ODBC Environment and save handle.
    m_RetCode = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &m_hENV);
    if ((m_RetCode != SQL_SUCCESS) && (m_RetCode != SQL_SUCCESS_WITH_INFO))
        return false;

    // Notify ODBC that this is an ODBC 3.0 application.
    m_RetCode =
        SQLSetEnvAttr(m_hENV, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
    if ((m_RetCode != SQL_SUCCESS) && (m_RetCode != SQL_SUCCESS_WITH_INFO))
        return false;

    // 설치된 dns 검색...
    char szFindDSN[MAX_PATH];
    bool bFindDSN = false;
    if (this->FirstDSN(szFindDSN, MAX_PATH)) {
        do {
            if (!strcmpi(szFindDSN, szDSN))
                bFindDSN = true;

            g_LOG.CS_ODS(0xffff, "Find DSN ::: %s", szFindDSN);
        } while (this->NextDSN(szFindDSN, MAX_PATH));
    }

    if (!bFindDSN) {
        // ODBC 설치...
        bFindDSN = this->RegisterDSN(szDSN, szDBName, szServerIP, szUser);
    }

    ::SQLFreeHandle(SQL_HANDLE_ENV, m_hENV);
    m_hENV = SQL_NULL_HENV;

    return bFindDSN;
}

//-------------------------------------------------------------------------------------------------
bool
classODBC::Connect(char* szDSN, char* szUserName, char* szPassword) {
    // Allocate the ODBC Environment and save handle.
    m_RetCode = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &m_hENV);
    if ((m_RetCode != SQL_SUCCESS) && (m_RetCode != SQL_SUCCESS_WITH_INFO))
        return false;

    // Notify ODBC that this is an ODBC 3.0 application.
    m_RetCode =
        SQLSetEnvAttr(m_hENV, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
    if ((m_RetCode != SQL_SUCCESS) && (m_RetCode != SQL_SUCCESS_WITH_INFO))
        return false;

    // Allocate an ODBC connection handle and connect.
    m_RetCode = ::SQLAllocHandle(SQL_HANDLE_DBC, m_hENV, &m_hDBC1);
    if ((m_RetCode != SQL_SUCCESS) && (m_RetCode != SQL_SUCCESS_WITH_INFO))
        return false;

    m_RetCode = ::SQLConnect(m_hDBC1,
        (UCHAR*)szDSN,
        (SWORD)strlen((char*)szDSN),
        (UCHAR*)szUserName,
        (SWORD)strlen((char*)szUserName),
        (UCHAR*)szPassword,
        (SWORD)strlen((char*)szPassword));
    if ((m_RetCode != SQL_SUCCESS) && (m_RetCode != SQL_SUCCESS_WITH_INFO)) {
        SQLCHAR state[5 + 1];
        SQLINTEGER err_code;
        SQLCHAR err_message[255];

        SQLRETURN ret =
            SQLGetDiagRec(SQL_HANDLE_DBC, m_hDBC1, 1, state, &err_code, err_message, 256, NULL

            );
        LOG_ERROR("SQLConnect() failed with error state: %s (DSN: %s)", (char*)state, szDSN);
        LOG_ERROR((char*)err_message);
        return false;
    }
    m_RetCode = ::SQLAllocHandle(SQL_HANDLE_STMT, m_hDBC1, &m_hSTMT1);
    if ((m_RetCode != SQL_SUCCESS) && (m_RetCode != SQL_SUCCESS_WITH_INFO))
        return false;

    return true;
}
//-------------------------------------------------------------------------------------------------
void
classODBC::Disconnect(void) {
    /* Clean up. */
    ::SQLFreeHandle(SQL_HANDLE_STMT, m_hSTMT1);
    ::SQLDisconnect(m_hDBC1);
    ::SQLFreeHandle(SQL_HANDLE_DBC, m_hDBC1);
    ::SQLFreeHandle(SQL_HANDLE_ENV, m_hENV);
}

//-------------------------------------------------------------------------------------------------
bool
classODBC::BindPARAM(short nParamIDX, BYTE* pData, unsigned int uiDataSize) {
    _ASSERT(nParamIDX > 0 && nParamIDX < m_nBindParamCNT);
    if (nParamIDX < 1 || nParamIDX > m_nBindParamCNT)
        return false;

    m_pBindPARAM[nParamIDX].m_pDATA = pData;
    m_pBindPARAM[nParamIDX].m_uiDataSize = uiDataSize;
    m_pBindPARAM[nParamIDX].m_StrLen_or_Ind = SQL_LEN_DATA_AT_EXEC((int)uiDataSize);

    m_RetCode = ::SQLBindParameter(m_hSTMT1, // SQLHSTMT			StatementHandle,
        (SQLUSMALLINT)nParamIDX, // SQLUSMALLINT     ParameterNumber,
        SQL_PARAM_INPUT, // SQLSMALLINT		InputOutputType,
        SQL_C_BINARY, // SQLSMALLINT		ValueType,
        SQL_BINARY, // SQL_LONGVARBINARY,// SQLSMALLINT		ParameterType,
        (SQLUINTEGER)uiDataSize, // SQLUINTEGER		ColumnSize,
        0, // SQLSMALLINT		DecimalDigits,
        (SQLPOINTER)nParamIDX, // SQLPOINTER		ParameterValuePtr, 변수 대신 32비트의 임의값을
                               // 지정-> BLOB ID
        0, // SQLINTEGER		BufferLength,
        &m_pBindPARAM[nParamIDX].m_StrLen_or_Ind // SQLINTEGER *     StrLen_or_IndPtr);
    );

    if (m_RetCode == SQL_SUCCESS)
        return true;

    ::OutputDebugString(this->GetERROR());
    return false;
}

/*
void classODBC::SetParam_bool		(UINT nParamId,bool bParamValue)
void classODBC::SetParam_char		(UINT nParamId,char chParamValue);
void classODBC::SetParam_short		(UINT nParamId,short iParamValue);
void classODBC::SetParam_long		(UINT nParamId,long nParamValue);
void classODBC::SetParam_float		(UINT nParamId,float fltParamValue);
void classODBC::SetParam_double		(UINT nParamId,double dblParamValue);
*/
bool
classODBC::SetParameter(short nParamIDX,
    BYTE* pData,
    UINT uiDataSize,
    short nIOType,
    short nValueType,
    short nParamType) {
    m_pBindPARAM[nParamIDX].m_pDATA = pData;
    m_pBindPARAM[nParamIDX].m_uiDataSize = uiDataSize;
    m_pBindPARAM[nParamIDX].m_StrLen_or_Ind = SQL_LEN_DATA_AT_EXEC((int)uiDataSize);

    m_RetCode = ::SQLBindParameter(m_hSTMT1, // SQLHSTMT			StatementHandle,
        (SQLUSMALLINT)nParamIDX, // SQLUSMALLINT     ParameterNumber,
        nIOType, // SQLSMALLINT		InputOutputType,
        nValueType, // SQLSMALLINT		ValueType,
        nParamType, // SQLSMALLINT		ParameterType,
        (SQLUINTEGER)uiDataSize, // SQLUINTEGER		ColumnSize,
        0, // SQLSMALLINT		DecimalDigits,
        (SQLPOINTER)nParamIDX, // SQLPOINTER		ParameterValuePtr, 변수 대신 32비트의 임의값을
                               // 지정-> BLOB ID
        0, // SQLINTEGER		BufferLength,
        &m_pBindPARAM[nParamIDX].m_StrLen_or_Ind // SQLINTEGER *     StrLen_or_IndPtr);
    );
    if (m_RetCode == SQL_SUCCESS)
        return true;

    ::OutputDebugString(this->GetERROR());
    return false;
}
bool
classODBC::SetParam_long(short nParamIDX,
    long& lOutResult,
    long& cbLen) { /*	SQLBindParameter(
                           hstmt,
                           1,
                           SQL_PARAM_OUTPUT,
                           SQL_C_LONG,
                           SQL_INTEGER,
                           sizeof( long ),
                           0,
                           & param1,
                           sizeof( long ),
                           & cbValue1 );	*/
    m_RetCode = ::SQLBindParameter(m_hSTMT1, // SQLHSTMT			StatementHandle,
        nParamIDX, //(SQLUSMALLINT) nParamIDX,		// SQLUSMALLINT     ParameterNumber,
        SQL_PARAM_OUTPUT, // SQLSMALLINT		InputOutputType,
        SQL_C_SLONG, // SQLSMALLINT		ValueType,
        SQL_INTEGER, // SQLSMALLINT		ParameterType,
        0, // sizeof( long ),					// SQLUINTEGER		ColumnSize,
        0, // SQLSMALLINT		DecimalDigits,
        &lOutResult, // SQLPOINTER		ParameterValuePtr, 변수 대신 32비트의 임의값을 지정-> BLOB
                     // ID
        0, // SQLINTEGER		BufferLength,
        &cbLen // SQLINTEGER *     StrLen_or_IndPtr);
    );
    if (m_RetCode == SQL_SUCCESS)
        return true;

    ::OutputDebugString(this->GetERROR());
    return false;
}

bool
classODBC::SetParam_string(short nParamIDX, char* szStr) {
    size_t uiLen = strlen(szStr);
    return SetParameter(nParamIDX, (BYTE*)szStr, uiLen, SQL_PARAM_OUTPUT, SQL_C_CHAR, SQL_VARCHAR);
}

bool
classODBC::SetParam_wstring(short nParamIDX, char* szStr) {
    size_t uiLen = strlen(szStr);
    return SetParameter(nParamIDX, (BYTE*)szStr, uiLen, SQL_PARAM_OUTPUT, SQL_C_CHAR, SQL_WVARCHAR);
}
/*
void classODBC::SetParam_DateTime	(UINT nParamId,TIMESTAMP_STRUCT dtParamValue);
*/

//-------------------------------------------------------------------------------------------------
bool
classODBC::BindRESULT() {
    ::SQLNumResultCols(m_hSTMT1, &m_nResultColCnt);
    if (0 == m_nResultColCnt) {
        // 0 인경우는 select문 이외의 다른 명령을 실행한 경우로 바인딩할 필요없다.
        // this->Clear ();
        return false;
    }

    if (m_nResultColCnt > m_nAllocColCnt)
        AllocColumn(m_nResultColCnt);

    SQLSMALLINT DecimalDigits, Nullable;
    for (short nI = 0; nI < m_nResultColCnt; nI++) {
        m_RetCode = ::SQLDescribeCol(m_hSTMT1,
            nI + 1,
            m_pColumn[nI].m_Name,
            MAX_ODBC_COLUMN_NAME,
            &m_pColumn[nI].m_NameLen,
            &m_pColumn[nI].m_SqlCType,
            &m_pColumn[nI].m_ColumnSize,
            &DecimalDigits,
            &Nullable);

        if (m_RetCode != SQL_SUCCESS && m_RetCode != SQL_SUCCESS_WITH_INFO) {
            return false;
        }

        switch (m_pColumn[nI].m_SqlCType) {
            case SQL_BIT:
            case SQL_TINYINT:
            case SQL_SMALLINT:
            case SQL_INTEGER:
                m_pColumn[nI].m_SqlCType = SQL_C_LONG;
                // m_pColumn[ nI ].iLen = 4;
                break;

            case SQL_BIGINT:
                m_pColumn[nI].m_SqlCType = SQL_C_SBIGINT;
                break;

            case SQL_NUMERIC:
            case SQL_DECIMAL:
                m_pColumn[nI].m_SqlCType = SQL_C_LONG;
                break;

            case SQL_DOUBLE:
            case SQL_FLOAT:
            case SQL_REAL:
                m_pColumn[nI].m_SqlCType = SQL_C_DOUBLE;
                // m_pColumn[ nI ].iLen = 8;
                break;
            case SQL_DATE:
            case SQL_TYPE_DATE:
                m_pColumn[nI].m_SqlCType = SQL_C_DATE;
                // m_pColumn[ nI ].iLen = sizeof(CGOdbcStmt::DATE);
                break;
            case SQL_TIMESTAMP:
            case SQL_TYPE_TIMESTAMP:
                m_pColumn[nI].m_SqlCType = SQL_C_TIMESTAMP;
                // m_pColumn[ nI ].iLen = sizeof(CGOdbcStmt::TIMESTAMP);
                break;
            case SQL_CHAR:
            case SQL_VARCHAR:
                m_pColumn[nI].m_SqlCType = SQL_C_CHAR;
                // m_pColumn[ nI ].iLen = m_pColumn[ nI ].iSqlSize + 1;
                break;
            case SQL_LONGVARCHAR:
                // case    SQL_TYPE_ORACLE_CLOB:
                m_pColumn[nI].m_SqlCType = SQL_C_CHAR;
                // m_pColumn[ nI ].bLOB = true;
                // m_pColumn[ nI ].iLen = 0;
                break;
            case SQL_BINARY:
            case SQL_VARBINARY:
                m_pColumn[nI].m_SqlCType = SQL_C_BINARY;
                // m_pColumn[ nI ].iLen = m_pColumn[ nI ].iSqlSize;
                break;
            case SQL_LONGVARBINARY:
                // case    SQL_TYPE_ORACLE_BLOB:
                m_pColumn[nI].m_SqlCType = SQL_C_BINARY;
                // m_pColumn[ nI ].bLOB = true;
                // m_pColumn[ nI ].iLen = 0;
                break;
            case SQL_GUID:
                m_pColumn[nI].m_SqlCType = SQL_C_GUID;
                // m_pColumn[ nI ].iLen = sizeof(GUID);
                break;
            default:
                m_pColumn[nI].m_SqlCType = SQL_C_CHAR;
                // m_pColumn[ nI ].iLen = 128;
                break;
        }

        m_RetCode = ::SQLBindCol(m_hSTMT1,
            nI + 1,
            m_pColumn[nI].m_SqlCType,
            m_pColumn[nI].m_Data,
            MAX_ODBC_COLUMN_LEN,
            &m_pColumn[nI].m_StrLen_or_IndPtr);
        if ((m_RetCode != SQL_SUCCESS) && (m_RetCode != SQL_SUCCESS_WITH_INFO)) {
            OutputDebugString("Error in SQLBindCol \n");
        }
        /*
            //allocate data buffer and bind columns
            m_pData = new unsigned char[lBuffSize];
            for (i = 0; i < m_iColCount; i++)
            {
                if (m_pColumn[i].bLOB)
                {
                    m_pColumn[i].pData = 0;
                    rc = SQLBindCol(m_hStmt, i + 1, m_pColumn[i].m_SqlCType,
                                    0, 0, &m_pColumn[i].cbData);
                }
                else
                {
                    m_pColumn[i].pData = m_pData + m_pColumn[i].lOffset;
                    rc = SQLBindCol(m_hStmt, i + 1, m_pColumn[i].m_SqlCType,
                                    m_pColumn[i].pData, m_pColumn[i].iLen, &m_pColumn[i].cbData);
                }
                if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
                    _throwError();
            }
        */
    }
    return true;
}

//-------------------------------------------------------------------------------------------------
bool
classODBC::QuerySQLBuffer(/* bool bBindResult */) {
    // Init ...
    ::SQLCloseCursor(m_hSTMT1);
    /*
    m_RetCode = ::SQLCloseCursor (m_hSTMT1);
    if ( (m_RetCode != SQL_SUCCESS) && (m_RetCode != SQL_SUCCESS_WITH_INFO) ) {
        :: OutputDebugString( this->GetERROR() );
        // return false;
    }
    */

    m_RetCode = ::SQLExecDirect(m_hSTMT1, (SQLCHAR*)m_pQueryBuff, SQL_NTS);
    if (m_RetCode == SQL_NEED_DATA) {
        SQLPOINTER pBLOB_ID;
        int iParamNo;
        unsigned int uiPtr, uiLen;

        m_RetCode = ::SQLParamData(m_hSTMT1, &pBLOB_ID);
        while (m_RetCode == SQL_NEED_DATA) {
            /*
            SQL_NEED_DATA 리턴되면 실행중에 데이터를 공급해 주어야 한다.
            그전에 어떤 필드에 대한 테이터를 요구하는지를 조사해야 하는데 SQLParamData함수를
            사용한다. SQLParamData(SQLHSTMT StatementHandle, SQLPOINTER *ValuePtrPtr); <-- 2번째
            값에 BLOB ID가 온다. SQLPutData로 실제 데이타를 보내준다.
            */
            iParamNo = (int)pBLOB_ID;
            if (iParamNo >= m_nBindParamCNT) {
                // Too many records count...
                ::OutputDebugString("Too many result records\n");
                break;
            }

            for (uiPtr = 0; uiPtr < m_pBindPARAM[iParamNo].m_uiDataSize;
                 uiPtr += MAX_ODBC_COLUMN_LEN) {
                uiLen =
                    min((ULONG)MAX_ODBC_COLUMN_LEN, m_pBindPARAM[iParamNo].m_uiDataSize - uiPtr);
                ::SQLPutData(m_hSTMT1, &m_pBindPARAM[iParamNo].m_pDATA[uiPtr], uiLen);
            }

            m_RetCode = ::SQLParamData(m_hSTMT1, &pBLOB_ID);
        }
    }
    if ((m_RetCode != SQL_SUCCESS) && (m_RetCode != SQL_SUCCESS_WITH_INFO)
        && (m_RetCode != SQL_NO_DATA)) {
        ::OutputDebugString(this->GetERROR());
        return false;
    }

    //	if ( bBindResult )
    this->BindRESULT();

    return true;
}
bool
classODBC::QuerySQL(char* szFormat, ...) {
    va_list ArgPtr;

    va_start(ArgPtr, szFormat);
    vsprintf(m_pQueryBuff, szFormat, ArgPtr);
    va_end(ArgPtr);

    ::OutputDebugString(m_pQueryBuff);

    return this->QuerySQLBuffer();
}
// bool classODBC::QuerySP (char *szFormat, ...)
//{
//    va_list ArgPtr;
//
//    va_start(ArgPtr, szFormat);
//    vsprintf(m_pQueryBuff, szFormat, ArgPtr);
//    va_end(ArgPtr);
//
//	return this->QuerySQLBuffer( false );
//}

//-------------------------------------------------------------------------------------------------
bool
classODBC::QueryString(char* szQuery, unsigned long ulLength) {
    ::CopyMemory(m_pQueryBuff, szQuery, ulLength);
    return this->QuerySQLBuffer();
}
bool
classODBC::QueryString(char* szQuery) {
    strcpy(m_pQueryBuff, szQuery);
    return this->QuerySQLBuffer();
}

//-------------------------------------------------------------------------------------------------
int
classODBC::ExecSQLBuffer() {
    // Init ...
    ::SQLCloseCursor(m_hSTMT1);
    /*
    m_RetCode = ::SQLCloseCursor (m_hSTMT1);
    if ( (m_RetCode != SQL_SUCCESS) && (m_RetCode != SQL_SUCCESS_WITH_INFO) ) {
        :: OutputDebugString( this->GetERROR() );
        return false;
    }
    */

    m_iResultRowCnt = 0;
    m_RetCode = ::SQLExecDirect(m_hSTMT1, (SQLCHAR*)m_pQueryBuff, SQL_NTS);
    if (m_RetCode == SQL_NEED_DATA) {
        SQLPOINTER pBLOB_ID;
        int iParamNo;
        unsigned int uiPtr, uiLen;

        m_RetCode = ::SQLParamData(m_hSTMT1, &pBLOB_ID);
        while (m_RetCode == SQL_NEED_DATA) {
            /*
            SQL_NEED_DATA 리턴되면 실행중에 데이터를 공급해 주어야 한다.
            그전에 어떤 필드에 대한 테이터를 요구하는지를 조사해야 하는데 SQLParamData함수를
            사용한다. SQLParamData(SQLHSTMT StatementHandle, SQLPOINTER *ValuePtrPtr); <-- 2번째
            값에 BLOB ID가 온다. SQLPutData로 실제 데이타를 보내준다.
            */
            iParamNo = (int)pBLOB_ID;
            for (uiPtr = 0; uiPtr < m_pBindPARAM[iParamNo].m_uiDataSize;
                 uiPtr += MAX_ODBC_COLUMN_LEN) {
                uiLen =
                    min((ULONG)MAX_ODBC_COLUMN_LEN, m_pBindPARAM[iParamNo].m_uiDataSize - uiPtr);
                ::SQLPutData(m_hSTMT1, &m_pBindPARAM[iParamNo].m_pDATA[uiPtr], uiLen);
            }

            m_RetCode = ::SQLParamData(m_hSTMT1, &pBLOB_ID);
        }
    }

    if ((m_RetCode != SQL_SUCCESS) && (m_RetCode != SQL_SUCCESS_WITH_INFO)
        && (m_RetCode != SQL_NO_DATA)) {
        ::OutputDebugString(this->GetERROR());
        return -1;
    }

    // Update, Delete, Insert명령시 영향받은 레코드 갯수를 구해온다.
    m_RetCode = ::SQLRowCount(m_hSTMT1, &m_iResultRowCnt);
    if ((m_RetCode != SQL_SUCCESS) && (m_RetCode != SQL_SUCCESS_WITH_INFO)) {
        ::OutputDebugString(this->GetERROR());
        return -1;
    }

    // this->Clear ();

    return m_iResultRowCnt;
}
int
classODBC::ExecSQL(char* szFormat, ...) {
    va_list ArgPtr;

    va_start(ArgPtr, szFormat);
    vsprintf(m_pQueryBuff, szFormat, ArgPtr);
    va_end(ArgPtr);

    return this->ExecSQLBuffer();
}

//-------------------------------------------------------------------------------------------------
int
classODBC::ExecSQLBuffer(char* szQuery, unsigned long ulLength) {
    ::CopyMemory(m_pQueryBuff, szQuery, ulLength);
    return this->ExecSQLBuffer();
}
int
classODBC::ExecSQLBuffer(char* szQuery) {
    strcpy(m_pQueryBuff, szQuery);
    return this->ExecSQLBuffer();
}

//-------------------------------------------------------------------------------------------------
bool
classODBC::Fetch() {
    m_RetCode = ::SQLFetch(m_hSTMT1);
    if (m_RetCode == SQL_ERROR)
        return false;

#ifdef _DEBUG
    for (short nI = 0; nI < m_nResultColCnt; nI++)
        _ASSERT(m_pColumn[nI].m_StrLen_or_IndPtr < MAX_ODBC_COLUMN_LEN);
#endif

    return (m_RetCode != SQL_NO_DATA);
}

//-------------------------------------------------------------------------------------------------
void
classODBC::Clear(void) {
    if (m_hSTMT1 != SQL_NULL_HSTMT) {
        ::SQLCloseCursor(m_hSTMT1);
        //		SQLFreeStmt (m_hSTMT1, SQL_UNBIND);
        /*
            return;
            // Free
            SQLFreeStmt( m_hSTMT1, SQL_DROP );
            // Allocate again.
            SQLAllocStmt( m_hDBC1, &m_hSTMT1 );
        */
    };
}

//-------------------------------------------------------------------------------------------------
bool
classODBC::GetTimestamp(WORD wColumnIDX, sqlTIMESTAMP* pTimeStamp) {
    if (wColumnIDX > m_nResultColCnt)
        return false;

    if (SQL_NULL_DATA == m_pColumn[wColumnIDX].m_StrLen_or_IndPtr)
        return false;

    pTimeStamp->m_wYear = m_pColumn[wColumnIDX].m_SQL_TIMESTAMP.year;
    pTimeStamp->m_btMon = m_pColumn[wColumnIDX].m_SQL_TIMESTAMP.month;
    pTimeStamp->m_btDay = m_pColumn[wColumnIDX].m_SQL_TIMESTAMP.day;
    pTimeStamp->m_btHour = m_pColumn[wColumnIDX].m_SQL_TIMESTAMP.hour;
    pTimeStamp->m_btMin = m_pColumn[wColumnIDX].m_SQL_TIMESTAMP.minute;
    pTimeStamp->m_btSec = m_pColumn[wColumnIDX].m_SQL_TIMESTAMP.second;

    return true;
}

BYTE*
classODBC::GetDataPTR(WORD wColumnIDX) {
    //	if ( m_pColumn[ btColumn ].m_SqlCType != SQL_BINARY ) return NULL;

    if (wColumnIDX > m_nResultColCnt)
        return NULL;

    if (SQL_NULL_DATA == m_pColumn[wColumnIDX].m_StrLen_or_IndPtr)
        return NULL;

    return m_pColumn[wColumnIDX].m_Data;
}

//-------------------------------------------------------------------------------------------------
char*
classODBC::GetStrPTR(WORD wColumnIDX, bool bSetSpaceToNULL) {
    //	if ( m_pColumn[ btColumn ].m_SqlCType != SQL_CHAR ) return NULL;

    if (wColumnIDX > m_nResultColCnt)
        return NULL;

    if (SQL_NULL_DATA == m_pColumn[wColumnIDX].m_StrLen_or_IndPtr)
        return NULL;
    if (m_pColumn[wColumnIDX].m_ColumnSize >= MAX_ODBC_COLUMN_LEN)
        return NULL;

    if (bSetSpaceToNULL) {
        for (unsigned short wC = 0; wC < m_pColumn[wColumnIDX].m_ColumnSize; wC++) {
            if (m_pColumn[wColumnIDX].m_String[wC] == ' ') {
                m_pColumn[wColumnIDX].m_String[wC] = NULL;
                break;
            }
        }
    } else {
        for (unsigned short wC = m_pColumn[wColumnIDX].m_ColumnSize - 1; wC > 1; wC--) {
            if (m_pColumn[wColumnIDX].m_String[wC] != ' ') {
                // 뒤쪽 공백은 모두 짤라 버림...
                m_pColumn[wColumnIDX].m_String[wC + 1] = NULL;
                break;
            }
        }
    }
    m_pColumn[wColumnIDX].m_String[m_pColumn[wColumnIDX].m_ColumnSize] = NULL;

    return m_pColumn[wColumnIDX].m_String;
}

//-------------------------------------------------------------------------------------------------
int
classODBC::GetStrLen(WORD wColumnIDX) {
    if (wColumnIDX > m_nResultColCnt)
        return 0;

    return m_pColumn[wColumnIDX].m_StrLen_or_IndPtr;
}

//-------------------------------------------------------------------------------------------------
int
classODBC::GetInteger(WORD wColumnIDX) {
    //	if ( m_pColumn[ btColumn ].m_SqlCType != SQL_INTEGER ) return NULL;

    if (wColumnIDX > m_nResultColCnt)
        return 0;

    if (m_pColumn[wColumnIDX].m_StrLen_or_IndPtr < 0)
        return 0;

    return m_pColumn[wColumnIDX].m_SQL_INTEGER;
}

//-------------------------------------------------------------------------------------------------
short
classODBC::GetInteger16(WORD wColumnIDX) {
    //	if ( m_pColumn[ btColumn ].m_SqlCType != SQL_INTEGER ) return NULL;

    if (wColumnIDX > m_nResultColCnt)
        return 0;

    if (m_pColumn[wColumnIDX].m_StrLen_or_IndPtr < 0)
        return 0;

    return m_pColumn[wColumnIDX].m_SQL_SMALLINT;
}

//-------------------------------------------------------------------------------------------------
__int64
classODBC::GetInt64(WORD wColumnIDX) {
    if (wColumnIDX > m_nResultColCnt)
        return 0;

    if (m_pColumn[wColumnIDX].m_StrLen_or_IndPtr < 0)
        return 0;

    return m_pColumn[wColumnIDX].m_SQL_BIGINT;
}

//-------------------------------------------------------------------------------------------------
DWORD
classODBC::MakeQuery(char* szCommand, ...) {
    va_list va;
    va_start(va, szCommand);

    int iStrLen;
    iStrLen = strlen(szCommand);

    ::CopyMemory(m_pQueryBuff, szCommand, iStrLen);
    char* pBuff = m_pQueryBuff + iStrLen;

    MQ_PARAM_TYPE type;
    while ((type = va_arg(va, MQ_PARAM_TYPE)) != MQ_PARAM_END) {
        switch (type) {
            case MQ_PARAM_ADDSTR: {
                char* pStr = va_arg(va, char*);
                iStrLen = strlen(pStr);

                *pBuff++ = ' '; // 공백을 앞에 넣는다.
                ::CopyMemory(pBuff, pStr, iStrLen);
                pBuff += iStrLen;
                *pBuff++ = ' '; // 공백을 뒤에 넣는다.
                break;
            }
            case MQ_PARAM_INT: {
                int iValue = va_arg(va, int);
                sprintf(m_szTemp, "%d", iValue);

                iStrLen = strlen(m_szTemp);
                ::CopyMemory(pBuff, m_szTemp, iStrLen);
                pBuff += iStrLen;
                break;
            }

            case MQ_PARAM_INT16: {
                short nValue = va_arg(va, short);
                sprintf(m_szTemp, "%d", nValue);

                iStrLen = strlen(m_szTemp);
                ::CopyMemory(pBuff, m_szTemp, iStrLen);
                pBuff += iStrLen;
                break;
            }

            case MQ_PARAM_INT64: {
                __int64 i64Value = va_arg(va, __int64);
                // char *_i64toa( __int64 value, char *string, int radix );

                _i64toa(i64Value, m_szTemp, 10);

                iStrLen = strlen(m_szTemp);
                ::CopyMemory(pBuff, m_szTemp, iStrLen);
                pBuff += iStrLen;
                break;
            }
            case MQ_PARAM_FLOAT: {
                double fValue = va_arg(va, double);
                sprintf(m_szTemp, "%f", fValue);

                iStrLen = strlen(m_szTemp);
                ::CopyMemory(pBuff, m_szTemp, iStrLen);
                pBuff += iStrLen;
                break;
            }
            case MQ_PARAM_STR: {
                *pBuff++ = '\'';

                char* pStr = (va_arg(va, char*));
                iStrLen = strlen(pStr);

                // Ralph: Escape quotes, this is a work around and the query should be done using
                // parameter binding instead.
                std::string escaped;
                for (int i = 0; i < iStrLen; ++i) {
                    char c = pStr[i];
                    if (c == '\'') {
                        escaped.push_back('\'');
                    }
                    escaped.push_back(c);
                }

                ::CopyMemory(pBuff, escaped.c_str(), escaped.size());
                pBuff += iStrLen;

                *pBuff++ = '\'';
                break;
            }
            case MQ_PARAM_BINDIDX: {
                /* int iParamIDX = */ va_arg(va, int); // skip bind index..
                /*
                // unsigned long ::mysql_real_escape_string (MYSQL *mysql, char *to, const char
                *from, unsigned long length) *pBuff++= '\''; pBuff += ::mysql_real_escape_string(
                m_pMySQL, pBuff, (const char*)m_pBindPARAM[ iParamIDX ].m_pDATA, m_pBindPARAM[
                iParamIDX ].m_uiDataSize); *pBuff++= '\'';
                */
                *pBuff++ = '?';
                break;
            }
        }
    }
    va_end(va);

    *pBuff = 0;
    OutputDebugString(m_pQueryBuff);
    OutputDebugString("\n\n");

    m_dwQueryBuffSize = pBuff - m_pQueryBuff;

    _ASSERT(m_dwMaxQueryBuffSize > m_dwQueryBuffSize);

    return m_dwQueryBuffSize;
}

std::vector<std::string>
classODBC::get_error_messages(SQLSMALLINT type) {
    return this->get_error_messages(this->m_hSTMT1, type);
}

bool
classODBC::bind(uint32_t idx,
    uint8_t* data,
    uint32_t size,
    SQLSMALLINT c_type,
    SQLSMALLINT sql_type) {

    if (idx >= MAX_BIND_PARAMS) {
        LOG_WARN("Attempting to bind more parameters than the max of %d", MAX_BIND_PARAMS);
        return false;
    }

    param_data[idx].index = idx;
    param_data[idx].size = size;
    param_data[idx].data = std::vector<uint8_t>(data, data + size);
    param_data[idx].size_at_exec = SQL_LEN_DATA_AT_EXEC(size);

    SQLPOINTER parameter_value = (SQLPOINTER)param_data[idx].data.data();
    SQLLEN* strlen_or_indptr = (SQLLEN*)&param_data[idx].size;

    SQLRETURN res = SQLBindParameter(this->m_hSTMT1,
        idx,
        SQL_PARAM_INPUT,
        c_type,
        sql_type,
        size, // ColumnSize
        0, // DecimalDigits
        parameter_value,
        size, // BufferLength
        strlen_or_indptr);

    if (res != SQL_SUCCESS) {
        LOG_WARN("Failed to bind the SQL parameter at index %d", idx);

        for (const std::string& msg: this->get_error_messages()) {
            LOG_WARN(msg.c_str());
        }
        return false;
    }

    return true;
}

bool
classODBC::bind_binary(size_t idx, uint8_t* data, uint32_t size) {
    return this->bind(idx, data, size, SQL_C_BINARY, SQL_BINARY);
}

bool
classODBC::bind_int16(size_t idx, int16_t i) {
    return this->bind(idx, (uint8_t*)&i, sizeof(int16_t), SQL_C_SSHORT, SQL_SMALLINT);
}

bool
classODBC::bind_int32(size_t idx, int32_t i) {
    return this->bind(idx, (uint8_t*)&i, sizeof(int32_t), SQL_C_SLONG, SQL_INTEGER);
}

bool
classODBC::bind_int64(size_t idx, int64_t i) {
    return this->bind(idx, (uint8_t*)&i, sizeof(int64_t), SQL_C_SBIGINT, SQL_BIGINT);
}

bool
classODBC::bind_string(uint32_t idx, const char* data, size_t size) {
    return this->bind(idx, (uint8_t*)data, size, SQL_C_CHAR, SQL_VARCHAR);
}

bool
classODBC::execute(const std::string& query) {
    SQLRETURN res;
    res = SQLCloseCursor(this->m_hSTMT1);
    res = SQLExecDirect(this->m_hSTMT1, (SQLCHAR*)query.c_str(), query.size());

    return res == SQL_SUCCESS || res == SQL_NO_DATA;
}

std::vector<std::string>
classODBC::get_error_messages(SQLHANDLE handle, SQLSMALLINT type) {
    if (this->m_hSTMT1 == SQL_NULL_HSTMT) {
        return {};
    }

    std::vector<std::string> messages;

    SQLRETURN res = SQL_SUCCESS;
    SQLCHAR sql_state[6];
    SQLCHAR sql_msg[2048];
    SQLINTEGER native_error_id;
    SQLSMALLINT sql_msg_len;

    int record_id = 1;
    while (res != SQL_NO_DATA) {
        res = SQLGetDiagRec(type,
            handle,
            record_id,
            sql_state,
            &native_error_id,
            sql_msg,
            255,
            &sql_msg_len);

        if (res == SQL_SUCCESS) {
            messages.push_back((char*)sql_msg);
        } else if (res == SQL_SUCCESS_WITH_INFO) {
            LOG_WARN("Could not get a sql error message because buffer was too small")
        } else if (res == SQL_ERROR) {
            LOG_WARN("An error occurred when trying to get a sql error message");
        } else if (res == SQL_INVALID_HANDLE) {
            LOG_DEBUG("SQLGetDiagRect() called on an invalid handle");
        }

        ++record_id;
    }

    return messages;
}