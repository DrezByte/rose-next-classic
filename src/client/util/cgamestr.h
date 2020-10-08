#ifndef _CGAMESTR_
#define _CGAMESTR_

#include "util/classstr.h"

/// CSTR for VFS

class CGameStr: public CStr {
public:
    CGameStr(){};
    ~CGameStr(){};

    static char* ReadString(CFileSystem* pFileSystem, bool bIgnoreWhiteSpace = true);
};

#endif //_CGAMESTR_