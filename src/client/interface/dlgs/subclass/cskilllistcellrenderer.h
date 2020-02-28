#ifndef _CSkillListCellRenderer_
#define _CSkillListCellRenderer_
#include "tgamectrl/ilistcellrenderer.h"

class CSkillListCellRenderer: public IListCellRenderer {
public:
    CSkillListCellRenderer(void);
    virtual ~CSkillListCellRenderer(void);
    virtual CWinCtrl*
    GetListCellRendererComponent(const CTObject* pObject, bool selected, bool mouseover);
};
#endif