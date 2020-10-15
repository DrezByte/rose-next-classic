#include "stdafx.h"

#include "VFSManager.h"
#include "CFileSystemNormal.h"
#include "CFileSystemTriggerVFS.h"

#include "rose/io/reader.h"
#include "rose/io/stb.h"

#include <algorithm>

using namespace Rose::IO;

CVFSManager __SingletonVFSManager;

CVFSManager::CVFSManager() {
    // Construct
    m_hVFile = NULL;
    m_iVFSType = VFS_NORMAL;
}

CVFSManager::~CVFSManager() {
    ReleaseAll();
}

/// Get current file system
CFileSystem*
CVFSManager::GetNewFileSystem(int iVFSType) {
    CFileSystem* pFileSystem = NULL;

    switch (iVFSType) {
        case VFS_NORMAL: {
            pFileSystem = (CFileSystem*)new CFileSystemNormal();
        } break;
        case VFS_TRIGGER_VFS: {
            if (m_hVFile == NULL) {
                pFileSystem = (CFileSystem*)new CFileSystemNormal();
                break;
            }

            CFileSystemTriggerVFS* pFsVFS = new CFileSystemTriggerVFS();
            pFsVFS->SetVFS(m_hVFile);
            pFileSystem = (CFileSystem*)pFsVFS;

        } break;
    }

    return pFileSystem;
}

bool
CVFSManager::InitVFS(int iVFSType, int iReserveCount /*=10*/) {
    ReleaseAll();

    m_iVFSType = iVFSType;

    if (iVFSType == VFS_TRIGGER_VFS && m_hVFile == NULL) {
        if (g_pCApp)
            g_pCApp->ErrorBOX("���� VFS�� �����Ͻÿ�", "ERROR");
        return false;
    }

    CFileSystem* pFileSystem = NULL;
    for (int i = 0; i < iReserveCount; i++) {
        pFileSystem = GetNewFileSystem(iVFSType);

        if (pFileSystem == NULL) {
            ReleaseAll();
            return false;
        }

        m_VFSList.push_back(pFileSystem);
    }

    return true;
}

/// util functio for for_each
void
ReleaseSingleVFS(CFileSystem* pFileSystem) {
    if (pFileSystem != NULL) {
        delete pFileSystem;
        pFileSystem = NULL;
    }
}

void
CVFSManager::ReleaseAll() {
    std::for_each(m_VFSList.begin(), m_VFSList.end(), ReleaseSingleVFS);
    std::for_each(m_VFSUsedList.begin(), m_VFSUsedList.end(), ReleaseSingleVFS);

    m_VFSList.clear();
    m_VFSUsedList.clear();
}

CFileSystem*
CVFSManager::GetFileSystem() {
    CFileSystem* pFileSystem = NULL;

    if (m_VFSList.empty()) {
        pFileSystem = GetNewFileSystem(m_iVFSType);
        m_VFSList.push_back(pFileSystem);
    }

    pFileSystem = m_VFSList.back();
    m_VFSList.pop_back();

    m_VFSUsedList.push_back(pFileSystem);

    return pFileSystem;
}

void
CVFSManager::ReturnToManager(CFileSystem* pFileSystem) {
    m_VFSList.push_back(pFileSystem);

    m_VFSUsedList.remove(pFileSystem);
}

bool
CVFSManager::IsExistFile(const char* pFileName) {
    if (pFileName == NULL)
        return false;

    CFileSystem* pFileSystem = GetFileSystem();
    if (pFileSystem == NULL)
        return false;

    bool res = pFileSystem->IsExist(pFileName);
    ReturnToManager(pFileSystem);
    return res;
}

bool
CVFSManager::load_stb(STBDATA& stb, const std::filesystem::path& path) {
    if (path.empty()) {
        return false;
    }

    std::string filepath = path.string();
    CFileSystem* fs = this->GetFileSystem();
    if (!fs || !fs->IsExist(filepath.c_str())) {
        return false;
    }

    if (!fs->OpenFile(filepath.c_str())) {
        return false;
    }

    if (!fs->ReadToMemory()) {
        return false;
    }

    std::byte* ptr = reinterpret_cast<std::byte*>(fs->GetData());
    std::vector<std::byte> data(ptr, ptr + fs->GetSize());

    fs->ReleaseData();
    fs->CloseFile();
    this->ReturnToManager(fs);

    BinaryReader b;
    if (!b.open(std::move(data))) {
        return false;
    }
    return stb.load(std::move(b));
}