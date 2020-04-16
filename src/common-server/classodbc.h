#pragma once

#include <string>
#include <vector>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include <odbcss.h>

#include "classSQL.h"
#include "classSTR.h"

#define MAX_ODBC_COLUMN_CNT 64
#define MAX_ODBC_COLUMN_LEN 4096
#define MAX_ODBC_COLUMN_NAME 64

#define MAX_ODBC_BIND_PARAMETER 64
#define MAX_ODBC_QUERY_BUFFER 8192

/// ODBC bind parameter data
struct ParamData {
    uint32_t index;
    uint32_t size;
    std::vector<uint8_t> data;
    int32_t size_at_exec;
};

/// ODBC fetch result
enum class FetchResult {
    Ok,
    NoData,
    Error,
};

struct tagODBCCOL {
    SQLCHAR m_Name[MAX_ODBC_COLUMN_NAME];
    SQLSMALLINT m_NameLen;
    SQLSMALLINT m_SqlCType;
    SQLUINTEGER m_ColumnSize;
    SQLINTEGER m_StrLen_or_IndPtr;
    union {
        char m_String[MAX_ODBC_COLUMN_LEN + 1];
        BYTE m_Data[MAX_ODBC_COLUMN_LEN + 1];
        TIMESTAMP_STRUCT m_SQL_TIMESTAMP;
        SQLINTEGER m_SQL_CHAR;
        SQLINTEGER m_SQL_INTEGER;
        SQLINTEGER m_SQL_SMALLINT;
        SQLINTEGER m_SQL_REAL;
        SQLINTEGER m_SQL_DOUBLE;
        __int64 m_SQL_BIGINT;
    };
};

struct tagODBCDATA {
    union {
        BYTE* m_pDATA;
        bool m_bVal;
        char m_chVal;
        short m_nVal;
        long m_lVal;
        float m_fVal;
        double m_dblVal;
    };
    SQLUINTEGER m_uiDataSize;
    SQLINTEGER m_StrLen_or_Ind;
};

class classODBC: public classSQL {

public:
    CStrVAR m_ErrMSG;
    short m_nBindParamCNT;
    void AllocParam(short nCntToAlloc);
    tagODBCDATA* m_pBindPARAM;

    short m_nAllocColCnt;
    void AllocColumn(short nCntToAlloc);

    tagODBCCOL* m_pColumn;

    SQLHENV m_hENV;
    SQLHDBC m_hDBC1;
    SQLHSTMT m_hSTMT1;

    SQLSMALLINT m_nResultColCnt;
    SQLINTEGER m_iResultRowCnt; // Update, Delete, Insert명령시 영향받은 레코드
                                // 갯수를 구해온다

    /// Maximum number of parameters that can be bound to one statement
    const uint32_t MAX_BIND_PARAMS = 50;

    /// Data for currently bound parameters
    std::vector<ParamData> param_data;

public:
    RETCODE m_RetCode;

    classODBC(short nBindParamCNT = MAX_ODBC_BIND_PARAMETER,
        WORD wQueryBufferLEN = MAX_ODBC_QUERY_BUFFER);
    ~classODBC();

    //	bool		QuerySP  (char *szFormat, ...);
    bool QuerySQL(char* szFormat, ...);
    int ExecSQL(char* szFormat, ...);

    void Clear(void);

    SQLSMALLINT GetColDataType(BYTE btColumn) { return m_pColumn[btColumn].m_SqlCType; }

    bool FirstDSN(char* pszDSN, int lMax);
    bool NextDSN(char* pszDSN, int lMax);
    bool RegisterDSN(char* szDSN, char* szDBName, char* szServerIP, char* szUser);
    bool ReigsterDsnIfNone(char* szDSN, char* szDBName, char* szServerIP, char* szUser);

    // inherited from classSQL
    bool Connect(char* szDSN, char* szUserName, char* szPassword);
    void Disconnect(void);
    bool SelectDB(char* szDBName) { return true; }

    const char* GetERROR();

    int GetRecordCNT() { return (int)m_iResultRowCnt; }
    int GetColumnCNT() { return m_nResultColCnt; }

    bool Fetch();
    bool BindPARAM(short nParamIDX, BYTE* pData, unsigned int uiDataSize);

    DWORD MakeQuery(char* szCommand, ...);
    int ExecSQLBuffer();
    int ExecSQLPacket(char* szQuery, ULONG ulLength) { return 0; }

    int ExecSQLBuffer(char* szQuery);
    int ExecSQLBuffer(char* szQuery, unsigned long ulLength);

    bool QueryString(char* szQuery, unsigned long ulLength);
    bool QueryString(char* szQuery);

    bool QuerySQLBuffer(/* bool bBindResult=true */);

    BYTE* GetDataPTR(WORD wColumn);
    char* GetStrPTR(WORD wColumn, bool bSetSpaceToNULL = true);
    int GetStrLen(WORD wColumn);
    int GetInteger(WORD wColumn);
    short GetInteger16(WORD wColumnIDX); // __KCHS_BATTLECART__
    __int64 GetInt64(WORD wColumn);
    bool GetTimestamp(WORD wColumnIDX, sqlTIMESTAMP* pTimeStamp);

    bool BindRESULT();
    bool GetMoreRESULT() { return (::SQLMoreResults(this->m_hSTMT1) != SQL_NO_DATA); }

    bool SetParameter(short nParamIDX,
        BYTE* pData,
        UINT uiDataSize,
        short nIOType,
        short nValueType,
        short nParamType);

    bool SetParam_string(short nParamIDX, char* szStr);
    bool SetParam_wstring(short nParamIDX, char* szStr);
    bool SetParam_long(short nParamIDX, long& lOutResult, long& cbLen);

    /// Bind parameters
    bool bind(uint32_t idx, uint8_t* data, uint32_t size, SQLSMALLINT c_type, SQLSMALLINT sql_type);
    bool bind_binary(size_t idx, uint8_t* blob, uint32_t size);
    bool bind_int16(size_t idx, int16_t i);
    bool bind_int32(size_t idx, int32_t i);
    bool bind_int64(size_t idx, int64_t i);
    bool bind_string(size_t idx, const char* data, size_t size);
    bool bind_string(size_t idx, const std::string& data);

    /// Execute a query
    bool execute(const std::string& query);

    /// Get a row returned from the last query
    FetchResult fetch();

    /// Gets the maximum or actual character length of a character string or binary column.
    /// It is the maximum character length for a fixed-length data type, or the actual
    /// character length for a variable-length data type. Its value always excludes
    /// the null-termination byte that ends the character string.
    int column_length(size_t idx);

    /// Get a columns value from the last row
    std::vector<uint8_t> get_binary(size_t col);
    int16_t get_int16(size_t col);
    int32_t get_int32(size_t col);
    int64_t get_int64(size_t col);
    std::string get_string(size_t col);

    // TODO: Checked versions?
    // e.g. bool get_binary_checked(size_t col, std::vector<uint8_t>& val);

    // Get a list of error message strings
    std::vector<std::string> get_error_messages(SQLHANDLE handle, SQLSMALLINT type);
    std::vector<std::string> get_error_messages(SQLSMALLINT type = SQL_HANDLE_STMT);
};
